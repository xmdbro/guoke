# guoke

## Requirements

- Windows (x64)
- CMake 3.20 or newer
- A C++20 compiler, such as MinGW-w64 GCC or MSVC with the Windows SDK
- Ninja for the build commands

For MSVC with Ninja, use an x64 Visual Studio developer terminal.

## Build and run

```powershell
cmake -S . -B build -G Ninja
cmake --build build
.\build\bin\guoke.exe
```

No CMake presets or third-party dependencies are required.

## Next..

- minimal native Windows interface.
- process selection and DLL compatibility checks.
- demo application and sample DLL for testing.
- one user-mode loading method with clear results.