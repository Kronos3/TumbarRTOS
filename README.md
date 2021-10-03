# TumbarRTOS
This RTOS kernel implements a priority based tasking
with round-robin preemption at equal priority levels.

This project is meant to be a bare metal kernel to
learn how they operate on an embedded system.

## How to build
I'm fairly partial to the LLVM compiler because it
is a cross compiler by design. This CMake file will
expect a Clang/LLVM compiler as it passes the
`--target=armv7m-eabi-none` target triple.

> Note: To switch to GCC, install `arm-none-eabi-gcc` and
> remove this option from the CMake.

## Supported hardware
My target board is the STM32 Discovery F407 however this will
work on any STM32 (with M4-cortex) board if you provide the
linker script (`STM32F407VGTx_FLASH.ld`) and startup assembly
(`startup_stm32f407xx.s`). These can be taken from CubeMX or
simply found on Github.

