//
// Created by tumbar on 10/16/21.
//

#ifndef TUMBARRTOS_RTOSCONFIG_H
#define TUMBARRTOS_RTOSCONFIG_H

// Number of task priorities
#define OS_SCHED_PRI_N 32

// Number of events (x32) to support
#define OS_MAX_EVENT 4

// General purpose registers (excluding SP, LR, PC)
#define M4_GPR_N 13

#ifdef M4_FPU
// Single precision registers
#define M4_SP_N 32

// Double precision registers
#define M4_DP_N 16
#endif

#endif //TUMBARRTOS_RTOSCONFIG_H
