#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "transformations.h"
#include "bmp.h"

#define HEADER_SIZE 54

struct bmp_header* cpy_header(struct bmp_header* og) {
  struct bmp_header* cpy = calloc(HEADER_SIZE, 1);
  cpy->type = og->type;
  cpy->size = og->size;
  cpy->reserved1 = og->reserved1;
  cpy->reserved2 = og->reserved2;
  cpy->offset = og->offset;
  cpy->dib_size = og->dib_size;
  cpy->width = og->width;
  cpy->height = og->height;
  cpy->planes = og->planes;
  cpy->bpp = og->bpp;
  cpy->compression = og->compression;
  cpy->image_size = og->image_size;
  cpy->x_ppm = og->x_ppm;
  cpy->y_ppm = og->y_ppm;
  cpy->num_colors = og->num_colors;
  cpy->important_colors = og->important_colors;
  return cpy;
}

struct bmp_image* flip_horizontally(const struct bmp_image* image) {
  if (!image) {
    return NULL;
  }
  size_t pixel_num = image->header->width*image->header->height;
  struct bmp_image* mod_image = calloc(
    sizeof(struct bmp_header)+pixel_num*(sizeof(struct pixel)), 1
  );
  mod_image->header = cpy_header(image->header);
  struct pixel* pixels = calloc(pixel_num, sizeof(struct pixel));

  for (unsigned int i = 0; i < pixel_num; i+=image->header->width) {
    for (unsigned int j = 0; j < image->header->width; j++) {
      pixels[i+j] = image->data[i+image->header->width-1-j];
    }
  }
  mod_image->data = pixels;
  return mod_image;
}

struct bmp_image* flip_vertically(const struct bmp_image* image) {
  if (!image) {
    return NULL;
  }
  size_t pixel_num = image->header->width*image->header->height;
  struct bmp_image* mod_image = calloc(
    sizeof(struct bmp_header)+pixel_num*(sizeof(struct pixel)), 1
  );
  mod_image->header = cpy_header(image->header);
  struct pixel* pixels = calloc(pixel_num, sizeof(struct pixel));

  for (unsigned int i = 0; i < image->header->width; i++) {
    for (unsigned int j = 0; j < pixel_num; j+=image->header->width) {
      pixels[i+j] = image->data[i+(pixel_num-image->header->width)-j];
    }
  }
  mod_image->data = pixels;
  return mod_image;
}

struct bmp_image* rotate_right(const struct bmp_image* image) {
  if (!image) {
    return NULL;
  }
  unsigned int padding = image->header->height%4;
  size_t pixel_num = image->header->width*image->header->height;
  struct bmp_image* mod_image = calloc(
    sizeof(struct bmp_header)+pixel_num*(sizeof(struct pixel)), 1
  );

  mod_image->header = cpy_header(image->header);
  mod_image->header->width = image->header->height;
  mod_image->header->height = image->header->width;
  mod_image->header->size = (unsigned int)(HEADER_SIZE+pixel_num*3+padding*image->header->width); 
  mod_image->header->image_size = (unsigned int)mod_image->header->size-HEADER_SIZE;

  struct pixel* pixels = calloc(pixel_num, sizeof(struct pixel));
  int k = 0;
  for (unsigned int i = image->header->width-1; i > 0; i--) {
    for (unsigned int j = 0; j < pixel_num; j+=image->header->width) {
      pixels[k++] = image->data[i+j];
    }
  }
  for (unsigned int j = 0; j < pixel_num; j+=image->header->width) {
    pixels[k++] = image->data[j];
  }
  mod_image->data = pixels;
  return mod_image;
}

struct bmp_image* rotate_left(const struct bmp_image* image) {
  if (!image) {
    return NULL;
  }
  unsigned int padding = image->header->height%4;
  size_t pixel_num = image->header->width*image->header->height;
  struct bmp_image* mod_image = calloc(
    sizeof(struct bmp_header)+pixel_num*(sizeof(struct pixel)), 1
  );

  mod_image->header = cpy_header(image->header);
  mod_image->header->width = image->header->height;
  mod_image->header->height = image->header->width;
  mod_image->header->size = (unsigned int)(HEADER_SIZE+pixel_num*3+padding*image->header->width);
  mod_image->header->image_size = (unsigned int)mod_image->header->size-HEADER_SIZE;

  struct pixel* pixels = calloc(pixel_num, sizeof(struct pixel));
  int k = 0;
  for (unsigned int i = 0; i < image->header->width; i++) {
    for (unsigned int j = (unsigned int)pixel_num-image->header->width; j > 0; j-=image->header->width) {
      pixels[k++] = image->data[i+j];
    }
    pixels[k++] = image->data[i];
  }
  mod_image->data = pixels;
  return mod_image;
}

