#include <stdlib.h>

#include "bmp.h"

#define HEADER_SIZE 54
#define BMP_CODE 0x4D42 
/*
void print_header(struct bmp_header* header) {
  printf("type: %d\n", header->type);
  printf("size: %d\n", header->size);
  printf("reserved1: %d\n", header->reserved1);
  printf("reserved2: %d\n", header->reserved2);
  printf("offset: %d\n", header->offset);
  printf("dib_size: %d\n", header->dib_size);
  printf("width: %d\n", header->width);
  printf("height: %d\n", header->height);
  printf("planes: %d\n", header->planes);
  printf("bpp: %d\n", header->bpp);
  printf("compression: %d\n", header->compression);
  printf("image_size: %d\n", header->image_size);
  printf("x_ppm: %d\n", header->x_ppm);
  printf("y_ppm: %d\n", header->y_ppm);
  printf("num_colors: %d\n", header->num_colors);
  printf("important_colors: %d\n", header->important_colors);
}


void print_data(size_t pixel_num, struct pixel* pixels) {
  printf("pixel_num: %ld\n", pixel_num);
  printf("B\tG\tR\n");
  for (int i = 0; i < pixel_num; i++){
    printf("%d\t", pixels[i].blue);
    printf("%d\t", pixels[i].green);
    printf("%d\n", pixels[i].red);
  }
}
*/
struct bmp_header* read_bmp_header(FILE* stream) {
  if (!stream) {
    return NULL;
  }
  rewind(stream);
  struct bmp_header* header = calloc(HEADER_SIZE, 1);
  fread(header, HEADER_SIZE, 1, stream);
  if (header->type != BMP_CODE) {
    free(header);
    return NULL;
  }
  // print_header(header);
  return header;
}

struct pixel* read_data(FILE* stream, const struct bmp_header* header) {
  if (!stream || !header || header->type != BMP_CODE) {
    return NULL;
  }
  fseek(stream, HEADER_SIZE, SEEK_SET);
  size_t pixel_num = header->width*header->height;
  long int padding = header->width%4;
  struct pixel* pixels = calloc(sizeof(struct pixel), pixel_num);
  for (unsigned int i = 0; i < pixel_num; i+=header->width) {
    for (unsigned int j = 0; j < header->width; j++) {
      struct pixel pixel;
      fread(&pixel, 3, 1, stream);
      pixels[i+j] = pixel;
    }
    
    fseek(stream, padding, SEEK_CUR);
  }
  //print_data(pixel_num, pixels);
  
  return pixels;
}

struct bmp_image* read_bmp(FILE* stream) {
  if (!stream) {
    return NULL;
  }
  rewind(stream);
  struct bmp_image* image = calloc(sizeof(struct bmp_image), 1);
  struct bmp_header* header = read_bmp_header(stream);
  if (!header) {
    fprintf(stderr, "Error: This is not a BMP file.\n");
    free(image);
    free(header);
    return NULL;
  }
  if (header->size <= HEADER_SIZE || header->image_size < 0 || header->width < 0 || header->height < 0 || header->offset != HEADER_SIZE) {
    fprintf(stderr, "Error: Corrupted BMP file.\n");
    free(image);
    free(header);
    return NULL;
  }
  
  image->header = header;
  image->data = read_data(stream, image->header);
  return image;
}

void free_bmp_image(struct bmp_image* image) {
  if (!image) {
    return;
  }
  free(image->header);
  free(image->data);
  free(image);
}

bool write_bmp(FILE* stream, const struct bmp_image* image) {
  if (!stream || !image) {
    return false;
  }
  rewind(stream);
  fwrite(image->header, HEADER_SIZE, 1, stream);
  size_t pixel_num = image->header->width*image->header->height;
  long int padding = image->header->width%4;

  for (unsigned int i = 0; i < pixel_num; i+=image->header->width) {
    for (unsigned int j = 0; j < image->header->width; j++) {
      fwrite(&(image->data[i+j]), 3, 1, stream);
    }
    for (unsigned int k = 0; k < padding; k++) {
      fwrite("\x00", 1, 1, stream);
    }
  }

  return true;
}



