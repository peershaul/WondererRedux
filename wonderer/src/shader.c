#include <GL/glew.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>

#include "../include/mem_utils.h"
#include "../include/logger.h"

typedef struct {
  const char *vert_path, *frag_path;
  GLuint id;
} Shader;

#define WONDERER_MULTI_SHADER_SRC
#include "../include/shader.h"

static dynamic_arr shader_pool;
static Shader* bound = NULL;

void wondererShaderLoaderInit() {
  wondererDynamicArrayInitialize(4, sizeof(Shader), &shader_pool);
}

const char *get_shader_type_str(GLenum type) {
  switch(type){
  case GL_VERTEX_SHADER: return "Vertex Shader";
  case GL_FRAGMENT_SHADER: return "Fragment Shader";
  default: return "Undefined Shader Type";
  }
}

static bool starts_with(const char *neadle, const char *haystack, 
						int neadle_length, int haystack_length){
	if(neadle_length > haystack_length) return false;
	int i = 0, o = 0;

	while(i < neadle_length){
		if(i == 0 && haystack[o] == ' '){
			o++;
			continue;
		}
		if(neadle[i] == haystack[o + i])
			i++;
		else return false;
	} 
	return true;
}

static int find_char(char c, const char* text, int text_length){
	for(int i = 0; i < text_length; i++)
		if(text[i] == c)
			return i;
	return -1;
}

static int extract_link(const char* line, char *dest, int line_length){
	int link_start = find_char('"', line, line_length);
	int link_end = find_char('"', &line[link_start + 1], line_length - link_start - 1) + link_start + 1;

	int dest_length = 0;
	for(int i = link_start + 1; i < link_end; i++)
		dest[dest_length++] = line[i];		
	dest[dest_length++] = '\0';
	return dest_length;
}

static void fetch_source(const char* path, dynamic_arr *dest, 
						 bool is_top_level){

	FILE *f = fopen(path, "r");

	if(f == NULL){
		WondererError("Cannot open file: \"%s\"\n", path);

		exit(0);
	}

	fseek(f, 0, SEEK_END);
	uint64_t file_length = ftell(f);
	fseek(f, 0, SEEK_SET);

	if(is_top_level)
		wondererDynamicArrayInitialize(file_length, sizeof(char), dest);
	else 
		wondererDynamicArrayIncreaseSize(dest->length + file_length, dest);

	char line[128], c;
	const char* include_word = "#include";
	int line_index = 0;
	const int include_word_length = strlen(include_word);

	char* dest_arr = dest->arr;
	
	// Trying to see if the file includes other files and if so includes them
	for(int i = 0; i < file_length; i++){
		c = fgetc(f);	

		// Checking for end of line condition
		if(c == '\n' || i == file_length - 1){
			// Null-Terminating the line
			line[line_index] = '\0';

			// Searching for '#includes'
			if(starts_with(include_word, line, include_word_length, 
						   include_word_length)){
				char sub_path[128] = "";

				// Extracting the path from the file
				extract_link(line, sub_path, line_index + 1);
				
				// Closing the file and saving its location
				uint64_t current_location = ftell(f);	
				fclose(f);

				// Recalling this function to fetch the source of the file that is included
				fetch_source(sub_path, dest, false);

				// Reopening the file in this instance of the function in order to continue reading
				f = fopen(path, "r");
				fseek(f, current_location, SEEK_SET);
			}
			else {	
				// Returning to the line its original line terminator
				line[line_index++] = i == file_length - 1 && is_top_level? '\0' : '\n'; 

				// Copying the line to the destination
				memcpy(&dest_arr[dest->last], line, line_index);
				dest->last += line_index;
			}

			line_index = 0;
		}
		else line[line_index++] = c;
		if(c == '\0'){
			printf("REACHED THE END!\n");
			break;
		}
	}
	fclose(f);
}

