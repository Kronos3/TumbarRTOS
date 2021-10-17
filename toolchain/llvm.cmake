# LLVM is cross-compiler, build for ARMv7m
add_compile_options(--target=armv7m-none-eabi)
add_link_options(--target=armv7m-none-eabi)
#add_compile_options(-mimplicit-it=always)

set(CMAKE_AR llvm-ar)
set(TOOLCHAIN_PATH "" CACHE STRING "Path to arm-none-eabi toolchain (bin/lib/include)")

if (TOOLCHAIN_PATH STREQUAL "")
    message(STATUS "No toolchain path was provided, using /usr/arm-none-eabi")
    # set(CMAKE_SYSROOT /usr/arm-none-eabi)
    add_compile_options(--sysroot=/usr/arm-none-eabi)
else()
    message(STATUS "Using toolchain headers: ${TOOLCHAIN_PATH}")
    set(CMAKE_SYSROOT ${TOOLCHAIN_PATH})
endif()

# GNU objcopy/objdump is not cross platform
# Glorious llvm is :)
set(CMAKE_OBJCOPY llvm-objcopy)
set(CMAKE_OBJDUMP llvm-objdump)
