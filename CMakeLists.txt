cmake_minimum_required(VERSION 3.10)
project(emulator)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin/)
set(CMAKE_PREFIX_PATH "/mingw64")

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")

set(SRC_FILES src/main.cpp 
            src/joypad/joypad.cpp
            src/mmu/mmu.cpp 
            src/cpu/cpu.cpp 
            src/cpu/instructions/instructions.cpp
            src/cpu/instructions/instructionTable.cpp
            src/cpu/instructions/instructionTableEXT.cpp 
            src/ppu/ppu.cpp 
            src/GameBoy/gameboy.cpp 
            src/RenderWindow/RenderWindow.cpp)

add_executable(emulator ${SRC_FILES})
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

target_include_directories(emulator PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include/)
target_link_libraries(emulator sfml-window-s sfml-graphics-s sfml-system-s opengl32 winmm)