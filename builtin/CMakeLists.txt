add_library(compiler_builtin STATIC
        memset.S
        memset.c
        )

add_compile_options(-x assembler-with-cpp)
