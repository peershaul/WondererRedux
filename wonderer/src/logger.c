#include "../include/logger.h"

#include <GL/glew.h> 

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

static WondererLogLevel log_level = LEVEL_INFO;

void wonderer_logger_out(WondererLogLevel level, const char *file, int line,
                         const char *func, const char *message, ...) {

  if(log_level > level) return;

  else if (level == LEVEL_NONE){
    WondererLog("You cant log with error_log = LEVEL_NONE");
    return;
  }
  
  va_list list;
  int ret;

  char buffer[255];

  va_start(list, message);
  ret = vsprintf(buffer, message, list);
  va_end(list);

  if(ret < 0) WondererError("Failed to display error message");

  const char* modes[] = {
    "INFO",
    "WARNING",
    "ERROR"
  };

  const uint8_t color_modes[] = {
    37,
    33,
    31
  };

  
  // Detecting new lines
  uint8_t new_line_count = 0;
  uint8_t text_index = 0;

  while(buffer[text_index] != '\0')
    if(buffer[text_index++] == '\n')
      new_line_count++;

  if(new_line_count > 0){
    uint8_t old_length = strlen(buffer);
    uint16_t new_length = old_length + new_line_count;
    uint16_t new_index = 0;

    char new_buffer[new_length];
    for(uint8_t i = 0; i < old_length; i++){
      new_buffer[new_index++] = buffer[i];
      if(buffer[i] == '\n')
	new_buffer[new_index++] = '\t';
    }
    new_buffer[new_index] = '\0';

    fprintf(level > LEVEL_WARNING? stderr : stdout, "\e[4;%dm[%s:%d] (%s) %s:\e[0m\n\t%s\n",
	    color_modes[level], file, line, func, modes[level], new_buffer);
  }
  else
    fprintf(level > LEVEL_WARNING? stderr : stdout, "\e[4;%dm[%s:%d] (%s) %s:\e[0m %s\n",
	    color_modes[level], file, line, func, modes[level], buffer);
}


void wondererSetLogLevel(WondererLogLevel level) {
  if(level <= LEVEL_NONE)
    log_level = level;
}

WondererLogLevel wondererGetLogLevel() {
  return log_level;
}

static void openglErrorCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  char* type_msg;
  char* severity_msg;
  char* source_msg;

  switch(type){

  case GL_DEBUG_TYPE_ERROR:
    type_msg = "Debug Error";
    break;
    
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    type_msg = "Deprecated Behavior";
    break;
    
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    type_msg = "Undefined Behavior";
    
  case GL_DEBUG_TYPE_PORTABILITY:
    type_msg = "Portability";
    break;
    
  case GL_DEBUG_TYPE_PERFORMANCE:
    type_msg = "Performance";
    break;
    
  case GL_DEBUG_TYPE_OTHER:
    type_msg = "Other";
    break;
    
  }

  switch(severity){
    
  case GL_DEBUG_SEVERITY_LOW:
    severity_msg = "Low";;
    break;
    
  case GL_DEBUG_SEVERITY_MEDIUM:
    severity_msg = "Medium";
    break;
    
  case GL_DEBUG_SEVERITY_HIGH:
    severity_msg = "High";
    break;

  }


  switch(source){
    
  case GL_DEBUG_SOURCE_API:
    source_msg = "Source API";
    break;
    
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    source_msg = "Window System";
    break;
    
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    source_msg = "Shader Compiler";
    break;
    
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    source_msg = "Third Party";
    break;
    
  case GL_DEBUG_SOURCE_APPLICATION:
    source_msg = "Application";
    break;
    
  case GL_DEBUG_SOURCE_OTHER:
    source_msg = "Other";
    break;
    
  }

  WondererError("OPENGL ERROR\nsource: %s\ntype: %s\nseverity: %s\ndescription: %s",
		source_msg, type_msg, severity_msg, message);
  
}


void wondererInitGLLogger() {
  WondererLog("Initializing OpenGL logger...");
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(openglErrorCallback, NULL);
  GLuint unusedIds = 0;
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);
  WondererLog("OpenGL logger initialized!");
}

void wonderer_gl_clear_errors() {
  GLenum error = glGetError();
  while(error != GL_NO_ERROR) error = glGetError();
}

void wonderer_gl_print_errors(const char *file, int line, const char *func) {
  GLenum error = glGetError();

  while(error != GL_NO_ERROR){
    wonderer_logger_out(LEVEL_ERROR, file, line, func, "OPENGL ERROR: %x (%d)", error, error);
  }
}
