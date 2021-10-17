set(RTOS_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/fw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/scheduler.c
        ${CMAKE_CURRENT_SOURCE_DIR}/scheduler.S
        ${CMAKE_CURRENT_SOURCE_DIR}/tim.c
        ${CMAKE_CURRENT_SOURCE_DIR}/led.c
        ${CMAKE_CURRENT_SOURCE_DIR}/io.c
        PARENT_SCOPE
)

set(RTOS_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        PARENT_SCOPE
)
