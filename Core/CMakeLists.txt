file(GLOB CORE_SOURCES Src/*.c)

add_library(core STATIC
        ${CORE_SOURCES}
)

target_include_directories(core PUBLIC Inc)
target_link_libraries(core drivers)
