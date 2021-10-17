include(toolchain/gcc.cmake)  # Change this to gcc if you want


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

set(BOARD_NAME STM32L476RGTX)

# Select CFLAGS for instruction set
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx)
add_compile_options(-mcpu=cortex-m4 -mthumb)

# Set up the linking program
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/${BOARD_NAME}_FLASH.ld)
add_link_options(-Wl,-gc-sections,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m4 -mthumb)
add_link_options(-T ${LINKER_SCRIPT})

add_compile_options(
        ${RTOS_CFLAGS}
        -Wno-incompatible-library-redeclaration
        -Wno-unused-command-line-argument
        )

#Uncomment for hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#############################
# Main kernel sources
#############################
include_directories(include)
add_subdirectory(Drivers)
add_subdirectory(Core)
add_subdirectory(rtos)

set(SOURCES
        ${DRIVER_SOURCES}
        ${CORE_SOURCES}
        ${LINKER_SCRIPT}
        ${RTOS_SOURCES}
        Core/Startup/startup_stm32l476rgtx.s
)

add_compile_definitions(STM32L476xx)

############################
# Main target binary
############################
add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(
        ${PROJECT_NAME}.elf PUBLIC
        ${DRIVER_INCLUDES}
        ${CORE_INCLUDES}
        ${RTOS_INCLUDES}
)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
    Building ${BIN_FILE}")

