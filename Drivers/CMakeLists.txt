set(CMSIS_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32L4xx/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Include
        )

set(HAL_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/STM32L4xx_HAL_Driver/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
        )

file(GLOB HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/STM32L4xx_HAL_Driver/Src/*.c)

add_library(drivers STATIC
        ${HAL_SOURCES}
)

target_compile_definitions(drivers PUBLIC STM32L476xx)
target_include_directories(drivers PUBLIC
        ${CMSIS_INCLUDE}
        ${HAL_INCLUDE}
        )
