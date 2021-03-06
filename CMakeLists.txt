#
# CMakeLists.txt
#
# Todos os direitos reservados.
#
#

cmake_minimum_required(VERSION 3.0)
project(RealEngine)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)

# Project Include Directories
include_directories("Headers/")
include_directories("Headers/Components")
include_directories("Headers/Core")
include_directories("Headers/Device")
include_directories("Headers/Graphics")
include_directories("Headers/Managers")

if (WIN32)
    # For Windows Operating Systems

endif()

if (APPLE)
    # For Mac OS X Operating Systems
    include_directories("/System/Library/Frameworks")
    include_directories("/usr/lib")
    include_directories("/usr/local/Frameworks")
    include_directories("/usr/local/lib")

    # Find Library Locations
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(FREEIMAGE_LIBRARY FreeImage)
    find_library(GLFW_LIBRARY GLFW)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(IOSURFACE_LIBRARY IOSurface)
    find_library(LIBCPP_LIBRARY libc++.dylib)
    find_library(METAL_LIBRARY Metal)
    find_library(MOLTENVK_LIBRARY MoltenVK)
    find_library(QUARTZCORE_LIBRARY QuartzCore)

    # Link Libraries
    link_libraries(${FOUNDATION_LIBRARY})
    link_libraries(${FREEIMAGE_LIBRARY})
    link_libraries(${GLFW_LIBRARY})
    link_libraries(${IOKIT_LIBRARY})
    link_libraries(${IOSURFACE_LIBRARY})
    link_libraries(${LIBCPP_LIBRARY})
    link_libraries(${METAL_LIBRARY})
    link_libraries(${MOLTENVK_LIBRARY})
    link_libraries(${QUARTZCORE_LIBRARY})
endif()

if (UNIX AND NOT APPLE)
    # For Linux and BSD Operating Systems
    link_libraries(freeimage)
    link_libraries(glfw)
    link_libraries(pthread)
    link_libraries(vulkan)
endif()

# Header Files
set(HEADERS Headers/Core/Error.h
        Headers/Core/Result.h
        Headers/Core/Types.h
        Headers/Device/Allocator.h
        Headers/Device/Buffer.h
        Headers/Device/Instance.h
        Headers/Device/Device.h
        Headers/Device/Memory.h
        Headers/Device/PoolAllocator.h
        Headers/Device/Queue.h
        Headers/Device/Image.h
        Headers/Graphics/Material.h
        Headers/Graphics/Renderer.h
        Headers/Graphics/Texture.h
        Headers/Graphics/Window.h
        Headers/Components/SpriteComponent.h
        Headers/Managers/GraphicsManager.h
        Headers/Managers/MemoryManager.h
        Headers/Managers/WindowManager.h
        Headers/Managers/WorldManager.h
        Headers/Core/Game.h)

# Source Files
set(SOURCES Sources/Device/Buffer.cpp
        Sources/Device/Instance.cpp
        Sources/Device/Device.cpp
        Sources/Device/Memory.cpp
        Sources/Device/PoolAllocator.cpp
        Sources/Device/Image.cpp
        Sources/Device/Queue.cpp
        Sources/Graphics/Material.cpp
        Sources/Graphics/Renderer.cpp
        Sources/Graphics/Texture.cpp
        Sources/Graphics/Window.cpp
        Sources/Components/SpriteComponent.cpp
        Sources/Managers/GraphicsManager.cpp
        Sources/Managers/MemoryManager.cpp
        Sources/Managers/WindowManager.cpp
        Sources/Managers/WorldManager.cpp
        Sources/Core/Game.cpp)

# Copy Shaders
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/vert.spv
        ${CMAKE_CURRENT_BINARY_DIR}/Shaders
        COPYONLY
)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/frag.spv
        ${CMAKE_CURRENT_BINARY_DIR}/Shaders
        COPYONLY
)

add_executable(RealEngine main.cpp ${SOURCES} ${HEADERS})
