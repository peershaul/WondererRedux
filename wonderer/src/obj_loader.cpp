#include "../include/obj_loader.h"

extern "C"{

#include "../include/mem_utils.h"
#include "../include/drawer.h"

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

}

#include <vector> // I know right!?

static const char *valid_prefixes[] = {
   "v",
   "vt",
   "vn",
};

static int stride;

#define STATE_COUNT (sizeof(valid_prefixes) / sizeof(char*))


static void load_vertex_layout(int *vertex_layout, ObjFormat *format, 
                               int format_length){

   for(int i = 0; i < format_length; i++)
      vertex_layout[format[i].type] = format[i].dimentions;   

}

static ObjFormatType find_prefix(char *prefix){

   for(int i = 0; i < STATE_COUNT; i++)
      if(strcmp(valid_prefixes[i], prefix) == 0)
         return (ObjFormatType) i;

   return (ObjFormatType) -1;
}

static int find_in_attrib_indices(std::vector<int> all_attrib_indices, 
                                  int* attrib_indices){
   
   for(int i = 0; i < all_attrib_indices.size() / STATE_COUNT; i++){
      bool match = true;
      for(int j = 0; j < STATE_COUNT; j++)
         if(all_attrib_indices[STATE_COUNT * i + j] != attrib_indices[j]){
            match = false;
            break;
         }
      if(match) return i;
   }
   return -1;
}


static void read_obj_file(const char *path, int *vertex_layout, 
                          ObjFormat *format, int format_length,
                          std::vector<float> *object_vertices, 
                          std::vector<uint16_t> *object_indices){

   std::vector<float> vertex_attribs[STATE_COUNT] = {}; 

   FILE *f = fopen(path, "r"); 
   
   fseek(f, 0, SEEK_END);
   uint64_t file_size = ftell(f);
   fseek(f, 0, SEEK_SET);

   printf("file size: %ld\n", file_size);

   char line[1024] = "";
   int line_index = 0;

   int space_count = 0;
   int last_line = 0;
   bool skip = false;

   float values[4] = {};
   ObjFormatType curr_type = WONDERER_OBJ_FORMAT_NONE;
   
   for(uint64_t i = 0; i < file_size; i++){
      char c = fgetc(f);
      
      if(c == '\n' || c == '\0'){
         line[line_index] = '\0';

         if(!skip){
            values[space_count - 1] = atof(line);
            for(int i = 0; i < vertex_layout[curr_type]; i++)
               vertex_attribs[curr_type].push_back(values[i]);
         } 

         space_count = 0;
         line_index = 0;
         last_line = i + 1;
         skip = false;
      }
      else if(c == ' ' && !skip){

         line[line_index] = '\0';
         if(space_count == 0){
            curr_type = find_prefix(line);
            if(curr_type == -1) {
               skip = true;
               if(strcmp("f", line) == 0) break;
            }
         } 
         else 
            values[space_count - 1] = atof(line);
         space_count++;
         line_index = 0;
      }
      else if(!skip) line[line_index++] = c;
      
   }

   space_count = 0;
   line_index = 0;
   skip = false;
   int attrib_indices[STATE_COUNT] = {}; 
   int attrib_indices_index = 0;
   int index_length = 0;

   std::vector<int> all_attrib_indices = {};
   *object_indices = {};

   fseek(f, last_line, SEEK_SET);

   for(int i = last_line; i < file_size; i++){
      char c = fgetc(f);
      bool is_space = c == ' ' && !skip;

      if(is_space || c == ' ' || i == file_size - 1){

         line[line_index] = '\0';
         
         if(space_count == 0)
            skip = strcmp(line, "f") != 0;
         else{
            attrib_indices[attrib_indices_index] = atoi(line) - 1;
            int index = find_in_attrib_indices(all_attrib_indices, attrib_indices);
            if(index == -1){

               for(int j = 0; j < STATE_COUNT; j++)
                  all_attrib_indices.push_back(attrib_indices[j]);
               object_indices->push_back(index_length++);

            }
            else object_indices->push_back(index);
         }
         
         if(is_space)
            space_count++;
         else {
            skip = false;
            space_count = 0;
         }
         attrib_indices_index = 0;
         line_index = 0;
      }
      else if(c == '/' && !skip){
         line[line_index] = '\0';
         attrib_indices[attrib_indices_index++] = atoi(line) - 1;
         line_index = 0;
      }
      else if(!skip) line[line_index++] = c;
   }

   *object_vertices = {};

   for(int i = 0; i < all_attrib_indices.size() / STATE_COUNT; i++){
      for(int j = 0; j < format_length; j++){

         ObjFormatType type = format[j].type;
         int dimentions = format[j].dimentions;
         int attrib_index = all_attrib_indices[i * 3 + type];

         // TODO: vertex_attribs has data only on its first element 
         //       on WONDERER_OBJ_FORMAT_LOCATION 
         //       the remaining vectors are null
         for(int h = 0; h < dimentions; h++)
            object_vertices->push_back(vertex_attribs[type][attrib_index * dimentions + h]);
      }
   }

   for(int i = 0; i < all_attrib_indices.size() / 3; i++){
      for(int j = 0; j < 3; j++)
         printf("% 2d\t", all_attrib_indices[i * 3 + j]);
      printf("\n");
   } 

   for(int i = 0; i < STATE_COUNT; i++){
      int dimentions = vertex_layout[i];
      printf("type: %d\n", i);
      for(int j = 0; dimentions != 0 && j < vertex_attribs[i].size() / dimentions; j++){
         printf("\tindex: %d, ", j);
         for(int h = 0; h < dimentions; h++)
            printf("\t% .3f", vertex_attribs[i][j * dimentions + h]);
         printf("\n");
      }
   }

   fclose(f);
}


void wondererObjLoad(const char *path, DrawData *dest, ObjFormat *format, 
                     int format_length){

   int vertex_layout[STATE_COUNT] = {};
   load_vertex_layout(vertex_layout, format, format_length);

   std::vector<uint8_t> layout_sections = {};
   for(int i = 0; i < format_length; i++)
      layout_sections.push_back(format[i].dimentions);

   std::vector<uint16_t> indices;
   std::vector<float> vertices;
   read_obj_file(path, vertex_layout, format, format_length, 
                 &vertices, &indices);

   dest->number_of_indices = (uint16_t) indices.size();
   wondererDrawerGenerateLayout(dest, vertices.data(), vertices.size() * sizeof(float), 
                                indices.data(), indices.size() * sizeof(uint16_t),
                                layout_sections.data(), layout_sections.size());
   
}

