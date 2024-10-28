cmake_minimum_required(VERSION 3.8)
project(emulator)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin/)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(SFML_STATIC_LIBRARIES TRUE)
set(CMAKE_CXX_COMPILER "c++")

set(SRC_FILES src/main.cpp src/mmu/mmu.cpp src/cpu/cpu.cpp src/cpu/instructions.cpp src/ppu/ppu.cpp src/GameBoy/gameboy.cpp)
add_executable(emulator ${SRC_FILES})

target_link_directories(emulator PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(emulator PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include/)

target_link_libraries(emulator libsfml-graphics-s.a libsfml-window-s.a libsfml-system-s.a opengl32 gdi32 winmm)