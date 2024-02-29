#include "image.h"

#include <string.h>

extern int  js_image_open(const char* src, uint src_len);
extern void js_image_open_async(int data_id);
extern uint js_image_width(int data_id);
extern uint js_image_height(int data_id);
extern void js_image_delete(int data_id);

void image_open_async(Image* image, const char* filename) {
  image->handle = js_image_open(filename, strlen(filename));
  image->ready = 0;
  js_image_open_async(image->handle);
}

uint image_width(const Image* image) {
  return js_image_width(image->handle);
}

uint image_height(const Image* image) {
  return js_image_height(image->handle);
}

void image_delete(Image* image) {
  js_image_delete(image->handle);
}
