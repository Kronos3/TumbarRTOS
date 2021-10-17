add_library(rtos STATIC
        main.c
        semaphore.c
        fw.c
        scheduler.c
        scheduler.S
        tim.c
        io.c
        )

# target_link_libraries(rtos libc)
target_compile_options(rtos PUBLIC ${RTOS_CFLAGS})
