#include <stdio.h>
#include <stdbool.h>

#define CMODULE_IMPL
#define CMODULE_PATH_SIZE_MAX 16
#include "cmodule.h"

bool loadpnlog(cmodule* mod, const char* path) {
    if (cmodule_loadwp(mod, path) > 0) {
        printf("failed loading module \"%s\", %s\n", mod->path, cmodule_geterr());
        return false;
    } else { 
        printf("succes loading module \"%s\"\n", mod->path); 
        return true;
    }
}

bool gsymnlog(cmodule* mod, void** ptr, const char* symbol) {
    if (cmodule_gsym(mod, ptr, symbol) > 0) {
        printf("failed getting symbol \"%s\" from \"%s\", %s\n", symbol, mod->path, cmodule_geterr());
        return false;
    } else { 
        printf("succes getting symbol \"%s\" from \"%s\"\n", symbol, mod->path);
        return true;
    }
}

int main(void) {
    cmodule example            = {0};
    void (*example_func)(void) = NULL;
    int* example_var           = NULL;

    if (!loadpnlog(&example, "modexample")) return -1;
    if (!gsymnlog(&example, (void**)&example_func, "sum")) return -1;
    if (!gsymnlog(&example, (void**)&example_var, "example_var")) return -1;

    example_func();
    printf("example_var = %d\n", *example_var);

    return 0;
}
