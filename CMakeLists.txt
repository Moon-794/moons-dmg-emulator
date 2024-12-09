cmake_minimum_required(VERSION 3.8)
project(emulator)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin/)
set(CMAKE_PREFIX_PATH "/mingw64")

set(SRC_FILES src/main.cpp src/mmu/mmu.cpp src/cpu/cpu.cpp src/cpu/instructions.cpp src/ppu/ppu.cpp src/GameBoy/gameboy.cpp src/RenderWindow/RenderWindow.cpp)
add_executable(emulator ${SRC_FILES})

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

target_include_directories(emulator PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include/)
target_link_libraries(emulator sfml-window-s sfml-graphics-s sfml-system-s opengl32 winmm)