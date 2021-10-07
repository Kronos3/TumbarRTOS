add_library(board STATIC
        board.c
        )

# target_link_libraries(core libc)
target_compile_options(core PUBLIC ${RTOS_CFLAGS})
