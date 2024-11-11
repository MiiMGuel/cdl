#ifndef CDL_H
#define CDL_H

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(_WIN32) || defined(_WIN64))
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#       define _UNDEF_LEAN_AND_MEAN
#   endif 
#   ifndef NOMINMAX
#       define NOMINMAX
#       define _UNDEF_NOMINMAX
#   endif 
#   include <windows.h>
#   ifdef _UNDEF_LEAN_AND_MEAN 
#       undef WIN32_LEAN_AND_MEAN
#       undef _UNDEF_LEAN_AND_MEAN 
#   endif 
#   ifdef _UNDEF_NOMINMAX
#       undef NOMINMAX
#       undef _UNDEF_NOMINMAX
#   endif 
#else 
#   include <dlfcn.h>
#endif 

void* cdl_load(const char* filename);
void* cdl_loadws(const char* filename);
void* cdl_gsym(void* mod, const char* symbol);
void cdl_free(void* mod);

#ifdef __cplusplus
}
#endif
#endif // CDL_H

#ifdef CDL_IMPL
#undef CDL_IMPL

#include <stdlib.h>
#include <string.h>

#if (defined(_WIN32) || defined(_WIN64))
    static const char* suffix = ".dll";
#elif defined(__APPLE__)
    static const char* suffix = ".dylib";
#else 
    static const char* suffix = ".so";
#endif

void* cdl_load(const char* filename) {
    void* handle = NULL;
#   if (defined(_WIN32) || defined(_WIN64))
        handle = LoadLibrary(filename);
#   else
        handle = dlopen(filename, RTLD_LAZY);
#   endif 
    return handle;
}

void* cdl_loadws(const char* filename) {
    void* handle         = NULL;
    size_t filename_size = strlen(filename) + 1;
    size_t suffix_size   = strlen(suffix) + 1;
    char* sfilename      = calloc(filename_size + suffix_size, sizeof(char));
    strcpy(sfilename, filename);
    strcat(sfilename, suffix);
#   if (defined(_WIN32) || defined(_WIN64))
        handle = LoadLibrary(sfilename);
#   else
        handle = dlopen(sfilename, RTLD_LAZY);
#   endif
    free(sfilename);
    return handle;
}

void* cdl_gsym(void* mod, const char* symbol) {
    void* ptr = NULL;
#   if (defined(_WIN32) || defined(_WIN64))
        ptr = GetProcAddress(mod, symbol);
#   else
        ptr = dlsym(mod, symbol);
#   endif
    return ptr;
}

void cdl_free(void* mod) {
#   if (defined(_WIN32) || defined(_WIN64))
        FreeLibrary(mod);
#   else
        dlclose(mod);
#   endif 
}
#endif // CDL_IMPL
