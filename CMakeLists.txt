cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# build in release mode by default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# enable extra warnings
add_compile_options(-Wall -Wextra)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(puzzles)

# Add OpenGL and GLFW
if(POLICY CMP0072)
    # Prefer GLVND over legacy OpenGL modules
    cmake_policy(SET CMP0072 NEW)
endif()
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Don't build docs etc.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

add_subdirectory(glfw)

if(MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
endif()

add_executable(puzzles WIN32
    Direction.cpp
    main.cpp
    Pos.cpp
    UI.cpp
    utils.cpp
    VoxelPiece.cpp
    Voxels.cpp)

# Silence macOS OpenGL deprecation warnings
target_compile_definitions(puzzles PRIVATE GL_SILENCE_DEPRECATION=1)

target_link_libraries(puzzles ${OPENGL_LIBRARIES} glfw)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT puzzles)
endif()
