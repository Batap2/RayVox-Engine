# Batap Engine
 
## Documentation
https://batap2.github.io/Batap-Engine/

## Build

### Requirements
Make sure you have the following installed before building:
- [CMake](https://cmake.org/) ≥ 3.19
- [Ninja](https://ninja-build.org/) (required for building with the presets)
- Visual Studio 2022 with "Desktop development with C++" workload 
- [Vulkan SDK](https://vulkan.lunarg.com/) (headers + dxc; located via the `VULKAN_SDK` env var set by the installer)

#### Using CMake Presets
This project includes a `CMakePresets.json` file, so you can build it easily:

```bat
build_msvc.bat <preset-name> [--configure]
```

- `<preset-name>` : name of the CMake preset to use (as defined in `CMakePresets.json`)
  - `msvc-release`
  - `msvc-debug`
  - `msvc-relwithdebinfo`
  - `msvc-debug-asan`
- `--configure` : Forces a CMake reconfiguration before building. Use this on the first build or after changes to CMake files or source layout.

VSCode users: build tasks are already defined in the `.vscode` folder.

## Run

Binaries land in `build/<preset>/bin/`. Three executables:

```bat
bin\GameExemple_Editor.exe --project GameExemple   :: the editor of the example game (its components included)
bin\GameExemple.exe        --project GameExemple   :: the game alone
bin\Batap_Editor.exe                               :: bare editor, engine components only
```

`--project <dir>` points a build-tree exe at its assets; without it, assets
are expected next to the executable.