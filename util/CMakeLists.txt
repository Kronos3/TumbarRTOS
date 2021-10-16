add_library(util STATIC
        uart.c
)

target_include_directories(util PUBLIC ../include)
target_link_libraries(util drivers)