static GLuint compile_shader(GLenum shader_type, const char *shader_path) {

	dynamic_arr src_code = {};	
	fetch_source(shader_path, &src_code, true);
	GLuint shader = glCreateShader(shader_type);

	const char* src_ptr = src_code.arr;

	glShaderSource(shader, 1, &src_ptr, NULL);
	glCompileShader(shader);

	GLint shader_compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);
	if(shader_compiled != GL_TRUE){
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(shader, 1024, &log_length, message);
		WondererError("ERROR IN SHADER COMPILATION\nPath: \"%s\" Type: %s\n%s", shader_path,
			get_shader_type_str(shader_type), message);
		exit(0);
	return 0;
	}
	
	free(src_code.arr);

	return shader;
}

static Shader shader_create(const char *vert_path, const char *frag_path) {
  GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, vert_path);
  GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER, frag_path);
  if(frag_shader == 0 || vert_shader == 0)
    return (Shader){};

  GLuint program = glCreateProgram();
  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);
  glLinkProgram(program);

  GLint linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if(linked != GL_TRUE){
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetProgramInfoLog(program, 1024, &log_length, message);
    
		printf("SHADER LINK ERROR\nVertex Path: \"%s\" Fragment Path: \"%s\"\n%s\n",
		  vert_path, frag_path, message);
		exit(0);
    return (Shader){};
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);
  return (Shader){.vert_path = vert_path, .frag_path = frag_path, .id = program};
}

Shader *wondererShaderCreate(const char *vert_path, const char *frag_path) {
  Shader* arr = shader_pool.arr;
  int32_t empty_space = -1;

  for(uint16_t i = 0; i < shader_pool.last; i++)
    if(arr[i].id == 0){
      empty_space = i;
      break;
    }

  if(empty_space != -1){
    arr[empty_space] = shader_create(vert_path, frag_path);
    Shader* ret = &arr[empty_space];

    return ret; 
  }

  if(shader_pool.last >= shader_pool.length){
    wondererDynamicArrayIncreaseSize(shader_pool.length * 2, &shader_pool);
    arr = shader_pool.arr;
  }

  arr[shader_pool.last] = shader_create(vert_path, frag_path);
  Shader* ret = &arr[shader_pool.last++];

  return ret; 
}

Shader *wondererShaderGet(const char *vert_path, const char *frag_path) {
  Shader* arr = shader_pool.arr;
  int32_t found = -1;
  for(uint16_t i = 0; i < shader_pool.last; i++)
    if(strcmp(vert_path, arr[i].vert_path) == 0 && strcmp(frag_path, arr[i].frag_path) == 0){
      found = i;
      break;
    }

  if(found != -1){
    Shader* ret = &arr[found];
    WondererLog("Found Existing shader \"%s\" \"%s\"", vert_path, frag_path);
    return ret;
  }

  return wondererShaderCreate(vert_path, frag_path);
}

void wondererShaderDestroy(Shader *shader) {
  glDeleteProgram(shader->id);
  *shader = (Shader){};
}

void wondererShaderBind(Shader *shader) {
  bound = shader;
  glUseProgram(shader->id);
}

void wondererShaderUnbind() {
  bound = NULL;
  glUseProgram(0);
}

GLuint wondererShaderGetID(Shader *shader) { return shader->id; }

void wondererShaderLoaderTerminate() {
  wondererShaderUnbind();
  
  Shader* arr = shader_pool.arr;
  for(uint16_t i = 0; i < shader_pool.last; i++)
    glDeleteProgram(arr[i].id);
  
  free(shader_pool.arr);
  shader_pool = (dynamic_arr){};
}

void wondererShaderReload(Shader *shader) {
  const char* vert_path = shader->vert_path, *frag_path = shader->frag_path;
  bool is_bound = bound == shader;
  wondererShaderDestroy(shader);
  *shader = shader_create(vert_path, frag_path);
  if(bound)
    wondererShaderBind(shader);
}

void wondererShaderReloadAll() {
  Shader* arr = shader_pool.arr;
  for(uint16_t i = 0; i < shader_pool.last; i++){
    if(arr[i].id == 0) continue;
    wondererShaderReload(&arr[i]);
  }
}
