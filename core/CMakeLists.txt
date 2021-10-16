add_library(core STATIC
        main.c
        semaphore.c
        fw.c
        scheduler.c
        scheduler.S
        tim.c
        )

# target_link_libraries(core libc)
target_compile_options(core PUBLIC ${RTOS_CFLAGS})
