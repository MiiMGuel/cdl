# CModule
CModule is a single header file library used to load dynamic libraries (further referred as modules) at runtime.

## Include Options
Include options are included with `#define <option>` before including the `cmodule.h` header file.

options:
- `CMODULE_IMPL`                      : include the implementation of `cmodule.h`
- `CMODULE_PATH_SIZE_MAX <value=256>` : sets the size of `struct cmodule.path[value]`

notes:
`CMODULE_IMPL` must be added in atleast one C/C++ source file.
