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

void timer2_delay(uint16_t time_ms);
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void);

void timer3_delay(uint16_t time_ms);
void __attribute__((interrupt, no_auto_psv))_T3Interrupt(void);

#endif		