# Open Visual Programmer

## Overview

`open_vp` is a node‑based visual programming environment built with Qt 6. It combines a block‑diagram editor backed by the `QtNodes` library and a  front panel for interactive control, display widgets, and data visualization.

Core capabilities:
- Visual graph editing using `QtNodes` (tag `3.0.12`) as a submodule.
- Front panel UI with controls like gauges, switches, LEDs, and waveform graphs.
- Dataflow execution engine with type propagation and node registry.
- Optional hardware integration via `QtSerialPort` for DAQ/serial devices.
- Charts and OpenGL widgets for rich visualization.

Key modules:
- UI: `src/ui` and `include/ui` (e.g., `MainWindow`, `FrontPanel`, `GraphLayout`).
- Core: `src/core` and `include/core` (e.g., `ExecutionEngine`, `DataStore`, `TypePropagator`).
- Nodes: `src/nodes` and `include/nodes` (math, display, image, hardware, Python script, etc.).
- External: `external/nodeeditor` (QtNodes library at tag `3.0.12`).


## Architecture Snapshot

- Application target name is `open_vp` (see `c:\Users\hmgics\projects\open_vp\CMakeLists.txt:52`).
- The project consumes Qt components declared in `c:\Users\hmgics\projects\open_vp\CMakeLists.txt:14`:
  - `Core`, `Gui`, `Widgets`, `OpenGLWidgets`, `Charts`, `OpenGL`, `PrintSupport`, `Network`
  - Optional: `SerialPort` (`c:\Users\hmgics\projects\open_vp\CMakeLists.txt:18-21`)
- The `QtNodes` submodule is added via `add_subdirectory(external/nodeeditor)` (`c:\Users\hmgics\projects\open_vp\CMakeLists.txt:36`) and targets link against `QtNodes` (e.g., `OpenFlowControls` at `c:\Users\hmgics\projects\open_vp\CMakeLists.txt:49`, `open_vp` at `c:\Users\hmgics\projects\open_vp\CMakeLists.txt:304`).


## Prerequisites

- Windows 10/11 with Visual Studio 2019 (v142 toolset).
- Qt 6.7.x MSVC build (tested with `6.7.3`).
- CMake ≥ 3.16.
- Optional: vcpkg for dependency management.

Ensure Qt is discoverable by CMake via `CMAKE_PREFIX_PATH` (see `c:\Users\hmgics\projects\open_vp\CMakeLists.txt:12`).


## Getting Started

Clone and initialize submodules:

```
git clone <your-repo-url> open_vp
cd open_vp
git submodule update --init --recursive
```

Configure (Visual Studio 2019 + vcpkg toolchain):

```
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 \
  -DCMAKE_TOOLCHAIN_FILE="C:\\Users\\<you>\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake"
```

Build Release:

```
cmake --build build --config Release -- /m
```

Run tests (set PATH for Qt and `QtNodes.dll`):

```
$env:PATH = "C:\\Users\\<you>\\projects\\open_vp\\build\\bin\\Release;C:\\Qt\\6.7.3\\msvc2019_64\\bin;" + $env:PATH
ctest --test-dir build -C Release --output-on-failure
```

Launch the app:

```
$env:PATH = "C:\\Users\\<you>\\projects\\open_vp\\build\\bin\\Release;C:\\Qt\\6.7.3\\msvc2019_64\\bin;" + $env:PATH
& .\build\Release\open_vp.exe
```

Packaging on Windows (optional):

```
"C:\\Qt\\6.7.3\\msvc2019_64\\bin\\windeployqt.exe" .\build\Release\open_vp.exe
```


## Dependencies

- Qt 6 components: `Core`, `Gui`, `Widgets`, `OpenGLWidgets`, `Charts`, `OpenGL`, `PrintSupport`, `Network`
- Optional: `SerialPort` (enable if installed)
- `QtNodes` submodule at `external/nodeeditor` tag `3.0.12`

The submodule builds its own `QtNodes` shared library and installs artifacts in `build/bin/Release` and `build/lib/Release`.


## Platform Support

- Primary: Windows 10/11 (MSVC 2019 + Qt 6.7.x)
- Planned/possible (subject to CI and environment readiness):
  - Linux (gcc/clang, Qt 6)
  - macOS (Apple Clang, Qt 6)

`QtNodes` 3.x supports Windows/MSVC and has been used on Linux and macOS with Qt 5.15/6.x; `open_vp` currently targets Qt 6 on Windows.


## Contributing

We welcome contributions in the form of bug reports, feature proposals, and pull requests.

- Fork the repo and create a feature branch.
- Keep changes modular and follow CMake/Qt idioms used in this codebase.
- Add tests under `tests/` where feasible and ensure `ctest` passes.
- For larger changes, open an issue first to discuss design.
- Style: modern C++ (C++17), avoid global state, prefer clear separation of UI/core/nodes.
- Do not commit secrets/paths; avoid logging sensitive data.

PR checklist:
- Build succeeds in `Release` and `Debug`.
- Tests pass locally via `ctest`.
- No regressions in UI basic flows (launch, open/close graph, add/remove nodes).


## Known Issues

- On Windows, running tests/executables requires setting `PATH` to include the Qt bin and `build/bin/Release` to locate `QtNodes.dll`. Without this, you may see `0xc0000135` (missing DLL) when starting tests.
- `Qt6SerialPort` is optional. If not installed, serial features are disabled at build time (see `c:\Users\hmgics\projects\open_vp\CMakeLists.txt:15-21`). Install the Qt SerialPort component or via vcpkg to enable.
- Packaging requires `windeployqt` to copy Qt runtime and plugins next to the executable.


## Roadmap / TODO

- Add Linux/macOS build instructions and CI coverage.
- Expand hardware abstraction and SerialPort integrations.
- Extend node library (image processing, network protocols, analysis nodes).
- Plugin system for user-defined nodes and controls.
- Improve documentation and examples for custom nodes and UI controls.
- Create installer artifacts for Windows.
- Performance profiling and optimization on large graphs.


## Acknowledgments

- Built on top of the `QtNodes` library (Pavel K and contributors), consumed as a submodule at tag `3.0.12`.
