# C++ Modules application

This project attempts to organize files in such a way that:
- C++20 Modules are used;
- it is possible to add new implementations for a single interface (my use case is platform-dependent versions, but may be any other case really);
- there are no macros to conditionally compile code;
- there are no includes unless it's module implementation;
- build system is the source of truth that selects the right modules to build together.

## Behaviours, different from conventional header/source
- Clang: you can't use header modules at the moment and do e.g. put your macros and `#include <SDL.h>` in a separate header then `import` it (see [the discussion](https://github.com/libsdl-org/SDL/issues/5957#issuecomment-1403577700)). This means you can't use any header third-party libraries. Xmake has fallback but it does not work with the rest of system headers (see [this discussion](https://github.com/xmake-io/xmake/issues/5695#issuecomment-3524142964)).
- As templates are bound to the module they are declared in, you can't provide a specialization for a class in a different module. Workaround is to use `extern "C++"` in declaration of template to put it into a "global" module. (see [this discussion](https://stackoverflow.com/a/79827716/127610))
- There's only one "purview" for a module. You can't split your declarations into multiple files that are imported separately.
- You can't use `extern` to refer to a symbol/function from a different module: symbols are now bound to modules and you can't reference the module from your `extern` declaration elsewhere.
- Once you have interface and (selected conditionally) implementation module, you can't export anything additional from the implementation module. This means you can't expose e.g platform-dependent data and types to be used in other modules - everything has to go through the interface.
- Forward declarations don't work and need `extern "C++"` workaround: if something forward-declared in one module it can't be defined in the other.
