#include <pnm.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void compare_images(Pnm_ppm pixmap1, Pnm_ppm pixmap2){
  int h, w;
  float e = 0;
  Pnm_rgb pixel1, pixel2;
  if(pixmap1->height < pixmap2->height)
    h = pixmap1->height;
  else h = pixmap2->height;
  if(pixmap1->width < pixmap2->width)
    w = pixmap1->width;
  else w = pixmap2->width;

  int i, j;
  for(j = 0; j < h; j++){
    for(i = 0; i < w; i++){
      pixel1 = pixmap1->methods->at(pixmap1->pixels, i, j);
      pixel2 = pixmap1->methods->at(pixmap2->pixels, i, j);
      float denominator1 = (float)(pixmap1->denominator);
      float denominator2 = (float)(pixmap2->denominator);
      e += ((pixel1->red/denominator1 - pixel2->red/denominator2) * 
        (pixel1->red/denominator1 - pixel2->red/denominator2));
      e += ((pixel1->green/denominator1 - pixel2->green/denominator2) * 
        (pixel1->green/denominator1 - pixel2->green/denominator2));
      e += ((pixel1->blue/denominator1 - pixel2->blue/denominator2) * 
        (pixel1->blue/denominator1 - pixel2->blue/denominator2));
    }
  }

  e = sqrt(e / (3.0 * (float)w * (float)h));
  printf("%f\n", e); 
}

int main(int argc, char *argv[]){
  FILE* fp1;
  FILE* fp2;
  if(argc == 3){
    if(!strcmp(argv[1],"-"))
      fp1 = stdin;
    else
      fp1 = fopen(argv[1], "rb");
    if(!strcmp(argv[2],"-")){
      if(fp1 == stdin)
        fprintf(stderr, "Only one file pointer can take from stdin.");
      else
        fp2 = stdin;
    }
    else
      fp2 = fopen(argv[2], "rb");
  }
  else
    fprintf(stderr, "Please pass in two arguments to main");

  A2Methods_T methods = uarray2_methods_plain;
  printf("reading in file\n");
  Pnm_ppm pixmap1 = Pnm_ppmread(fp1, methods);
  printf("successfully read file 1\n");
  Pnm_ppm pixmap2 = Pnm_ppmread(fp2, methods);
  printf("successfully read file 2\n");

  if(abs(pixmap1->height - pixmap2->height) > 1 ||
     abs(pixmap1->width - pixmap2->width) > 1)
    fprintf(stderr,"Both width and height of images can differ by at most one");

  compare_images(pixmap1, pixmap2);
  
  Pnm_ppmfree(&pixmap1);
  Pnm_ppmfree(&pixmap2);
  return 0;
}
