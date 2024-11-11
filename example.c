#include <stdio.h>
#include <stdbool.h>

#include "cdl.h"

int main(void) {
    void* example               = NULL;
    void  (*example_func)(void) = NULL;
    int*  example_var           = NULL;

    example = cdl_loadws("modexample");
    if (example == NULL) {
        printf("failed loading module!\n");
        return -1;
    }

    example_func = cdl_gsym(example, "example_func");
    if (example_func == NULL) {
        printf("failed getting symbol \"sum\"!\n");
        return -1;
    }

    example_var = cdl_gsym(example, "example_var");
    if (example_var == NULL) {
        printf("failed getting symbol \"example_var\"!\n");
        return -1;
    }

    example_func();
    printf("example_var = %d\n", *example_var);

    return 0;
}
