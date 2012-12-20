#include <compress40.h>
#include <pnm.h>
#include <a2methods.h>
#include "a2plain.h"
#include "a2blocked.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <assert.h>
#include <mem.h>
#include "component.h"
#include <arith40.h>
#include <bitpack.h>

typedef A2Methods_UArray2 A2;

A2 trimImage(Pnm_ppm image){
  assert(image);
  int h = image->height;
  int w = image->width;
  A2 newPixels;
  // creates the new 2d pixel array.
  if(h% 2 != 0 && w % 2 != 0){
    newPixels = image->methods->new_with_blocksize(w - 1, h - 1, 
      sizeof(struct Pnm_rgb), 1);
    h = h - 1;
    w = w - 1;
  }
  else if(h%  2 != 0){
    newPixels = image->methods->new_with_blocksize(w, h- 1, 
      sizeof(struct Pnm_rgb), 1);
    h = h- 1;
  }
  else if(w % 2 != 0){
    newPixels = image->methods->new_with_blocksize(w - 1, h, 
      sizeof(struct Pnm_rgb), 1);
    w = w - 1;
  } 
  else newPixels = image->methods->new_with_blocksize(w, h, 
    sizeof(struct Pnm_rgb), 1);
  // transfer date from newPixels of old pixels.
  int i, j;
  for(j = 0; j < h; j++){
    for(i = 0; i < w; i++){
      assert(newPixels);
      Pnm_rgb targetLoc = image->methods->at(newPixels, i, j);
      Pnm_rgb contents = image->methods->at(image->pixels, i, j);
      *targetLoc = *contents;
    }
  }
  return newPixels;  
}

signed quantize(float x){
  if (x > .3)
    return 30;
  else if (x < -.3)
    return -30;
  else
    return roundf(100 * x);
}

void pack(int i, int j, A2 array, void* elem, void* cl){
  (void)elem;
  (void)array;
  A2 pixels = cl;
  A2Methods_T methods = uarray2_methods_blocked;
  assert(methods);
  assert(pixels);
  // gets block of 2x2 pixels
  comp elem1 = methods->at(pixels, 2*i, 2*j);
  comp elem2 = methods->at(pixels, 2*i + 1, 2*j);
  comp elem3 = methods->at(pixels, 2*i, 2*j + 1);
  comp elem4 = methods->at(pixels, 2*i + 1, 2*j + 1);
  // gets y, PR, and PB values.
  float avgPB = (elem1->pb + elem2->pb + elem3->pb + elem4->pb) / 4.0;
  float avgPR = (elem1->pr + elem2->pr + elem3->pr + elem4->pr) / 4.0;
  unsigned indexPB = Arith40_index_of_chroma(avgPB);
  unsigned indexPR = Arith40_index_of_chroma(avgPR);
  float y1 = elem1->y;
  float y2 = elem2->y;
  float y3 = elem3->y;
  float y4 = elem4->y;
  // calculates the a, b, c, d coefficent
  unsigned a = roundf(63.0 * (y4 + y3 + y2 + y1)/4.0);
  signed b = quantize((y4 + y3 - y2 - y1)/4.0);
  signed c = quantize((y4 - y3 + y2 - y1)/4.0);
  signed d = quantize((y4 - y3 - y2 + y1)/4.0);
  uint64_t word = 0;
  //packs values into word.
  word = Bitpack_newu(word, 6, 26, a);
  word = Bitpack_news(word, 6, 20, b);
  word = Bitpack_news(word, 6, 14, c);
  word = Bitpack_news(word, 6, 8, d);
  word = Bitpack_newu(word, 4, 4, indexPB);
  word = Bitpack_newu(word, 4, 0, indexPR);
  uint64_t *targetLoc = methods->at(array, i, j);
  *targetLoc = word;
}

extern void writeImage(A2 image, A2Methods_T methods){
  int i, j;
  assert(methods);
  assert(image);
  int h = methods->height(image) * 2;
  int w = methods->width(image) * 2;
  printf("COMP40 Compressed image format 2\n%u %u\n", w, h);
  for(j = 0; j < methods->height(image); j++){
    for(i = 0; i < methods->width(image); i++){
      uint64_t *elem = methods->at(image, i, j);
      // writes the info as 8-bit bytes.
      assert(elem);
      putchar(Bitpack_getu(*elem, 8, 24));
      putchar(Bitpack_getu(*elem, 8, 16));
      putchar(Bitpack_getu(*elem, 8, 8));
      putchar(Bitpack_getu(*elem, 8, 0));
    }
  }
}

