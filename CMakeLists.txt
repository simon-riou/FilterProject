cmake_minimum_required(VERSION 3.28)
project(ImageFilterApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Fetch SFML
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

# ImGui sources
set(IMGUI_SRC
    imgui/imgui-SFML.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

# Gather all project sources
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

# Build executable
add_executable(ImageFilterApp
    ${SRC_FILES}
    ${IMGUI_SRC}
)

# Include directories
target_include_directories(ImageFilterApp PRIVATE
    include
    imgui
)

# Link libraries
target_link_libraries(ImageFilterApp PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
