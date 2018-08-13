cmake_minimum_required(VERSION 3.12)
project(RealEngine)

set(CMAKE_CXX_STANDARD 14)

# Include Directories
include_directories("Headers/")
include_directories("Headers/Core")
include_directories("Headers/Device")
include_directories("Headers/Graphics")
include_directories("Headers/Managers")

# Libraries
link_libraries(freeimage)
link_libraries(vulkan)

# Header Files
set(HEADERS Headers/Core/Result.h Headers/Core/Error.h Headers/Core/Types.h Headers/Device/Instance.h Headers/Device/Device.h)

# Source Files
set(SOURCES Sources/Device/Instance.cpp Sources/Device/Device.cpp)

add_executable(RealEngine main.cpp ${SOURCES} ${HEADERS})