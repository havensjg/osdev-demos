cmake_minimum_required(VERSION 3.17.0)

# prevent cmake from making test executables??
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# set up i686-elf cross-compiler tools
include(toolchain-i686-elf.cmake)

project(OSDEV)

# enable assembly
enable_language(ASM)

# check that grub is installed
find_program(GRUB_EXECUTABLE grub-mkrescue REQUIRED)

# directory/ies containing header files
include_directories(include)

# set up iso file structure
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/isodir)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/isodir/boot)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/isodir/boot/grub)

# iso file
add_custom_target(livecd
    COMMAND ${GRUB_EXECUTABLE} -o ${CMAKE_CURRENT_BINARY_DIR}/myos.iso ${CMAKE_CURRENT_BINARY_DIR}/isodir
    VERBATIM
    )

# grub.cfg into isodir
add_dependencies(livecd grub_cfg)
add_custom_target(grub_cfg
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/src/grub.cfg
            ${CMAKE_CURRENT_BINARY_DIR}/isodir/boot/grub/grub.cfg
    )

# myos.bin into isodir
add_dependencies(livecd myos_bin)
add_custom_target(myos_bin
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/myos.bin
            ${CMAKE_CURRENT_BINARY_DIR}/isodir/boot/myos.bin
    DEPENDS myos.bin
    )

# myos.bin
file(GLOB C_SOURCES
    "include/*.h"
    "src/*.c"
    )
set_source_files_properties(${C_SOURCES} PROPERTIES COMPILE_OPTIONS "-g;-std=gnu99;-ffreestanding;-O2;-Wall;-Wextra")

file (GLOB ASM_SOURCES
    "src/*.s"
    )

add_executable(myos.bin
    ${C_SOURCES}
    ${ASM_SOURCES}
    )
set_target_properties(myos.bin PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/src/linker.ld)
target_link_libraries(myos.bin gcc)
target_link_options(myos.bin PUBLIC -ffreestanding -O2 -nostdlib -T ${CMAKE_SOURCE_DIR}/src/linker.ld)
