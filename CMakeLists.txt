#
# CMakeLists.txt
#
# Todos os direitos reservados.
#
#

cmake_minimum_required(VERSION 3.12)
project(RealEngine)

# C++ Standard
set(CMAKE_CXX_STANDARD 14)

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
    find_library(LIBGTEST_LIBRARY libgtest.a)
    find_library(LIBGTEST_MAIN_LIBRARY libgtest_main.a)
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
    link_libraries(${LIBGTEST_LIBRARY})
    link_libraries(${LIBGTEST_MAIN_LIBRARY})
    link_libraries(${METAL_LIBRARY})
    link_libraries(${MOLTENVK_LIBRARY})
    link_libraries(${QUARTZCORE_LIBRARY})
endif()

if (UNIX AND NOT APPLE)
    # For Linux and BSD Operating Systems
    link_libraries(freeimage)
    link_libraries(glfw)
    link_libraries(gtest)
    link_libraries(gtest_main)
    link_libraries(pthread)
    link_libraries(vulkan)
endif()

# Header Files
set(HEADERS Headers/Core/Result.h
        Headers/Core/Error.h
        Headers/Core/Types.h
        Headers/Device/Instance.h
        Headers/Device/Device.h
        Headers/Managers/GraphicsManager.h
        Headers/Device/Memory.h
        Headers/Managers/MemoryManager.h
        Headers/Device/Allocator.h
        Headers/Device/PoolAllocator.h
        Headers/Device/Buffer.h
        Headers/Device/Queue.h
        Headers/Device/Image.h
        Headers/Graphics/Renderer.h
        Headers/Device/Command.h
        Headers/Graphics/Texture.h
        Headers/Graphics/Window.h
        Headers/Graphics/Material.h
        Headers/Components/SpriteComponent.h Headers/Managers/WindowManager.h)

# Source Files
set(SOURCES Sources/Device/Instance.cpp
        Sources/Device/Device.cpp
        Sources/Managers/GraphicsManager.cpp
        Sources/Device/Memory.cpp
        Sources/Managers/MemoryManager.cpp
        Sources/Device/PoolAllocator.cpp
        Sources/Device/Buffer.cpp
        Sources/Device/Image.cpp
        Sources/Device/Queue.cpp
        Sources/Graphics/Renderer.cpp
        Sources/Graphics/Texture.cpp
        Sources/Graphics/Window.cpp
        Sources/Graphics/Material.cpp
        Sources/Components/SpriteComponent.cpp
        Tests/TestManagers.cpp
        Tests/TestManagerFixture.h
        Tests/TestResources.cpp Sources/Managers/WindowManager.cpp)

add_executable(RealEngine main.cpp ${SOURCES} ${HEADERS})