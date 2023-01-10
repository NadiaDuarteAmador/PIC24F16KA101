
/* 
 * File:   main.c
 * Author: nadia, justin, shahdad
 *
 * Created on September 23, 2022, 4:42 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>
#define FCY  (4000000)// FCY = FOSC / 2. FOSC = 800 MHz. Declare for __delay_ms built in function. FCY = internal instruction cycle clock. FOSC = Oscillator frequency.
#include <libpic30.h> //Include for __delay_ms built in function.
#pragma config POSCMOD = NONE // Pimary oscillator disabled. Configure RA2 as IO.
#pragma config OSCIOFNC = ON // CLKO output disabled, pin functions as IO port.


/*
 * 
 */

void main(void) {
    
    AD1PCFG = 0XFFFF; // Sets all analog ports to digital IO. Sets RA2 as digital IO.
     
    // Configuring Push Buttons as Inputs (PB1, PB2, PB3).
    TRISAbits.TRISA2 = 1; // RA2 Port, PB1.
    TRISBbits.TRISB4 = 1; // RB4 Port, PB2.
    TRISAbits.TRISA4 = 1; // RA4 Port, PB3. 
    
    // Enabling Pull Up for Inputs (PB1, PB2, PB3).
    CNPU2bits.CN30PUE = 1; // RA2
    CNPU1bits.CN1PUE = 1;  // RB4
    CNPU1bits.CN0PUE = 1;  // RA4
    
    // Configuring LED Output.
    TRISBbits.TRISB8 = 0; //RB8 Port. 
    
    
    while (1)
    {
     
        
        uint8_t PBsPressed = PORTAbits.RA2 + PORTAbits.RA4 + PORTBbits.RB4; // Counts how many Push Buttons are being pressed. (0 = All, 1 = Two, 2 = One, 3 = None)
        
        if (PBsPressed == 3) // No Push Buttons are pressed
        {
            LATBbits.LATB8 = 0; // LED OFF
        }
        
        else if (PBsPressed <=1) // While at least 2 Push Buttons are pressed
        {
            LATBbits.LATB8 = 1; // LED ON without blinking          
        }
        
        else if (PORTAbits.RA2 == 0) // While PB1 is pressed
        {
            LATBbits.LATB8 = 1;  // LED ON
            __delay_ms(1000)     // 1s Delay  
            LATBbits.LATB8 = 0;  // LED OFF
            __delay_ms(1000)     // 1s Delay
        }
        
        else if (PORTBbits.RB4 == 0)  // While PB2 is pressed
        {             
            LATBbits.LATB8 = 1;  // LED ON
            __delay_ms(2000)     // 2s Delay
            LATBbits.LATB8 = 0;  // LED OFF
            __delay_ms(2000)     // 2s Delay                                
        }
            
        else if (PORTAbits.RA4 == 0) // While PB3 is pressed
        {
            LATBbits.LATB8 = 1;  // LED ON
            __delay_ms(3000)     // 3s Delay
            LATBbits.LATB8 = 0;  // LED OFF
            __delay_ms(3000)     // 3s Delay        
        }

                          
    
    }
    
    return;
}
