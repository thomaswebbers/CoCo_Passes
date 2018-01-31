#include <stdio.h>

void __coco_dummy_print_allocation(int elems) {
    printf("Allocating %d elements on stack\n", elems);
}

void __coco_check_bounds(long offset, long array_size){
    if(offset < 0 || offset > array_size){
        printf("array out_of_bounds read/write. Offset: %d, Array size: %d", offset, array_size);
        exit();
    }
}
