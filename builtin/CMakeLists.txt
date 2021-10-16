add_library(compiler_builtin STATIC
        builtin.S
        builtin.c
        )

add_compile_options(-x assembler-with-cpp)
