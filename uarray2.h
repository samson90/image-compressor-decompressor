//Interface for UArray2

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;

// Definition: Allocates, initializes and returns a new 2d array of width w 
// and height h. Each element in UArray2 occupies size bytes.
// Precondition: UArray2 is not allocated in memory.
// Postcondition: UArray2 is allocated in memory. 
extern T UArray2_new(int w, int h, int size);

// Definition: Deallocates and clears *uarray2.
// Precondition: *uarray2 and uarray2 are not NULL.
// Postcondition: uarray2 is deallocated from memory.
extern void UArray2_free(T* uarray2);

// Definition: Returns the width of uarray2.
// Precondition: uarray2 exists in memory.
// Postcondition: uarray2 exists in memory.
extern int UArray2_width(T uarray2);

// Definition: Returns the width of uarray2.
// Precondition: uarray2 exists in memory.;
// Postcondition: uarray2 exists in memory.
extern int UArray2_height(T uarray2);

// Definition: Returns the size in bytes of all the elements in uarray2.
// Precondition: uarray2 exists in memory.
// Postcondition: uarray2 exists in memory.
extern int UArray2_size(T uarray2);

// Definition: Returns the element at location (i, j) in uarray2.
// Precondition: uarray2 exists in memory.
// Postcondition: uarray2 exists in memory.
void *UArray2_at(T uarray2, int i, int j);

// Definition: Calls apply(int w, int h, void *cl) for each element in uarray2
// cycling through rows first
// Precondition: uarray2 exists in memory
// Postcondition: uarray2 exists in memory
void UArray2_map_row_major(T uarray2, void apply(int i, int j, 
     UArray2_T uarray2, void *elem, void *cl), void *cl);

// Definition: Calls apply(int w, int h, void *cl) for each element in uarray2
// cycling through columns first
// Precondition: uarray2 exists in memory
// Postcondition: uarray2 exists in memory
void UArray2_map_col_major(T uarray2, void apply(int i, int j, 
     UArray2_T uarray2, void *elem, void *cl), void *cl);

#undef T
#endif

