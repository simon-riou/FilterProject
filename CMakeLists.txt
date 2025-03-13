cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(IMGUI_SRC
    imgui/imgui-SFML.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

add_executable(main src/main.cpp ${IMGUI_SRC} "include/filters.h" "include/image_encoding.h" "include/kernel_computation.h" )
target_compile_features(main PRIVATE cxx_std_17)
target_include_directories(main PRIVATE imgui/)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System)
