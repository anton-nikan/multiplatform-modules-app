# C++ Modules application experiment

This project attempts to organize files in such a way that:
- C++20 Modules are used;
- it is possible to add new implementations for a single interface (my use case is platform-dependent versions, but may be any other case really);
- there are no macros to conditionally compile code;
- there are no includes unless it's module implementation (hiding implementation details);
- build system is the source of truth that selects the right modules to build together.

## Behaviours, different from conventional header/source
Status of each original issue: **handled** (solved by the layout below), **open** (still applies).

- **Open.** Clang: you can't use header modules at the moment and do e.g. put your macros and `#include <SDL.h>` in a separate header then `import` it (see [the discussion](https://github.com/libsdl-org/SDL/issues/5957#issuecomment-1403577700)). This means you can't use any header third-party libraries. Xmake has fallback but it does not work with the rest of system headers (see [this discussion](https://github.com/xmake-io/xmake/issues/5695#issuecomment-3524142964)).
- **Handled.** As templates are bound to the module they are declared in, you can't provide a specialization for a class in a different module. The `extern "C++"` workaround (see [this discussion](https://stackoverflow.com/a/79827716/127610)) is no longer needed: there are no cross-module specializations, every type has exactly one owning module.
- **Open.** There's only one "purview" for a module. You can't split your declarations into multiple files that are imported separately.
- **Handled.** You can't use `extern` to refer to a symbol/function from a different module: symbols are bound to modules. Implementations are `module application;` units of the interface's own module, so they define the interface's declarations directly.
- **Handled.** Once you have interface and (selected conditionally) implementation module, you can't export anything additional from the implementation module, so platform-dependent data and types could not be shared. Platform data that another module needs lives in a separate, build-selected `*.native` module (see below).
- **Handled.** Forward declarations don't work across modules and needed `extern "C++"`. Contexts are now complete types in the interface, so nothing is forward-declared across modules.

## How it works now
- Generic modules (`src/`: `platform`, `application`, `render`, `resources`) contain only platform-independent code and import nothing platform-specific. Each exposes a `context` struct: a public part (plain members everyone may use) and `native_storage<N> native;`.
- `native_storage<N>` (`src/native_storage.cpp`) is `N` bytes of inline, aligned storage. The interface only reserves space; the concrete native type is created (`native.emplace<T>()`) and read (`native.as<T>()`) by the implementation, which is the only code able to name `T`. No heap, no RTTI: the type is checked with a per-type tag address, and a mismatch or missing native part throws. `has_value()` / `reset()` allow safe shutdown. If a native struct outgrows the reserved size, a `static_assert` says to raise the capacity in the interface.
- Platform code lives in one directory per backend, `src/macos/` and `src/sdl/`; the build decides which is compiled (see Backends). Files there are `module <name>;` implementation units of the generic modules and define the interface functions. A native type used by only one implementation is defined in that unit (`native_data`).
- A native type shared by several modules is a `*.native` module (`application.native`), imported only by implementations. Generic code and `main` never import it, so `main` cannot name or touch native members (it can't even name the module's types). Another platform provides its own `application.native` with the same module name.
- Building (macOS needs Clang for Blocks in metal-cpp, GCC can't build it; Apple's Clang lacks `import std`): `xmake f -c --toolchain=llvm --sdk=/opt/homebrew/opt/llvm && xmake`. `-fno-rtti` is set for the target.

## Backends
The `backend` option in `xmake.lua` selects the implementation units for `platform`, `application` and `render`:
- `macos` (default on macOS): `src/macos`, Cocoa and Metal through metal-cpp.
- `sdl`: `src/sdl`, SDL3 (`libsdl3` package, works on any platform). Switch with `xmake f -c --backend=sdl`.

Choosing `macos` on another platform, or an unknown value, stops with an error at build time. Both backends provide `application.native` (the SDL one holds the `SDL_Window*`). SDL has no view delegate that draws on its own, so `application::context` has a public `on_frame` hook: `render-sdl` sets it and the SDL loop in `application-sdl` calls it; the macOS backend ignores it.

## Models
- `model` (`src/model.cpp`) is the generic interface: `model_t` hides the library type behind a `unique_ptr` to an incomplete `native_model_t`, and the loaders return `std::expected<model_t, std::string>`. `resources::load` picks a loader by lower-cased extension (`.glb`, `.gltf`) and returns `std::expected` as well.
- The glTF implementation is selected in `xmake.lua` (tinygltf 2.9.x, pinned because 3.x has a different API): `src/impl/model-gltf.cpp` is the `module model;` unit and includes only declarations.
- `src/impl/tinygltf.cpp` is deliberately a plain (non-module) translation unit holding the `*_IMPLEMENTATION` macros: compiling tinygltf's implementation inside a module unit fails with clang. `src/impl/gltf.h` holds the shared library configuration macros so both files agree on them. Pattern for macro-driven single-header libraries: implementation in a plain unit, module units include declarations only.
