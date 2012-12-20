#include "component.h"

extern void component_convertComponent(int i, int j, A2Methods_UArray2 a, 
                                       void* elem, void* cl){
  Pnm_ppm pixMap = cl;
  Pnm_rgb rgb = elem;
  float scaledRed = ((float)rgb->red)/pixMap->denominator;
  float scaledGreen = ((float)rgb->green)/pixMap->denominator;
  float scaledBlue = ((float)rgb->blue)/pixMap->denominator;
  comp contents = malloc(sizeof(struct comp));
  contents->y = 0.299 * scaledRed + 0.587 * scaledGreen + 0.114 * scaledBlue;
  contents->pb = -0.168736 * scaledRed - 0.331264 * scaledGreen + 
    0.5 * scaledBlue;
  contents->pr = 0.5 * scaledRed - 0.418688 * scaledGreen - 
    0.081312 * scaledBlue;
  comp targetLoc = pixMap->methods->at(a, i, j);
  *targetLoc = *contents;
  free(contents);
}

// Stores a componenet struct in given array pixels
extern void component_storeComponent(A2Methods_UArray2 pixels, int i, int j, 
                                     float avgPB, float avgPR, float y){
  A2Methods_T methods = uarray2_methods_blocked;
  comp target = methods->at(pixels, i, j);
  comp contents = malloc(sizeof(struct comp));
  contents->y = y;
  contents->pb = avgPB;
  contents->pr = avgPR;
  *target = *contents;
  free(contents);
}

// creates a scaled value to an rgb value.
unsigned component_createRGB(float scaledValue, float denominator){
  float rgbvalue = scaledValue * denominator;
  if(rgbvalue > denominator)
    return (unsigned)denominator;
  else if(rgbvalue < 0)
    return 0;
  else
    return roundf(rgbvalue);
}

extern void component_convertRGB(A2Methods_UArray2 componentImage, 
                                 A2Methods_UArray2 pixels, 
                                 unsigned denominator){
  A2Methods_T methods = uarray2_methods_blocked;
  int i, j;
  comp component;
  Pnm_rgb loc;
  float scaledRed, scaledGreen, scaledBlue;
  for(j = 0; j < methods->height(pixels); j++){
    for(i = 0; i < methods->width(pixels); i++){
      component = methods->at(componentImage, i, j);
      Pnm_rgb temp = malloc(sizeof(struct Pnm_rgb));
      loc = methods->at(pixels, i, j);
      scaledRed = 1.0 * component->y + 1.402 * component->pr;
      scaledGreen = 1.0 * component->y - 0.344136 * component->pb - 
        0.714136 * component->pr;
      scaledBlue = 1.0 * component->y + 1.722 * component->pb;
      temp->red = component_createRGB(scaledRed, denominator);
      temp->green = component_createRGB(scaledGreen, denominator);
      temp->blue = component_createRGB(scaledBlue, denominator);
      *loc = *temp;
      free(temp);
    }
  }
}

