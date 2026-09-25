# retro-ai-runtime

![C++ build](https://github.com/MatPoliquin/retro-ai-runtime/actions/workflows/test-cpp.yml/badge.svg)
![Clang format](https://github.com/MatPoliquin/retro-ai-runtime/actions/workflows/clang.yml/badge.svg)

C++ runtime for running AI models inside retro-game emulator frontends such as RetroArch
The shared library loads TorchScript models and returns controller inputs through
the C API in [GameAI.h](GameAI.h). It is used by the
[RetroArchAI integration](https://github.com/MatPoliquin/RetroArchAI).

This is an early prototype, extracted from `stable-retro-scripts/retro_ai_lib`.
General training, evaluation, and model export remain in
[stable-retro-scripts](https://github.com/MatPoliquin/stable-retro-scripts).
The existing NHL94 C++ adapter stays here for this initial extraction.

## Build on Linux

Requires a C++17 compiler, CMake 3.18 or newer, LibTorch, and OpenCV development
files. Commands below run from the repository root. LibTorch 2.3.1 is the existing
CI baseline carried over from stable-retro-scripts.

```bash
sudo apt update
sudo apt install -y git cmake build-essential wget unzip zlib1g-dev libopencv-dev

git clone https://github.com/MatPoliquin/retro-ai-runtime.git
cd retro-ai-runtime

wget -O libtorch.zip https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.3.1%2Bcpu.zip
unzip libtorch.zip

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$PWD/libtorch"
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

The library is `build/libgame_ai.so`. Its filename and exported functions are
unchanged so existing frontend integration code can continue loading it.

## Build on Windows

Install Visual Studio's C++ build tools, CMake, and OpenCV. From PowerShell in the
repository root, download the Release LibTorch package:

```powershell
Invoke-WebRequest -Uri 'https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-2.3.1%2Bcpu.zip' -OutFile libtorch.zip
Expand-Archive libtorch.zip -DestinationPath .

cmake -S . -B build -DCMAKE_PREFIX_PATH="$PWD/libtorch" -DOpenCV_DIR="C:/opencv/build"
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Set `OpenCV_DIR` to the directory containing your installation's
`OpenCVConfig.cmake`, and add its DLL directory to `PATH` before testing or using
the library. CMake copies LibTorch DLLs beside the built targets. With the Visual
Studio generator, the library is `build/Release/game_ai.dll`.

## Runtime data and models

- `GameAI.h` defines the interface called by the emulator frontend.
- `RetroModel.*` loads TorchScript `.pt` models using LibTorch.
- `games/` contains the NHL94 adapter and the default image-based adapter.
- `data/` contains the existing model files, memory maps, and game configurations.
- `utils/` contains memory and configuration helpers.

The runtime locates `config.json`, `data.json`, and model files relative to the
game path passed to `create_game_ai`. Keep the existing game directory names and
data layout when installing it with a frontend. ROMs are not included.

The smoke test loads the shared library, creates and destroys an NHL94 adapter,
and runs the bundled NHL94 `ScoreGoal.pt` model. CTest sets the working directory
so these data files can be found when building outside the source directory.
The test does not require a ROM or a running emulator.

Train and export models with
[`scripts/export_model.py`](https://github.com/MatPoliquin/stable-retro-scripts/blob/35ef1c1611a06e3c2cd91e6d548139bc395f4fa7/scripts/export_model.py)
in stable-retro-scripts. Exported observations, normalization, and controller
outputs must match the runtime adapter that uses the model.

## Source history

The initial runtime sources and data come from
[`stable-retro-scripts` at `35ef1c1`](https://github.com/MatPoliquin/stable-retro-scripts/tree/35ef1c1611a06e3c2cd91e6d548139bc395f4fa7/retro_ai_lib).
Earlier Git history remains in that repository. The extraction preserves the
model and game-data files byte for byte.

Licensed under the [MIT license](LICENSE).
