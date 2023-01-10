/* 
 * File:   IOs.c
 * Author: nadia
 *
 * Created on October 4, 2022, 5:28 PM
 */

#include "xc.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <math.h>
#include <errno.h>
#include "IOs.h"

extern uint8_t CNflag; // Global variable CNflag in main.c


/*
 * 
 */

// IO INTERRUPTS 
void IOinit (void) {
    
    // LED
    TRISBbits.TRISB8 = 0; // Configure RB8 as digital output
    LATBbits.LATB8 = 0; // Make RB8 digital output
    // PUSH BUTTON 1
    TRISAbits.TRISA4 = 1; // Configure RA4 as digital output
    CNPU1bits.CN0PUE = 1; // Enable PU resistor on RA4/CN0
    CNEN1bits.CN0IE = 1; // Enables Change of Notification on CN0
    // PUSH BUTTON 2
    TRISBbits.TRISB4 = 1; // Configure RB4 as digital output
    CNPU1bits.CN1PUE = 1; // Enable PU resistor on RB4/CN1
    CNEN1bits.CN1IE = 1; // Enables Change of Notification on CN1
    // PUSH BUTTON 3
    TRISAbits.TRISA2 = 1; // Configure RA2 as digital output
    CNPU2bits.CN30PUE = 1; // Enable PU resistor on RA2/CN30
    CNEN2bits.CN30IE = 1; // Enables Change of Notification on CN30
    
    // CN Interrupt Settings
    IFS1bits.CNIF = 0; // Clear interrupt flag CN
    IPC4bits.CNIP = 6; // Priority levels (7 highest, 1 lowest, 0 disabled interrupt)
    IEC1bits.CNIE = 1; // Enable CN interrupts
    

    return;
}

void IOcheck(void)
{
    // IEC1bits.CNIE = 0; // disable CN interrupts to avoid debounces
    // delay_ms(400,1);   // 400 msec delay to filter out debounces
    // IEC1bits.CNIE = 1; // enable CN interrupts to detect pb release
while (1) 
{  
    
   uint8_t PBsPressed = PORTAbits.RA2 + PORTAbits.RA4 + PORTBbits.RB4; // Counts how many Push Buttons are being pressed. (0 = All, 1 = Two, 2 = One, 3 = None)
        
   LATBbits.LATB8 = 0;
        if (PBsPressed == 3) // No Push Buttons are pressed
        {
            LATBbits.LATB8 = 0; // LED Off
        }

        if (PBsPressed <= 1) // At least two PBs are pressed at the exact same time
        {
            LATBbits.LATB8 = 1; // LED On
            delay_ms(1); // Calling delay_ms function in TimeDelay.c with 1ms
            LATBbits.LATB8 = 0;  // LED Off
            delay_ms(1); 
        }
        else if (PORTAbits.RA2 == 0) // While PB1 is pressed
        {
            LATBbits.LATB8 = 1;  // LED On
            delay_ms(1000); // Calling delay_ms function in TimeDelay.c with 1s
            LATBbits.LATB8 = 0;  // LED Off
            delay_ms(1000); 
        }
        else  if (PORTBbits.RB4 == 0)
        {
            LATBbits.LATB8 = 1;  // LED ON
            delay_ms(2000); // Calling delay function in TimeDelay.c with 2s
            LATBbits.LATB8 = 0;  // LED Off
            delay_ms(2000); 
        }
        else   if (PORTAbits.RA4 == 0)
        {
            LATBbits.LATB8 = 1;  // LED On
            delay_ms(3000); // Calling delay_ms function in TimeDelay.c with 3s
            LATBbits.LATB8 = 0;  // LED Off 
            delay_ms(3000); 
        }
          
        } 
 return;
}  
        

        
    


// Change of Notification pin Interrupt subroutine
// When a button is pressed, cpu wakes up and comes here, sets flag to 1, goes back to line after idle in main
// To avoid interrupt nesting, clearing interrupts to avoid jumping between addresses and just reseting your code
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    
    CNflag = 1; // Global user defined flag - use only if needed
    IFS1bits.CNIF = 0;  // Clear IF flag
    Nop();
    
    return;
}