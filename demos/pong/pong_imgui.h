#ifndef PONG_IMGUI_H
#define PONG_IMGUI_H

#ifdef __cplusplus
extern "C" {
#endif

  #include <cglm/cglm.h>
  #include <stdbool.h>
  #include <stdint.h>

  struct InspectorData{
    bool show;
    float *dt;
    uint16_t *window_dimentions;
    vec3 bc_color;
  };

  /**
     updates and shows the inspector imgui window
     [in, out] struct InspectorData *data: the inspector data
   */
  void pong_show_inspector_window(struct InspectorData *data);

#ifdef __cplusplus
}
#endif

#endif 
