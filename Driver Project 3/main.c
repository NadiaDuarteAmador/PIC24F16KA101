/* 
 * File:   main.c
 * Author: 
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


//// CONFIGURATION BITS - PRE-PROCESSOR DIRECTIVES ////

// Code protection 
#pragma config BSS = OFF // Boot segment code protect disabled
#pragma config BWRP = OFF // Boot sengment flash write protection off
#pragma config GCP = OFF // general segment code protecion off
#pragma config GWRP = OFF

// CLOCK CONTROL 
#pragma config IESO = OFF    // 2 Speed Startup disabled
#pragma config FNOSC = FRC  // Start up CLK = 8 MHz
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled
#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation
#pragma config POSCFREQ = MS  //Primary Oscillator/External clk freq betwn 100kHz and 8 MHz. Options: LS, MS, HS
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO. 
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled

// WDT
#pragma config FWDTEN = OFF // WDT is off
#pragma config WINDIS = OFF // STANDARD WDT/. Applicable if WDT is on
#pragma config FWPSA = PR32 // WDT is selected uses prescaler of 32
#pragma config WDTPS = PS1 // WDT postscler is 1 if WDT selected

//MCLR/RA5 CONTROL
#pragma config MCLRE = OFF // RA5 pin configured as input, MCLR reset on RA5 diabled

//BOR  - FPOR Register
#pragma config BORV = LPBOR // LPBOR value=2V is BOR enabled
#pragma config BOREN = BOR0 // BOR controlled using SBOREN bit
#pragma config PWRTEN = OFF // Powerup timer disabled
#pragma config I2C1SEL = PRI // Default location for SCL1/SDA1 pin

//JTAG FICD Register
#pragma config BKBUG = OFF // Background Debugger functions disabled
#pragma config ICS = PGx2 // PGC2 (pin2) & PGD2 (pin3) are used to connect PICKIT3 debugger

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

// GLOBAL VARIABLES

 uint8_t CNflag = 0;


        
int main(void) {
    
     //Clock output on REFO pin - pin 18 RB15
     TRISBbits.TRISB15 = 0;  // Set RB15 as output for REFO
     REFOCONbits.ROEN = 1; // Ref oscillator is enabled
     REFOCONbits.ROSSLP = 0; // Ref oscillator is disabled in sleep
     REFOCONbits.ROSEL = 0; // Output base clk showing clock switching
     REFOCONbits.RODIV = 0b0000;
     
    //IO Initializations
    AD1PCFG = 0xFFFF;  // Turn all analog pins to digital
     
    NewClk(32);  // 32 kHz
    IOinit(); // Enables IO and CN interrupts on push buttons
    
    // Initialize IOs for low-power wake-up
    while(1)  // Infinite while loop
     {
        if (CNflag == 1) // Check to see if flag is 1 for Change of Notification Interrupt
        {
            CNflag = 0; // Check the Change of Notification (Check if a push button was pressed)
            IOcheck();    // Checks push button state to turn LED ON
        }
        
        LATBbits.LATB8 = 0; // Turn LED Off if no push button was pressed
        Idle(); // Turn CPU off, consumes little power   
                  
     }
    return 0;
}