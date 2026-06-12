# RPGengine

## CMake Options

`BUILD_EXAMPLES` - builds examples - `OFF` by default.

## Troubleshooting

```
.../RPGengine/third_party/dawn/src/dawn/native/d3d/d3d_platform.h:44:10: fatal error: DXProgrammableCapture.h: No such file or directory
   44 | #include <DXProgrammableCapture.h>
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
```

On Windows compile with MSVC or install windows build tools for your compiler.

---

```
Error: DynamicLib.Open: d3dcompiler_47.dll Windows Error: 87
    at EnsureFXC (...\RPGengine\third_party\dawn\src\dawn\native\d3d\PlatformFunctions.cpp:118)

Warning: Couldn't load Vulkan: DynamicLib.Open: vulkan-1.dll Windows Error: 87
    at operator () (...\RPGengine\third_party\dawn\src\dawn\native\vulkan\BackendVk.cpp:364)

RequestDeviceError: DynamicLib.Open: d3dcompiler_47.dll Windows Error: 87
    at EnsureFXC (...\RPGengine\third_party\dawn\src\dawn\native\d3d\PlatformFunctions.cpp:118)
```

Copy `3d3compiler_47.dll` and `vulkan-1.dll` from `C:/Windows/System32`, to where the executable is.

---

```
C:\Projects\Politechnika\RPG\RPGengine\RPGengine\third_party\dawn\src\dawn\glfw\utils_emscripten.cpp:29:2: error: 
      "utils_emscripten.cpp: This file requires EMSCRIPTEN to be defined."
   29 | #error "utils_emscripten.cpp: This file requires EMSCRIPTEN to be defined."
      |  ^
```

Replace `#if !defined(EMSCRIPTEN)` with `#if !defined(__EMSCRIPTEN__)`.