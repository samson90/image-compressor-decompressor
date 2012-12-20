#ifndef COMPONENT_INCLUDED
#define COMPONENT_INCLUDED
#include <pnm.h>
#include <mem.h>
#include <assert.h>
#include <stdlib.h>
#include "a2plain.h"
#include "a2blocked.h"
#include <math.h>

typedef struct comp{
  float y, pb, pr;
} *comp;

// Apply functions that converts rgb value to component.
extern void component_convertComponent(int i, int j, A2Methods_UArray2 a, 
                                       void* elem, void* cl);

// Stores a component struct in given array pixels.
extern void component_storeComponent(A2Methods_UArray2 pixels, int i, int j, 
                                     float avgPB, float avgPR, float y);

// converts a component struct to rgb values.
extern void component_convertRGB(A2Methods_UArray2 componenetImage, 
                                  A2Methods_UArray2 pixels, 
                                  unsigned denominator);
#endif
