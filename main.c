#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#include "bmp.h"
#include "transformations.h"

#define BUFFER 50

int main(int argc, char* args[]) {

  char* file = calloc(50, sizeof(char));
  if (argc > 2) {
    free(file);
    return -1;
  }
  else if (argc == 1) {
    scanf("%s", file);
  }
  else {
    strcpy(file, args[1]);
  }

  regex_t regex;
  int state = regcomp(&regex, ".*\\.bmp$", 0);
  if (state != 0) {
    printf("ERROR\n");
    free(file);
    return -1;
  }

  state = regexec(&regex, file, 0, NULL, 0);

  if (state == REG_NOMATCH) {
    printf("ERROR\n");
    free(file);
    return -1;
        }
  
  FILE* fb = fopen(file, "rb");
  FILE* stream0 = fopen("write_assets/write0.bmp", "wb");
  struct bmp_image* image = read_bmp(fb);
  write_bmp(stream0, image);
  fclose(fb);
  
  FILE* stream1 = fopen("write_assets/write1.bmp", "wb");
  struct bmp_image* mod_image1 = flip_horizontally(image);
  write_bmp(stream1, mod_image1);
  fclose(stream1);

  FILE* stream2 = fopen("write_assets/write2.bmp", "wb");
  struct bmp_image* mod_image2 = flip_vertically(image);
  write_bmp(stream2, mod_image2);
  fclose(stream2);

  FILE* stream3 = fopen("write_assets/write3.bmp", "wb");
  struct bmp_image* mod_image3 = rotate_right(image);
  write_bmp(stream3, mod_image3);
  fclose(stream3);

  FILE* stream4 = fopen("write_assets/write4.bmp", "wb");
  struct bmp_image* mod_image4 = rotate_left(image);
  write_bmp(stream4, mod_image4);
  fclose(stream4);
  
  FILE* stream5 = fopen("write_assets/write5.bmp", "wb");
  struct bmp_image* mod_image5 = crop(image, 0, 0, 1, 2);
  write_bmp(stream5, mod_image5);
  fclose(stream5);

  FILE* stream6 = fopen("write_assets/write6.bmp", "wb");
  struct bmp_image* mod_image6 = extract(image, "g");
  write_bmp(stream6, mod_image6);
  fclose(stream6);

  FILE* stream7 = fopen("write_assets/write7.bmp", "wb");
  struct bmp_image* mod_image7 = scale(image, 1.529040f);
  write_bmp(stream7, mod_image7);
  fclose(stream7);
  
  free_bmp_image(mod_image1);
  free_bmp_image(mod_image2);
  free_bmp_image(mod_image3);
  free_bmp_image(mod_image4);
  free_bmp_image(mod_image5);
  free_bmp_image(mod_image6);
  free_bmp_image(mod_image7);
  free_bmp_image(image);

  return 0;
}
