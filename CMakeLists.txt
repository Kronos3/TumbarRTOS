cmake_minimum_required(VERSION 3.20)
#set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_ARM "Build for ARM target" OFF)

# specify cross compilers and tools
if (BUILD_ARM)
#    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
#    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
#    set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
#    set(CMAKE_AR arm-none-eabi-ar)
    set(CMAKE_OBJCOPY /home/tumbar/arm-gcc/arm-none-eabi-objcopy)
    set(CMAKE_OBJDUMP /home/tumbar/arm-gcc/arm-none-eabi-objdump)
    set(SIZE arm-none-eabi-size)

    #Uncomment for hardware floating point
    #add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
    #add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
    #add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

    add_compile_options(--target=armv7m-none-eabi -mcpu=cortex-m4 -mthumb)
    # add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)
    add_link_options(--target=armv7m-none-eabi)

    # uncomment to mitigate c++17 absolute addresses warnings
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

project(TumbarRTOS)
enable_language(C)
enable_language(ASM)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

include_directories(include)
add_subdirectory(core)

set(SOURCES)
set(MODULES
        core)

# Set up the targets
if(BUILD_ARM)
    # Don't realtime test compiler
    set(CMAKE_C_COMPILER_WORKS 1)

    set(SOURCES ${SOURCES} startup_stm32f407xx.s)

    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld)
    add_link_options(-Wl,-gc-sections,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
    add_link_options(-mcpu=cortex-m4 -mthumb)
    add_link_options(-T ${LINKER_SCRIPT})

    # Use a different libc
    add_link_options(-nostdlib)

    add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
    target_link_libraries(${PROJECT_NAME}.elf ${MODULES})

    set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
    set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

    add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
            COMMENT "Building ${HEX_FILE}
    Building ${BIN_FILE}")
else()
    # TODO(tumbar) Set up RTOS target for host testing
endif()
