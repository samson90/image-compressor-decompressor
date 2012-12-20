#include "uarray2.h"
#include <uarray.h>
#include <mem.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define T UArray2_T

struct T {
    UArray_T externArray;
};

extern T UArray2_new(int w, int h, int size){
    // Allocates memory for external array, elements are sized large enough to
    // to fit internal arrays. 
    //optimize size;
    T uarray2;
    NEW(uarray2);
    uarray2->externArray = UArray_new(w, h * 8);
    int i;
    for(i = 0; i < w; i++){
        UArray_T *a = UArray_at(uarray2->externArray, i);
        *a = UArray_new(h, size); 
    }
    return uarray2;
}

extern void UArray2_free(UArray2_T* uarray2){
    assert(uarray2);
    int i;
    int width = UArray2_width(*uarray2);
    for (i = 0; i < width; i++)
        UArray_free(UArray_at((*uarray2)->externArray, i));
    UArray_free(&((*uarray2)->externArray));
    free(*uarray2);    
}

extern int UArray2_width(UArray2_T uarray2){
    assert(uarray2);
    return UArray_length(uarray2->externArray);
}

extern int UArray2_height(UArray2_T uarray2){
    assert(uarray2);
    assert(UArray_at(uarray2->externArray, 0));
    UArray_T *a = UArray_at(uarray2->externArray, 0);
    return UArray_length(*a);
}

extern int UArray2_size(UArray2_T uarray2){
    assert(uarray2);
    return UArray_size(UArray_at(uarray2->externArray,0));
}

void *UArray2_at(T uarray2, int i, int j){
    assert(uarray2);
    UArray_T * internal = UArray_at(uarray2->externArray, i);
    return UArray_at(*internal, j);
}

void UArray2_map_row_major(UArray2_T uarray2, void apply(int i, int j, UArray2_T
uarray2, void *elem, void *cl), void *cl){
    assert(uarray2);
    int j;
    int height = UArray2_height(uarray2);
    int width = UArray2_width(uarray2);
    for(j = 0; j < height; j++){
        int i;
        for(i = 0; i < width; i++){
            assert(UArray2_at(uarray2, i, j));
            void *elem = UArray2_at(uarray2, i, j);
            apply(i, j, uarray2, elem, cl);
        }
    }
}

void UArray2_map_col_major(UArray2_T uarray2, void apply(int i, int j, 
UArray2_T uarray2, void *elem, void *cl), void *cl){
    assert(uarray2);
    int i;
    int height = UArray2_height(uarray2);
    int width = UArray2_width(uarray2);
    for(i = 0; i < width; i++){
        int j;
        for(j = 0; j < height; j++){
            assert(UArray2_at(uarray2, i, j));
            void *elem = UArray2_at(uarray2, i, j);
            apply(i, j, uarray2, elem, cl);
        }
    }
}
#undef T
