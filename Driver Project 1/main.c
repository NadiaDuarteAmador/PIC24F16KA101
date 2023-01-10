/* 
 * File:   main.c
 * Author: nadia
 *
 * Created on September 11, 2022, 2:45 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

/*
 *  UCID ID: 30097519
 * 
 * Inputs are RA0, RA1, RA4, RA5. Where RA0 is the port for the LSB and RA5 is the port for the MSB. 
 * PORTA Reads the Input (4 bits Input used for condition statements in stimulus). 
 * The input condition statements were picked arbitrarily for simplicity when using the stimulus. 
 * 
 * B Outputs determine which and how many LEDS will be turned on. 
 * LATB Writes the Output (9 bits Output for 9 LEDS representing integers 1-9). 
 * 
 * 
 */


int main(void) {
    
    TRISAbits.TRISA0 = 1;   //Sets RA0 port as Input, PIN 2
    TRISAbits.TRISA1 = 1;   //Sets RA1 port as Input, PIN 3
    TRISAbits.TRISA4 = 1;   //Sets RA4 port as Input, PIN 10
    TRISAbits.TRISA5 = 1;   //Sets RA5 port as Input, PIN 1

    TRISBbits.TRISB0 = 0;   //Sets RB0 port as Output, PIN 4
    TRISBbits.TRISB1 = 0;   //Sets RB1 port as Output, PIN 5
    TRISBbits.TRISB2 = 0;   //Sets RB2 port as Output, PIN 6
    TRISBbits.TRISB4 = 0;   //Sets RB4 port as Output, PIN 9
    TRISBbits.TRISB7 = 0;   //Sets RB7 port as Output, PIN 11
    TRISBbits.TRISB8 = 0;   //Sets RB8 port as Output, PIN 12
    TRISBbits.TRISB9 = 0;   //Sets RB9 port as Output, PIN 13
    TRISBbits.TRISB12 = 0;  //Sets RB12 port as Output, PIN 15
    TRISBbits.TRISB13 = 0;  //Sets RB13 port as Output, PIN 16
    
// NOTE: The hex been written to the outputs in LATB, are shown in binary (16 Bits) to visualize the place where the 1's were placed. Each 1 represents a LED ON, 
//       but they were only placed on the index of their corresponding port outputs to avoid disturbing the other ports bit not being used. E.g. Number "5" in UCID ID
//       has an hex value of 0x97 -> 0000 0000 1001 0111 (Line 69), where there's five 1's (each for one LED) but these 1's are placed only on the indices of RB0, RB1, RB2, RB4 and RB7.
    
    while(1) 
    {
  
        if ((PORTAbits.RA0 == 1) && (PORTAbits.RA1 == 1))// Number "3". If (RA0, RA1 = 1), turn on 3 LEDS (RB0, RB1, RB2 = 0).
        {
            LATB = 0x7; // Binary: 0000 0000 0000 0111.
        }
        else if ((PORTAbits.RA0 == 1) && (PORTAbits.RA4 == 1)) // Number "0". If (RA0, RA4 = 1), turn on 0 lEDS.
        {
            LATB = 0x0; // Binary: 0000 0000 0000 0000.
        }
        else if ((PORTAbits.RA0 == 1) && (PORTAbits.RA5 == 1)) // Number "9". If (RA0, RA5 = 1), turn on 9 LEDS (RB0, RB1, RB2, RB4, RB7, RB8, RB9, RB12, RB13 = 1).
        {
            LATB = 0x3397; // Binary: 0011 0011 1001 0111. I had to consider 16 bits, to not disturb the others ports bit. 
                          // There are 9 1's in the binary number but they only correspond to the indices of the inputs being used.
          
        }
        else if ((PORTAbits.RA1 == 1) && (PORTAbits.RA4 == 1)) // Number "7". If (RA1, RA4 = 1), turn on 7 LEDS (RB0, RB1, RB2, RB4, RB7, RB8, RB9 = 1).
        {
            LATB = 0x397; // Binary: 0000 0011 1001 0111. 
        }
        else if ((PORTAbits.RA1 == 1) && (PORTAbits.RA5)) // Number "5". If (RA1, RA5 = 1), turn on 5 LEDS (RB0, RB1, RB2, RB4, RB7 = 1).
        {
            LATB = 0x97; // Binary: 0000 0000 1001 0111.
        }
        else if (PORTAbits.RA5 == 1) // Number "1". If (RA5 = 1), turn on 1 LED (RB0 = 1).
        {
            LATB = 0x1; // Binary: 0000 0000 0000 0001. 
        }
        else // Display nothing
        {
            LATB = 0x0; // 000000000.
        }
    }

    return;
}

