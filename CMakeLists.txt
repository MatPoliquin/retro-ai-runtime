cmake_minimum_required(VERSION 3.18)
project(retro_ai_runtime LANGUAGES CXX)

find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)

# Keep the library name and exported C API compatible with RetroArchAI.
add_library(game_ai SHARED
    GameAILocal.cpp
    RetroModel.cpp
    games/NHL94GameAI.cpp
    games/NHL94GameData.cpp
    games/DefaultGameAI.cpp
    utils/data.cpp
    utils/memory.cpp
    utils/utils.cpp
)
target_include_directories(game_ai PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(game_ai PRIVATE "${TORCH_LIBRARIES}" ${OpenCV_LIBS})
set_target_properties(game_ai PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

include(CTest)
if(BUILD_TESTING)
    add_executable(runtime_test test.cpp RetroModel.cpp)
    target_include_directories(runtime_test PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(runtime_test PRIVATE "${TORCH_LIBRARIES}" ${OpenCV_LIBS} ${CMAKE_DL_LIBS})
    set_target_properties(runtime_test PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
    add_dependencies(runtime_test game_ai)

    add_test(NAME runtime_smoke
        COMMAND runtime_test "$<TARGET_FILE:game_ai>"
    )
    set_tests_properties(runtime_smoke PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

if(MSVC)
    file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
    foreach(runtime_target game_ai runtime_test)
        if(TARGET ${runtime_target})
            add_custom_command(TARGET ${runtime_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${TORCH_DLLS}
                    "$<TARGET_FILE_DIR:${runtime_target}>"
                COMMAND_EXPAND_LISTS
            )
        endif()
    endforeach()
endif()