bool check_crop_params(const struct bmp_image* image, const uint32_t start_y, const uint32_t start_x, const uint32_t height, const uint32_t width) {
  if (!image || start_y < 0 || start_x < 0 || height < 1 || width < 1 || start_y+height > image->header->height || start_x+width > image->header->width) {
    return false;
  }
  return true;
}

struct bmp_image* crop(const struct bmp_image* image, const uint32_t start_y, const uint32_t start_x, const uint32_t height, const uint32_t width) {
  if (!check_crop_params(image, start_y, start_x, height, width)) {
    return NULL;
  }
  size_t pixel_num = height*width;
  unsigned int padding = width%4;
  struct bmp_image* mod_image = calloc(
    sizeof(struct bmp_header)+pixel_num*(sizeof(struct pixel)), 1
  );
  mod_image->header = cpy_header(image->header);
  mod_image->header->width = width;
  mod_image->header->height = height;
  mod_image->header->size = (unsigned int)(HEADER_SIZE+pixel_num*3+padding*height);
  mod_image->header->image_size = (unsigned int)mod_image->header->size-HEADER_SIZE;

  struct pixel* pixels = calloc(pixel_num, sizeof(struct pixel));
  int k = 0;
  for (unsigned int y = 0; y < image->header->height; y++) {
    for (unsigned int x = 0; x < image->header->width; x++) {
      if (x >= start_x && x < start_x+width && image->header->height-1-y >= start_y && image->header->height-1-y < start_y+height) {
        pixels[k++] = image->data[y*image->header->width+x];
      }
    }
  }
  mod_image->data = pixels;
  return mod_image;
}

bool check_rgb(const char* str) {
  if (strlen(str) == 0) {
    return false;
  }
  int r_count = 0;
  int g_count = 0;
  int b_count = 0;
  for (int i = 0; i < strlen(str); i++) {
    if (str[i] == 'r') {
      r_count++;
    }
    else if (str[i] == 'g') {
      g_count++;
    }
    else if (str[i] == 'b') {
      b_count++;
    }
    else {
      return false;
    }
  }
  if (r_count > 1 || g_count > 1 || b_count > 1) {
    return false;
  }
  return true;
}

bool char_in_str(char ch, const char* str) {
  for (int i = 0; i < strlen(str); i++) {
    if (ch == str[i]) {
      return true;
    }
  }
  return false;
}

struct bmp_image* extract(const struct bmp_image* image, const char* colors_to_keep) {
  if (!image || !colors_to_keep || !check_rgb(colors_to_keep)) {
    return NULL;
  }
  size_t pixel_num = image->header->width*image->header->height;
  struct bmp_image* mod_image = calloc(
    sizeof(struct bmp_header)+pixel_num*(sizeof(struct pixel)), 1
  );
  mod_image->header = cpy_header(image->header);
  bool check_b = char_in_str('b', colors_to_keep);
  bool check_g = char_in_str('g', colors_to_keep);
  bool check_r = char_in_str('r', colors_to_keep);
  struct pixel* pixels = calloc(pixel_num, sizeof(struct pixel));
  int k = 0;

  for (unsigned int i = 0; i < pixel_num; i++) {
    struct pixel px;
    if (check_b) {
      px.blue = image->data[i].blue;
    }
    else {
      px.blue = 0;
    }
    if (check_g) {
      px.green = image->data[i].green;
    }
    else {
      px.green = 0;
    }
    if (check_r) {
      px.red = image->data[i].red;
    }
    else {
      px.red = 0;
    }
    pixels[k++] = px;
  }
  mod_image->data = pixels;
  return mod_image;
}

struct bmp_image* scale(const struct bmp_image* image, float factor) {
  if (!image || factor <= 0) {
    return NULL;
  }
  unsigned int old_width = image->header->width;
  unsigned int old_height = image->header->height;
  unsigned int new_width = (unsigned int)round((float)image->header->width*factor);
  unsigned int new_height = (unsigned int)round((float)image->header->height*factor);
  unsigned int pixel_num = new_width*new_height;
  unsigned int padding = new_width%4;
  struct bmp_image* mod_image = calloc(
    sizeof(struct bmp_header)+pixel_num*sizeof(struct pixel), 1
  );
  mod_image->header = cpy_header(image->header);
  mod_image->header->width = new_width;
  mod_image->header->height = new_height;
  mod_image->header->size = (unsigned int)(HEADER_SIZE+pixel_num*3+padding*mod_image->header->height);
  mod_image->header->image_size = (unsigned int)mod_image->header->size-HEADER_SIZE;

  struct pixel* pixels = calloc(pixel_num, sizeof(struct pixel));
  
  int k = 0;
  for (unsigned int i = 0; i < new_height; i++) {
    for (unsigned int j = 0; j < new_width; j++) {
      unsigned int x = (unsigned int)(j*old_width/new_width);
      unsigned int y = (unsigned int)(i*old_height/new_height);
      pixels[k++] = image->data[y*old_width+x]; 
    }
  }
  
  mod_image->data = pixels;
  return mod_image;
}

