/* 
 * File:   
 * Author: Shahdad
 * Comments:
 * Revision history: 
 */
 
#ifndef TIMEDELAY_H
#define	TIMEDELAY_H

#include <xc.h>  
#include <stdio.h>
#include <stdlib.h>

void delay_ms(uint16_t time_ms);
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void);

#endif		