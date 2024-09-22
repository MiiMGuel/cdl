#ifndef _CMODULE_
#define _CMODULE_

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

#ifndef CMODULE_PATH_SIZE_MAX
#   define CMODULE_PATH_SIZE_MAX 256
#endif 

typedef enum cmodule_err {
    CMODULE_ERR_LOAD_NONE  = 0,
    CMODULE_ERR_LOAD_FAIL  = 1,
    CMODULE_ERR_LOAD_PNULL = 2,
    CMODULE_ERR_LOAD_PMAX  = 3,
    CMODULE_ERR_GSYM_NONE  = -1,
    CMODULE_ERR_GSYM_FAIL  = 4,
    CMODULE_ERR_GSYM_MNULL = 5,
    CMODULE_ERR_GSYM_PNULL = 6
} cmodule_err;

typedef struct cmodule {
#   if (defined(_WIN32) || defined(_WIN64))
        HMODULE handle;
#   else 
        void*   handle;
#   endif 
    char path[CMODULE_PATH_SIZE_MAX];
} cmodule;

cmodule_err cmodule_load   (cmodule* mod, const char* filename);
cmodule_err cmodule_loadws (cmodule* mod, const char* filename);
cmodule_err cmodule_gsym   (cmodule* mod, void** ptr, const char* symbol);
void        cmodule_free   (cmodule* mod);
const char* cmodule_geterr (void);

#define CMODULE_IMPL
#ifdef CMODULE_IMPL
#include <stdlib.h>
#include <string.h>

#if (defined(_WIN32) || defined(_WIN64))
    static const char* suffix = ".dll";
#elif defined(__APPLE__)
    static const char* suffix = ".dylib";
#else 
    static const char* suffix = ".so";
#endif

static cmodule_err last_err;

cmodule_err cmodule_load(cmodule* mod, const char* filename) {
    if ((mod == NULL) || (filename == NULL)) { 
        last_err = CMODULE_ERR_LOAD_PNULL; 
        return last_err; 
    } else if (sizeof(filename) > CMODULE_PATH_SIZE_MAX) { 
        last_err = CMODULE_ERR_LOAD_PMAX; 
        return last_err; 
    } else {
        strcpy(mod->path, filename);
#       if (defined(_WIN32) || defined(_WIN64))
            mod->handle = LoadLibrary(mod->path);
#       else
            mod->handle = dlopen(filename, RTLD_LAZY);
#       endif 
        if (!mod->handle) { 
            last_err = CMODULE_ERR_LOAD_FAIL; 
            return last_err; 
        } else { 
            last_err = CMODULE_ERR_LOAD_NONE; 
            return last_err;
        }
    }
}

cmodule_err cmodule_loadws(cmodule* mod, const char* filename) {
    if ((mod == NULL) || (filename == NULL)) { 
        last_err = CMODULE_ERR_LOAD_PNULL; 
        return last_err; 
    } else if (sizeof(filename) > CMODULE_PATH_SIZE_MAX - 7) { 
        last_err = CMODULE_ERR_LOAD_PMAX; 
        return last_err;
    } else {
        strcpy(mod->path, filename);
        strcat(mod->path, suffix);
#       if (defined(_WIN32) || defined(_WIN64))
            mod->handle = LoadLibrary(mod->path);
#       else
            mod->handle = dlopen(mod->path, RTLD_LAZY);
#       endif
        if (!mod->handle) { 
            last_err = CMODULE_ERR_LOAD_FAIL; 
            return last_err;
        } else { 
            last_err = CMODULE_ERR_LOAD_NONE; 
            return last_err;
        }
    }
}

cmodule_err cmodule_gsym(cmodule* mod, void** ptr, const char* symbol) {
    if ((mod == NULL) || (symbol == NULL)) {
        last_err = CMODULE_ERR_LOAD_PNULL;
        return last_err;
    } else if (mod->handle == NULL) {
        last_err = CMODULE_ERR_GSYM_MNULL;
        return last_err;
    } else {
#       if (defined(_WIN32) || defined(_WIN64))
            *ptr = (void*)GetProcAddress((HMODULE)mod->handle, symbol);
#       else
            *ptr = dlsym(mod->handle, symbol);
#       endif
        if (*ptr == NULL) { 
            last_err = CMODULE_ERR_GSYM_FAIL;
            return last_err;
        } else { 
            last_err = CMODULE_ERR_GSYM_NONE;
            return last_err;
        }
    }
}

void cmodule_free(cmodule* mod) {
#   if (defined(_WIN32) || defined(_WIN64))
        if (!FreeLibrary((HMODULE)mod->handle))
            UnmapViewOfFilename((HMODULE)mod->handle); // last resort
#   else
        dlclose(mod->handle);
#   endif 
}

const char* cmodule_geterr(void) {
    switch (last_err) {
    case CMODULE_ERR_LOAD_NONE:
        return "";

    case CMODULE_ERR_LOAD_PNULL:
        return "a parameter was null";  
    
    case CMODULE_ERR_LOAD_PMAX:
        return "parameter 'path' exceed max size"; 
    
    case CMODULE_ERR_LOAD_FAIL:
#       if (defined(_WIN32) || defined(_WIN64))
            return "module not found";
#       else
            return dlerror();
#       endif
    
    case CMODULE_ERR_GSYM_NONE:
        return "";

    case CMODULE_ERR_GSYM_PNULL:
        return "a parameter was null";

    case CMODULE_ERR_GSYM_MNULL:
        return "module was not loaded";

    case CMODULE_ERR_GSYM_FAIL:
#       if (defined(_WIN32) || defined(_WIN64))
            return "symbol not found";
#       else
            return dlerror();
#       endif       
    }
}

#endif // CMODULE_IMPL

#ifdef __cplusplus
}
#endif
#endif // _CMODULE_
