/* 
 * This file is part of the TumbarRTOS
 * Copyright (c) 2021 Andrei Tumbar.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef TUMBARRTOS_ARM_H
#define TUMBARRTOS_ARM_H

/**
 * Mask all CPU interrupts
 */
void interrupt_disable(void);

/**
 * Unmask all CPU interrupts
 */
void interrupt_enable(void);

#endif //TUMBARRTOS_ARM_H
