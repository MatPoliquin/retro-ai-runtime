# retro-ai-runtime Copilot Instructions

- This repository owns the C++ inference library and emulator-facing API. General Python training and export tools live in the separate stable-retro-scripts repository.
- Run build commands from the repository root: `cmake -S . -B build`, `cmake --build build`, and `ctest --test-dir build --output-on-failure`. Configure the LibTorch prefix and OpenCV location as described in README.md.
- Keep the `game_ai` library name and exported API in GameAI.h compatible with the RetroArchAI integration unless the task explicitly changes that interface.
- Follow .clang-format for C++ changes.
- Treat data/ as model and game-data assets. Do not rewrite or re-export models unless explicitly requested. Do not commit ROMs.
- Keep game-specific adapters under games/; the initial NHL94 adapter remains part of this runtime.
- Do not modify sibling repositories unless the task includes them.
