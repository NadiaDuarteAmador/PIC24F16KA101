/* 
 * File:   main.c
 * Author: nadia
 *
 * Created on January 9, 2017, 5:26 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "xc.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <math.h>
#include <errno.h>

#include "TimeDelay.h"
#include "IOs.h"
#include "ChangeClk.h"
#include "UART2.h"

//// CONFIGURATION BITS - PRE-PROCESSOR DIRECTIVES ////

#pragma config ICS = PGx2 // PGC2 (pin2) & PGD2 (pin3) are used to connect PICKIT3 debugger

// CLOCK Configuration 
#pragma config IESO = OFF    // 2 Speed Startup disabled
#pragma config FNOSC = FRC  // Start up CLK = 8 MHz

// CLOCK Switching Configuration 
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled 

#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation
#pragma config POSCFREQ = MS  //Primary Oscillator/External clk freq betwn 100kHz and 8 MHz. Options: LS, MS, HS
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO. 
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled

// WDT Configuration 
#pragma config FWDTEN = OFF // WDT is off
#pragma config WINDIS = ON // STANDARD WDT/. Applicable if WDT is on
#pragma config FWPSA = PR32 // WDT is selected uses prescaler of 32
#pragma config WDTPS = PS2048 // WDT postscler is 1 if WDT selected

// Deep Sleep RTCC WDT
#pragma config DSWDTEN = OFF // Deep Sleep WDT is disabled
#pragma config DSBOREN = OFF // Deep Sleep BOR is disabled
#pragma config RTCOSC = LPRC// RTCC uses LPRC 32kHz for clock
#pragma config DSWDTOSC = LPRC // DeepSleep WDT uses Lo Power RC clk
#pragma config DSWDTPS = DSWDTPS7 // DSWDT postscaler set to 32768 

// MACROS
#define Nop() {__asm__ volatile ("nop");} // no operation
#define ClrWdt() {__asm__ volatile ("clrwdt");} // timers that reset every user defined time
#define Sleep() {__asm__ volatile ("pwrsav #0");}   // set sleep mode, CPU OFF - SOME PERIPHERALS ON
#define Idle() {__asm__ volatile ("pwrsav #1");} // iddle mode, CPU OFF - PERIPHERALS ON
#define dsen() {__asm__ volatile ("BSET DSCON, #15");}

// HDVL subroutine
void __attribute__((interrupt, no_auto_psv))_HLVDInterrupt(void){
 
    IFS4bits.HLVDIF = 0;    // Clear HLVD interrupt flag
    IPC18bits.HLVDIP = 111; // priority bits, highest priority
    IEC4bits.HLVDIE = 0; // clear interrupt flag
   
}

        
int main(void) {
    
    /// HDVL setup
    HLVDCONbits.HLVDEN = 1; // high/low voltage detect power enable bit. Powers on HDVL Module
    HLVDCONbits.HLSIDL = 0; // continue operation in idle mode
    HLVDCONbits.VDIR = 1; // event occurs when voltage equals or exceeds trip point. To monitor voltage overcharging
    HLVDCONbits.HLVDL = 1101; // trip point set to a max of 3.28V and a min of 2.96V
    HLVDCONbits.BGVST = 1; // band gap voltage is stable
    HLVDCONbits.IRVST = 1; // internal reference voltage is stable
 
    //Clock switching        
    //Clock output on REFO pin - pin 18 RB15
     TRISBbits.TRISB15 = 0;  // Set RB15 as output for REFO
     REFOCONbits.ROEN = 1; // Ref oscillator is enabled
     REFOCONbits.ROSSLP = 0; // Ref oscillator is disabled in sleep
     REFOCONbits.ROSEL = 0; // Output base clk showing clock switching
     REFOCONbits.RODIV = 0b0000;
     
    //IO Initializations
    AD1PCFG = 0xFFFF;  // Turn all analog pins to digital
     
    NewClk(32);  // 8 for 8MHZ (9600 baud rate), 32 for 32 kHz (300 baud rate) and 500 for 500kHz. 
    RCONbits.SWDTEN = 1; // turns of WDT
     
    IOinit(); // Enables IO and CN interrupts on push buttons
    InitUART2(); // Configure UART2, Enable IO in UART2
    
    // Initialize IOs for low-power wake-up
    while(1)  // Infinite while loop
     {
      
        Idle(); // Turn CPU off, consumes little power
        if (RCONbits.WDTO == 1) // check if WDT woke up MCU from idle mode
        {
            IOcheck(); 
        }
                    
        }
    return 0;
}