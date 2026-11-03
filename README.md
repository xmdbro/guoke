# guoke

## Requirements

- Windows (x64)
- CMake 3.20 or newer
- A C++20 compiler, such as MinGW-w64 GCC or MSVC with the Windows SDK
- Ninja for the build commands

For MSVC with Ninja, use an x64 Visual Studio developer terminal.

## Build and run

From the repository root in PowerShell:

```powershell
cmake -S . -B build -G Ninja
cmake --build build
.\build\bin\guoke.exe
```

No CMake presets or third-party dependencies are required. The main application is currently a console scaffold.

## Demo application and DLL

The examples are enabled by default and build alongside guoke. To build only the demo targets:

```powershell
cmake --build build --target guoke_demo_app guoke_demo_dll
.\build\bin\guoke_demo_app.exe
```

The three targets have separate roles:

| Target | Role |
| --- | --- |
| `guoke` | The tool that will load the DLL into the selected test process. |
| `guoke_demo_app` | A passive test application. Prints its PID and architecture, then waits for Enter. |
| `guoke_demo_dll` | The DLL that will eventually demonstrate a visible change inside the test application. |

The app and DLL are independent. The app has no DLL loading, unloading, or inspection code and runs without the sample DLL present. Its PID will identify the target process when guoke's loading functionality is implemented.

When the DLL is loaded into a process, it writes `Hello from guoke_demo_dll!` to that process's standard output, if available. For the console demo app, this will appear in its console once guoke can load the DLL. There is no separate export to call, and the demo app still does not load the DLL itself. The greeting uses a small Windows API write in `DllMain`; more complex demonstration behavior and guoke's loading operation remain future work.

To omit the examples from a build:

```powershell
cmake -S . -B build -G Ninja -DGUOKE_BUILD_EXAMPLES=OFF
cmake --build build
```

Reconfigure with `-DGUOKE_BUILD_EXAMPLES=ON` to enable them again. Existing binaries from earlier builds are not removed when the option changes.

## Structure

```text
guoke/
|-- src/
|   `-- main.cpp
|-- examples/
|   |-- CMakeLists.txt
|   |-- demo-app/
|   |   `-- main.cpp
|   `-- demo-dll/
|       `-- dllmain.cpp
|-- CMakeLists.txt
|-- LICENSE
`-- README.md
```

The root CMake project builds all targets; the examples do not need a separate build directory. With Ninja, the EXEs and DLL go to `build/bin/`. Multi-configuration generators add a configuration subdirectory, such as `build/bin/Debug/`.

## Next..

- minimal native Windows interface.
- process selection and DLL compatibility checks.
- one user-mode loading method with clear results.
- a visible proof of concept in the demo application, triggered by the DLL loaded through guoke.
