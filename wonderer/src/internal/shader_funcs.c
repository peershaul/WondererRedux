#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <memory.h>

#include "../../include/logger.h"
#include "../../include/mem_utils.h"

static const char *get_shader_type_str(GLenum type) {
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
      if(i == 0 && haystack[o] == ' ')
         o++;
      else if(neadle[i] == haystack[o + i])
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

