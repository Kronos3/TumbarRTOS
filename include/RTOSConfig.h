//
// Created by tumbar on 10/16/21.
//

#ifndef TUMBARRTOS_RTOSCONFIG_H
#define TUMBARRTOS_RTOSCONFIG_H

// Number of task priorities
#define OS_SCHED_PRI_N 32

// Number of events (x32) to support
#define OS_MAX_EVENT 4

#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#define NAKED __attribute__ ((naked))

#define configMAX_SYSCALL_INTERRUPT_PRIORITY (5<<(8-4))

#endif //TUMBARRTOS_RTOSCONFIG_H
