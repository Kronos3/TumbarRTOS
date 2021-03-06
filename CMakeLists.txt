cmake_minimum_required(VERSION 3.13)
project(TumbarRTOS)
enable_language(C ASM)
set(CMAKE_C_STANDARD 99)

option(DEVELOPMENT "Use default settings for standalone development (i.e. no super project is being used)" OFF)

if (DEVELOPMENT)
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
    set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
    set(CMAKE_AR arm-none-eabi-ar)
    set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
    set(CMAKE_OBJDUMP arm-none-eabi-objdump)
    set(SIZE arm-none-eabi-size)

    add_compile_options(-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16)
    add_link_options(-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16)
endif()

add_library(
        tumbar_rtos
        scheduler.c
        scheduler.S
        semaphore.c
        tim.c
)
target_include_directories(tumbar_rtos PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(tumbar_rtos PUBLIC __TUMBAR_RTOS__)
