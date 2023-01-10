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
#include "UART2.h"
#include "TimeDelay.h"

extern uint8_t CNflag; // Global variable CNflag in main.c
uint8_t x = 4;

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
        
 //  LATBbits.LATB8 = 0;
        if (x == 4) // No Push Buttons are pressed
        {
            LATBbits.LATB8 = 0; // LED Off
            Disp2String("\n\rNothing pressed\n"); // Call function Disp2String in UART2.c
        }
        if (x == 3) // All PBs are pressed together
        {
            Disp2String("\n\rAll PBs pressed\n");
            LATBbits.LATB8 = 1; // LED On 
        }  
        else if (PBsPressed <= 1) // At least two PBs are pressed at the exact same time
        {
            if (x == 5) // If only PB1 and PB2 are pressed
            {
                Disp2String("\n\rPB1 and PB2 are pressed\n");
            }
            else if (x == 6) // If only PB1 and PB3 are pressed
            {
                Disp2String("\n\rPB1 and PB3 are pressed\n");
            }
            else if (x == 7) // If only PB2 and PB3 are pressed
            {
                Disp2String("\n\rPB2 and PB3 are pressed\n");
            }
            
            LATBbits.LATB8 = 1; // LED On
        }
        
        else if (x == 0) // While PB1 is pressed
        {
            Disp2String("\n\rPB1 is pressed\n");
            LATBbits.LATB8 = 1;  // LED On
            delay_ms(500); // Calling delay_ms function in TimeDelay.c with 0.5s
            LATBbits.LATB8 = 0;  // LED Off
            delay_ms(500); 
        }
        else  if (x == 1)
        {
            Disp2String("\n\rPB2 is pressed\n");
            LATBbits.LATB8 = 1;  // LED ON
            delay_ms(2000); // Calling delay function in TimeDelay.c with 2s
            LATBbits.LATB8 = 0;  // LED Off
            delay_ms(2000); 
        }
        else   if (x == 2)
        {
            Disp2String("\n\rPB3 is pressed\n");
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
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void)
{
    if(IFS1bits.CNIF ==1){  
        
        //Check the state of each button when the interrupt is triggered to filter debounce effect in buttons
       

            if((PORTAbits.RA2 == 0) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 1)) // PB1 pressed
            {
                x = 0; // Variable x gets assigned an arbitrarily value to recognize a specific state in IOCheck
            }
            else if ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 0) && (PORTAbits.RA4 == 1)) // PB2 pressed
            {
                x = 1;
            }
            else if ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 0)) // PB3 pressed
            {
                x = 2;
            }
            else if ((PORTAbits.RA2 == 0) && (PORTBbits.RB4 == 0) && (PORTAbits.RA4 == 0)) // All pressed
            {
                x = 3;
            }
            else if ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 1)) // None pressed
            {
                x = 4; // initial state
            }
            else if ((PORTAbits.RA2 == 0) && (PORTBbits.RB4 == 0) && (PORTAbits.RA4 == 1)) // PB1 and PB2 pressed
            {
                x = 5;
            }
            else if ((PORTAbits.RA2 == 0) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 0)) // PB1 and PB3 pressed
            {
                x = 6;
            }
            else if ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 0) && (PORTAbits.RA4 == 0)) // PB2 and PB3 pressed
            {
                x = 7;
            }
    }
    
    IFS1bits.CNIF = 0;  // Clear IF flag
}