extern void compress40(FILE *input){
  A2Methods_T methods = uarray2_methods_blocked;
  Pnm_ppm pixmap;
  TRY
    pixmap = Pnm_ppmread(input, methods);
  ELSE
    fprintf(stderr, "File is not of type ppm.\n");
    exit(1);
  END_TRY;
  if(pixmap->height < 2 || pixmap->width < 2){
    fprintf(stderr, "Height and width of image must be at least 2.\n");
    exit(1);
  }
  //trims the height and width of image to even number.
  assert(pixmap);
  A2 pixels = trimImage(pixmap);
  //converts rgb values to component
  assert(pixels);
  methods->map_default(pixels, component_convertComponent, pixmap);
  int h = methods->height(pixels);
  int w = methods->width(pixels);
  A2 compressedImage = methods->new_with_blocksize(w/2, h/2, 
    sizeof(uint64_t), 1);
  //packs pixel into compressed image format.
  assert(compressedImage);
  assert(methods);
  methods->map_default(compressedImage, pack, pixels);
  //writes image to stdout.
  writeImage(compressedImage, methods);
  Pnm_ppmfree(&pixmap);
  methods->free(&pixels);
  methods->free(&compressedImage);
}

void readImage(int i, int j, A2 array, void *elem, void *cl){
  (void)elem;
  uint64_t value = 0;
  FILE* input = cl;
  A2Methods_T methods = uarray2_methods_blocked;
  assert(input);
  assert(methods);
  //merges the 8-bit bytes into 32-bit word.
  value = Bitpack_newu(value, 8, 24, getc(input));
  value = Bitpack_newu(value, 8, 16, getc(input));
  value = Bitpack_newu(value, 8, 8, getc(input));
  value = Bitpack_newu(value, 8, 0, getc(input));
  uint64_t *targetLoc = methods->at(array, i, j);
  *targetLoc = value;
}

float unquantize(signed x){
  return (float)x / 100.0;
}

void unpack(int i, int j, A2 array, void *elem, void* cl){
  A2Methods_T methods = uarray2_methods_blocked;
  A2 pixels = cl;
  assert(methods);
  uint64_t *target = methods->at(array, i, j);
  assert(target);
  // gets the component coefficients and the PR and PB values.
  unsigned a = Bitpack_getu(*target, 6, 26);
  signed b = Bitpack_gets(*target, 6, 20);
  signed c = Bitpack_gets(*target, 6, 14);
  signed d = Bitpack_gets(*target, 6, 8);
  unsigned indexPB = Bitpack_getu(*target, 4, 4);
  unsigned indexPR = Bitpack_getu(*target, 4, 0);
  float avgPB = Arith40_chroma_of_index(indexPB);
  float avgPR = Arith40_chroma_of_index(indexPR);
  float a2 = (float)a / 63.0;
  float b2 = unquantize(b);
  float c2 = unquantize(c);
  float d2 = unquantize(d);
  //calculates the y values.
  float y1 = a2 - b2 - c2 + d2;
  float y2 = a2 - b2 + c2 - d2;
  float y3 = a2 + b2 - c2 - d2;
  float y4 = a2 + b2 + c2 + d2;
  component_storeComponent(pixels, 2*i, 2*j, avgPB, avgPR, y1); 
  component_storeComponent(pixels, 2*i + 1, 2*j, avgPB, avgPR, y2); 
  component_storeComponent(pixels, 2*i, 2*j + 1, avgPB, avgPR, y3); 
  component_storeComponent(pixels, 2*i + 1, 2*j + 1, avgPB, avgPR, y4);
  (void)elem;
}

extern void decompress40(FILE *input){
  A2Methods_T methods = uarray2_methods_blocked;
  unsigned height, width;
  //reads the compressed image header.
  int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", &width, 
    &height);
  assert(read == 2);
  int c = getc(input);
  assert(c == '\n');
  A2 array = methods->new_with_blocksize(width, height, sizeof(struct Pnm_rgb), 
    1);
  assert(array);
  // creates a pixmap with the array as the pixels.
  struct Pnm_ppm pixmap = { .width = width, .height = height
                          , .denominator = 255, .pixels = array,
                          .methods = methods 
                          };
  //compressedImage will read in the values of the compressed file
  //componentImage will store the component values of the image allowing the
  // pixmap to convert the values to rgb.
  A2 compressedImage = methods->new_with_blocksize(width/2, height/2, 
    sizeof(uint64_t), 1); 
  A2 componentImage = methods->new_with_blocksize(width, height, 
    sizeof(struct comp), 1);
  assert(compressedImage);
  assert(componentImage);
  methods->map_default(compressedImage, readImage, input);
  methods->map_default(compressedImage, unpack, componentImage);
  component_convertRGB(componentImage, pixmap.pixels, pixmap.denominator);
  assert(pixmap.pixels);
  Pnm_ppmwrite(stdout, &pixmap);
  methods->free(&array);
  methods->free(&compressedImage);
  methods->free(&componentImage);
}
