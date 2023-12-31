#version 450 

in vec2 tex_coords;

out vec4 fragColor;

uniform sampler2D frame;
uniform int paint;
uniform vec3 threshes;

void main(){
   float lum = texture(frame, tex_coords).x;
   vec3 blue = vec3(0.25, 0.5, 0.8);
   vec3 yellow = vec3(0.93, 0.95, 0.47);
   vec3 green = vec3(0.5, 0.8, 0.3);
   vec3 grey = vec3(0.9, 0.9, 0.9);

   float high_thresh = threshes.z;
   float mid_thresh = threshes.y;
   float low_thresh = threshes.x;


   // if(lum <= low_thresh)
   //    color = blue;
   // else if(lum <= high_thresh)
   //    color = mix(blue, green, (lum - low_thresh) / (high_thresh - low_thresh));
   // else 
   //    color = mix(green, grey, (lum - high_thresh) / (1 - high_thresh));
   // 
   if(paint == 0){
      vec3 color;

      if (lum <= low_thresh)
         color = blue;
      else if (lum <= mid_thresh)
         color = yellow;
      else if(lum <= high_thresh)
         color = green;
      else 
         color = grey;

      fragColor = vec4(color, 1);
   }
   else fragColor = vec4(0, lum >= 0.5? lum : 0, lum < 0.5? lum + 0.5 : 0, 1);
    
}
