#ifndef _DEBUG_DRAW_H_
#define _DEBUG_DRAW_H_

#include "vec.h"

void draw_offset(vec3 offset);
void draw_color(color3 c);
void draw_colors(color3 start, color3 end);
void draw_color_start(color3 start);
void draw_color_end(color3 end);
void draw_scale(float s);

void draw_point(vec3 p);
void draw_circle(vec3 p, float radius);
void draw_line(vec3 a, vec3 b);
void draw_line_solid(vec3 a, vec3 b, color3 color);
void draw_vector(vec3 v);
void draw_dir(vec3 v);
void draw_rect(vec3 pos, vec3 a, vec3 b);

void draw_render();

void draw_cleanup();

#endif
