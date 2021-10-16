add_library(util STATIC
        uart.c
)

target_include_directories(util PUBLIC ../include)
