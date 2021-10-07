# Select the optimization level based on build type
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    set(RTOS_CFLAGS -Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    set(RTOS_CFLAGS -Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    set(RTOS_CFLAGS -Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    set(RTOS_CFLAGS -Og -g)
endif ()

# Select CFLAGS for instruction set
add_compile_options(-mcpu=cortex-m4 -mthumb)

add_compile_options(
        -nostdinc
        -nostdlibinc
        ${RTOS_CFLAGS}
        -Wno-incompatible-library-redeclaration
        -Wno-unused-command-line-argument
        )
add_link_options(-nostdlib)

#Uncomment for hardware floating point
#add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
#add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
#add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_compile_definitions(STM32F407xx)
