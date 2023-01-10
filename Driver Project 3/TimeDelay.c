/* 
 * File:   TimeDelay.c
 * Author: nadia
 *
 * Created on October 4, 2022, 5:29 PM
 */
#include <stdlib.h>
#include <p24F16KA101.h>
#include "TimeDelay.h"
#include "xc.h"

// uint32_t TMR2flag;

/*
 * 
 */
void delay_ms(uint16_t time_ms){
    
  T2CONbits.TON = 1; // Starts Timer2
    

    IPC1bits.T2IP0 = 1;  // Interrupt Priority bit 0
    IPC1bits.T2IP1 = 1;  // Interrupt Priority bit 1
    IPC1bits.T2IP2 = 1;  // Interrupt Priority bit 2
    
    IEC0bits.T2IE  = 1;  // Enable interrupt
    IFS0bits.T2IF  = 0;  // Clear timer 2 interrupt flag

    PR2 = time_ms  * 16;  // PR2 = time_mx / (prescaler * (2/fclk) * 1000) , 
    // default prescaler = 1 and flck = 32khz -> PR2 = time_ms / (2/32) = time_ms * 16
    
    Idle();  // Idle state until another interrupt happens
    
    
}

// Timer 2 subroutine 
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void){
 
    IFS0bits.T2IF = 0;    // Clear timer 2 interrupt flag
    T2CONbits.TON = 0;    // Stops 32-bit Timer2


}