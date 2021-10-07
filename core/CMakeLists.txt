add_library(core STATIC
        arm.s
        main.c
        fw.c
        scheduler.c
        scheduler.S
        )

# target_link_libraries(core libc)
target_compile_options(core PUBLIC ${RTOS_CFLAGS})
