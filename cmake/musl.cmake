set(MUSL_SRC ${PROJECT_SOURCE_DIR}/musl)

option(LIBC_STATIC "Build libc (musl) statically" ON)
set(LIBC_ARCH "arm" CACHE STRING "Target musl architecture (look in musl/arch)")

set(COMPAT_32_ARCHES
        m68k
        microblaze
        arm
        powerpc
        mipsn32
        or1k
        i386
        mips
        sh
)
if (LIBC_ARCH IN_LIST COMPAT_32_ARCHES)
    file(GLOB COMPAT_SRCS ${MUSL_SRC}/compat/time32/*.c)
else()
    file(GLOB COMPAT_SRCS)
endif()

function(file_multidir )
endfunction()

set(MALLOC_DIR mallocng)
file(GLOB BASE_SRCS
        ${MUSL_SRC}/src/**/*.c
        ${MUSL_SRC}/src/malloc/${MALLOC_DIR}/*.c)
file(GLOB ARCH_SRCS
        ${MUSL_SRC}/${LIBC_ARCH}/*.[csS])

set(MUSL_GENERATED_INC ${CMAKE_CURRENT_BINARY_DIR}/musl)
file(MAKE_DIRECTORY ${MUSL_GENERATED_INC})
file(MAKE_DIRECTORY ${MUSL_GENERATED_INC}/bits)

add_custom_command(
        OUTPUT ${MUSL_GENERATED_INC}/bits/alltypes.h
        COMMAND sed -f ${MUSL_SRC}/tools/mkalltypes.sed
            ${MUSL_SRC}/arch/${LIBC_ARCH}/bits/alltypes.h.in
            ${MUSL_SRC}/include/alltypes.h.in
                > ${MUSL_GENERATED_INC}/bits/alltypes.h
        DEPENDS
            ${MUSL_SRC}/arch/${LIBC_ARCH}/bits/alltypes.h.in
            ${MUSL_SRC}/include/alltypes.h.in
            ${MUSL_SRC}/tools/mkalltypes.sed
)

add_custom_command(
        OUTPUT ${MUSL_GENERATED_INC}/bits/syscall.h
        COMMAND cp ${MUSL_SRC}/arch/${LIBC_ARCH}/bits/syscall.h.in ${MUSL_GENERATED_INC}/bits/syscall.h &&
            sed -n -e s/__NR_/SYS_/p
                < ${MUSL_SRC}/arch/${LIBC_ARCH}/bits/syscall.h.in
                >> ${MUSL_GENERATED_INC}/bits/syscall.h
        DEPENDS ${MUSL_SRC}/arch/${LIBC_ARCH}/bits/syscall.h.in
)

execute_process(
        COMMAND sh ${MUSL_SRC}/tools/version.sh
        OUTPUT_VARIABLE MUSL_VERSION
        WORKING_DIRECTORY ${MUSL_SRC})
string(STRIP ${MUSL_VERSION} MUSL_VERSION)
file(WRITE ${MUSL_GENERATED_INC}/version.h "#define VERSION \"${MUSL_VERSION}\"\n")

set(GENH
        ${MUSL_GENERATED_INC}/bits/alltypes.h
        ${MUSL_GENERATED_INC}/bits/syscall.h
        ${MUSL_GENERATED_INC}/version.h)

set(ALL_SRCS
        ${COMPAT_SRCS}
        ${BASE_SRCS}
        ${ARCH_SRCS}
        ${GENH})

if (LIBC_STATIC)
    add_library(musl STATIC ${ALL_SRCS})
else()
    add_library(musl SHARED ${ALL_SRCS})
endif()

target_include_directories(musl PRIVATE
        ${MUSL_SRC}/src/internal
        ${MUSL_SRC}/src/include
        ${MUSL_SRC}/arch/${LIBC_ARCH}
        ${MUSL_SRC}/arch/generic
        ${MUSL_GENERATED_INC}
        )

target_compile_options(musl PRIVATE
        -fomit-frame-pointer -fno-unwind-tables
        -fno-asynchronous-unwind-tables -ffunction-sections
        -fdata-sections -w -Wno-pointer-to-int-cast
        -Werror=implicit-function-declaration
        -Werror=implicit-int -Werror=pointer-sign
        -Werror=pointer-arith -Werror=int-conversion
        -Werror=incompatible-pointer-types -Qunused-arguments
        -Waddress -Warray-bounds -Wchar-subscripts
        -Wduplicate-decl-specifier -Winit-self -Wreturn-type
        -Wsequence-point -Wstrict-aliasing -Wunused-function
        -Wunused-label -Wunused-variable
        )

# Public libc headers
target_include_directories(musl PUBLIC ${MUSL_SRC}/include)
target_compile_definitions(musl PUBLIC _XOPEN_SOURCE=700)
