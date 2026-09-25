// Smoke test for the shared library and bundled TorchScript model.
#include "GameAI.h"
#include "RetroModel.h"
#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void test_loadlibrary(std::map<std::string, bool>& tests, const char* library_path)
{
#ifdef _WIN32
    HMODULE library = LoadLibraryA(library_path);
    if (!library)
    {
        std::cerr << "LoadLibrary failed: " << GetLastError() << '\n';
        return;
    }
    auto create = reinterpret_cast<create_game_ai_t>(
        GetProcAddress(library, "create_game_ai"));
    auto destroy = reinterpret_cast<destroy_game_ai_t>(
        GetProcAddress(library, "destroy_game_ai"));
#else
    void* library = dlopen(library_path, RTLD_NOW);
    if (!library)
    {
        std::cerr << "dlopen failed: " << dlerror() << '\n';
        return;
    }
    auto create = reinterpret_cast<create_game_ai_t>(
        dlsym(library, "create_game_ai"));
    auto destroy = reinterpret_cast<destroy_game_ai_t>(
        dlsym(library, "destroy_game_ai"));
#endif

    tests["LOAD LIBRARY"] = true;
    tests["GET CREATEGAME FUNC"] = create != nullptr;
    tests["GET DESTROYGAME FUNC"] = destroy != nullptr;
    if (create && destroy)
    {
        // Construction only uses the path; no ROM is needed for this test.
        void* game = create("./data/NHL941on1-Genesis/NHL941on1.md");
        if (game)
        {
            destroy(game);
            tests["CREATE AND DESTROY GAME"] = true;
        }
    }

#ifdef _WIN32
    FreeLibrary(library);
#else
    dlclose(library);
#endif
}

void test_pytorch(std::map<std::string, bool>& tests)
{
    RetroModelPytorch model;
    model.LoadModel("./data/NHL941on1-Genesis/ScoreGoal.pt");

    std::vector<float> input(16);
    std::vector<float> output(12);
    model.Forward(output, input);
    for (float action : output)
    {
        if (!std::isfinite(action))
        {
            throw std::runtime_error("Model returned a non-finite action");
        }
    }
    tests["LOAD AND RUN PYTORCH MODEL"] = true;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: runtime_test <shared-library-path>\n";
        return 1;
    }

    std::map<std::string, bool> tests = {
        {"LOAD LIBRARY", false},
        {"GET CREATEGAME FUNC", false},
        {"GET DESTROYGAME FUNC", false},
        {"CREATE AND DESTROY GAME", false},
        {"LOAD AND RUN PYTORCH MODEL", false},
    };

    try
    {
        test_loadlibrary(tests, argv[1]);
        test_pytorch(tests);
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
    }

    int result = 0;
    for (const auto& test : tests)
    {
        std::cout << test.first << "..." << (test.second ? "PASS" : "FAIL")
                  << '\n';
        if (!test.second)
        {
            result = 1;
        }
    }
    return result;
}
