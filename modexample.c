#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#   define MODEX __declspec(dllexport)
#else
#   define MODEX __attribute__((visibility("default")))
#endif

MODEX int example_var = 1234;

MODEX void example_func(void) {
    printf("Hello, Example!");
}
