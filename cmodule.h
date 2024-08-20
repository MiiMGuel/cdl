#ifndef _CMODULE_
#define _CMODULE_

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(_WIN32) || defined(_WIN64))
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#       define CMODULE_UNDEF_LEAN_AND_MEAN
#   endif 
#   ifndef NOMINMAX
#       define NOMINMAX
#       define CMODULE_UNDEF_NOMINMAX
#   endif 
#   include <windows.h>
#   ifdef CMODULE_UNDEF_LEAN_AND_MEAN 
#       undef WIN32_LEAN_AND_MEAN
#       undef CMODULE_UNDEF_LEAN_AND_MEAN 
#   endif 
#   ifdef CMODULE_UNDEF_NOMINMAX
#       undef NOMINMAX
#       undef CMODULE_UNDEF_NOMINMAX
#   endif 
#else 
#   include <dlfcn.h>
#endif 

#ifndef CMODULE_PATH_SIZE_MAX
#   define CMODULE_PATH_SIZE_MAX 256
#endif 

static_assert(CMODULE_PATH_SIZE_MAX >= 16, "'CMODULE_PATH_SIZE_MAX' should not be lower than 16");

typedef enum cmload_err {
    CMLOAD_ERR_NONE       = 0,
    CMLOAD_ERR_LOAD_FAIL  = 1,
    CMLOAD_ERR_PARAM_NULL = 2,
    CMLOAD_ERR_PATH_MAX   = 3
} cmload_err;

typedef enum cmgsym_err {
    CMGSYM_ERR_NONE       = 0,
    CMGSYM_ERR_GSYM_FAIL  = 1,
    CMGSYM_ERR_CMLOAD_ERR  = 2,
    CMGSYM_ERR_PARAM_NULL = 3
} cmgsym_err;

typedef struct cmodule {
#   if (defined(_WIN32) || defined(_WIN64))
        HMODULE handle;
#   else 
        void*   handle;
#   endif 
    char path[CMODULE_PATH_SIZE_MAX];
} cmodule;

cmload_err   cmodule_load     (cmodule* mod, const char* file);
cmload_err   cmodule_loadp    (cmodule* mod, const char* file);
cmgsym_err   cmodule_gsym     (cmodule* mod, void** ptr, const char* symbol);
void         cmodule_free     (cmodule* mod);
const char*  cmodule_load_err (void);
const char*  cmodule_gsym_err (void);

#ifdef CMODULE_IMPL
#include <stdlib.h>
#include <string.h>

#if (defined(_WIN32) || defined(_WIN64))
    static const char* prefix = ".dll";
#elif defined(__APPLE__)
    static const char* prefix = ".dylib";
#else 
    static const char* prefix = ".so";
#endif

static cmload_err last_mload;
static cmgsym_err last_mgsym;

cmload_err cmodule_load(cmodule* mod, const char* file) {
    if ((mod == NULL) || (file == NULL)) { 
        last_mload = CMLOAD_ERR_PARAM_NULL; 
        return CMLOAD_ERR_PARAM_NULL; 
    } else if (sizeof(file) > CMODULE_PATH_SIZE_MAX) { 
        last_mload = CMLOAD_ERR_PATH_MAX; 
        return CMLOAD_ERR_PATH_MAX; 
    } else {
        strcpy(mod->path, file);
#       if (defined(_WIN32) || defined(_WIN64))
            mod->handle = LoadLibrary(mod->path);
#       else
            mod->handle = dlopen(file, RTLD_LAZY);
#       endif 
        if (!mod->handle) { 
            last_mload = CMLOAD_ERR_LOAD_FAIL; 
            return CMLOAD_ERR_LOAD_FAIL; 
        } else { 
            last_mload = CMLOAD_ERR_NONE; 
            return CMLOAD_ERR_NONE;
        }
    }
}

cmload_err cmodule_loadp(cmodule* mod, const char* file) {
    if ((mod == NULL) || (file == NULL)) { 
        last_mload = CMLOAD_ERR_PARAM_NULL; 
        return CMLOAD_ERR_PARAM_NULL; 
    } else if (sizeof(file) > CMODULE_PATH_SIZE_MAX - 7) { 
        last_mload = CMLOAD_ERR_PATH_MAX; 
        return CMLOAD_ERR_PATH_MAX; 
    } else {
        strcpy(mod->path, file);
        strcat(mod->path, prefix);
#       if (defined(_WIN32) || defined(_WIN64))
            mod->handle = LoadLibrary(mod->path);
#       else
            mod->handle = dlopen(mod->path, RTLD_LAZY);
#       endif
        if (!mod->handle) { 
            last_mload = CMLOAD_ERR_LOAD_FAIL; 
            return CMLOAD_ERR_LOAD_FAIL; 
        } else { 
            last_mload = CMLOAD_ERR_NONE; 
            return CMLOAD_ERR_NONE;
        }
    }
}

cmgsym_err cmodule_gsym(cmodule* mod, void** ptr, const char* symbol) {
    if ((mod == NULL) || (symbol == NULL)) {
        last_mgsym = CMGSYM_ERR_PARAM_NULL;
        return CMGSYM_ERR_PARAM_NULL;
    } else if (mod->handle == NULL) {
        last_mgsym = CMGSYM_ERR_CMLOAD_ERR;
        return CMGSYM_ERR_CMLOAD_ERR;
    } else {
#       if (defined(_WIN32) || defined(_WIN64))
            *ptr = (void*)GetProcAddress((HCMODULE)mod->handle, symbol);
#       else
            *ptr = dlsym(mod->handle, symbol);
#       endif
        if (*ptr == NULL) { 
            last_mgsym = CMGSYM_ERR_GSYM_FAIL;
            return CMGSYM_ERR_GSYM_FAIL;
        } else { 
            last_mgsym = CMGSYM_ERR_NONE;
            return CMGSYM_ERR_NONE;
        }
    }
}

void cmodule_free(cmodule* mod) {
#   if (defined(_WIN32) || defined(_WIN64))
        if (!FreeLibrary((HCMODULE)mod->handle))
            UnmapViewOfFile((HCMODULE)mod->handle); // last resort
#   else
        dlclose(mod->handle);
#   endif 
}

const char* cmodule_load_err(void) {
    switch (last_mload) {
    case CMLOAD_ERR_NONE:
        return "";

    case CMLOAD_ERR_PARAM_NULL:
        return "a parameter was null";  
    
    case CMLOAD_ERR_PATH_MAX:
        return "parameter path exceed max size"; 
    
    case CMLOAD_ERR_LOAD_FAIL:
#       if (defined(_WIN32) || defined(_WIN64))
            return "module not found";
#       else
            return dlerror();
#       endif
    }
}

const char* cmodule_gsym_err(void) {
    switch (last_mgsym) {
    case CMGSYM_ERR_NONE:
        return "";

    case CMGSYM_ERR_PARAM_NULL:
        return "a parameter was null";

    case CMGSYM_ERR_CMLOAD_ERR:
        return "module was not loaded";

    case CMGSYM_ERR_GSYM_FAIL:
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
