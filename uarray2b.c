#include <uarray.h>
#include <uarray2b.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <math.h>

#define T UArray2b_T

struct T{
    UArray_T blockArray;
    int blockSize;
    int height;
    int width;
};

extern T UArray2b_new(int w, int h, int size, int b){
    T uarray2b;
    NEW(uarray2b);
    uarray2b->blockArray = UArray_new((((w - 1)/b + 1) * b) * 
                           (((h - 1)/b + 1) * b), size);
    uarray2b->blockSize = b;
    uarray2b->height = h;
    uarray2b->width = w;
    return uarray2b;
}

extern T UArray2b_new_64K_block(int w, int h, int size){
    int b = sqrt(65536/size);
    T uarray2b;
    NEW(uarray2b);
    uarray2b->blockArray = UArray_new((((w - 1)/b + 1) * b) *
                           (((h - 1)/b + 1) * b), size);
    uarray2b->blockSize = b;
    uarray2b->height = h;
    uarray2b->width = w;
    return uarray2b;
}



extern void UArray2b_free(T* uarray2b){
    assert(uarray2b);
    UArray_free(&((*uarray2b)->blockArray));
    free(*uarray2b);
}


extern int UArray2b_width(T uarray2b){
    assert(uarray2b);
    return uarray2b->width;
}

extern int UArray2b_height(T uarray2b){
    assert(uarray2b);
    return uarray2b->height;
}

extern int UArray2b_size(T uarray2b){
    assert(uarray2b);
    return UArray_size(uarray2b->blockArray);
}

extern int UArray2b_blocksize(T uarray2b){
    assert(uarray2b);
    return uarray2b->blockSize;
}

void *UArray2b_at(T uarray2b, int i, int j){
    assert(uarray2b);
    if(i > uarray2b->width || j > uarray2b->height){
        fprintf(stderr, "call out of bounds");
    }
    int w = uarray2b->width;
    int b = uarray2b->blockSize;
    return UArray_at(uarray2b->blockArray,
                    (i/b)*b*b + (j/b)*b*((b+w-1)/b)*b + b*(j%b) + i%b);
}


void UArray2b_map(T uarray2b, void apply(int i, int j, T uarray2b, void *elem, 
    void *c1), void *c1){
    int i, j, end1, end2, k, l;
    int b = uarray2b->blockSize;
    for(j = 0; j < uarray2b->height; j += b){
        for(i = 0; i < uarray2b->width; i += b){
            end1 = j + b;
            for(k = j; k < end1 && k < uarray2b->height; k++){
                end2 = i + b;
                for(l = i; l < end2 && l < uarray2b->width; l++){
                    assert(UArray2b_at(uarray2b, l, k));
                    apply(l, k, uarray2b, UArray2b_at(uarray2b, l, k), c1);
                }
            }
        }
    }
}
#undef T

