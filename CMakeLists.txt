cmake_minimum_required(VERSION 3.8)
project(emulator)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin/)

set(SRC_FILES src/main.cpp src/mmu/mmu.cpp src/cpu/cpu.cpp src/ppu/ppu.cpp src/GameBoy/gameboy.cpp)
add_executable(emulator ${SRC_FILES})

target_include_directories(emulator PRIVATE ${CMAKE_SOURCE_DIR}/src $ENV{VCPKG_ROOT}/installed/x64-windows/include)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(emulator OpenGL::GL glm::glm-header-only glfw)