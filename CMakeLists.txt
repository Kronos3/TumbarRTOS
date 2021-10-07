cmake_minimum_required(VERSION 3.20)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(TumbarRTOS C ASM)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

include(toolchain/llvm.cmake)  # Change this to gcc if you want
include(cmake/build.cmake)

add_subdirectory(builtin)

#############################
# Main kernel sources
#############################
include_directories(include)
add_subdirectory(core)
add_subdirectory(board)

set(MODULES compiler_builtin board core)
set(SOURCES startup_stm32f407xx.s)

# Set up the linking program
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld)
add_link_options(-Wl,-gc-sections,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m4 -mthumb)
add_link_options(-T ${LINKER_SCRIPT})

############################
# Main target binary
############################
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
target_link_libraries(
        ${PROJECT_NAME}.elf
        ${MODULES}
)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
    Building ${BIN_FILE}")