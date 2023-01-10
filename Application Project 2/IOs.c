/* 
 * File:   IOs.c
 * Author: nadia
 * DP 5
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

uint8_t pb_state = 0;
uint8_t minutes1 = 4;  // Minutes Player 1
uint8_t seconds1 = 0; // Seconds Player 1
uint8_t start_pause1 = 0;  // Start/Pause Player 1 timer
uint8_t minutes2 = 4; // Minutes Player 2
uint8_t seconds2 = 0; // Seconds Player 2
uint8_t start_pause2 = 0; // Start/Pause Player 2 timer

uint8_t LED_STAYS_ON = 0; // Used to debug LED
uint8_t stop = 0; // Used to debug pb1 and pb2 debouncing effects from rising and falling edges of the interrupt.
uint8_t stop_reset = 0; // Used to debug reset debouncing effects from rising and falling edges.


// IO INTERRUPTS 
void IOinit (void) {
    
    // LED
    TRISBbits.TRISB8 = 0; // Configure RB8 as digital output
    LATBbits.LATB8 = 0; // Make RB8 digital output
    // PUSH BUTTON 3
    TRISAbits.TRISA4 = 1; // Configure RA4 as digital input
    CNPU1bits.CN0PUE = 1; // Enable PU resistor on RA4/CN0
    CNEN1bits.CN0IE = 1; // Enables Change of Notification on CN0
    // PUSH BUTTON 2
    TRISBbits.TRISB4 = 1; // Configure RB4 as digital input
    CNPU1bits.CN1PUE = 1; // Enable PU resistor on RB4/CN1
    CNEN1bits.CN1IE = 1; // Enables Change of Notification on CN1
    // PUSH BUTTON 1
    TRISAbits.TRISA2 = 1; // Configure RA2 as digital input
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

while (1) 
{  
    if (pb_state == 0) // Initial state, displays initial message and saves power in idle
    {     
        XmitUART2('\r', 1); // next line
        XmitUART2(' ', 10);  // spaces              
        XmitUART2('\r', 1);   
        Disp2String("Blitz Chess Clock"); // Displays reset
        XmitUART2('\r', 1); // next line
        
        LATBbits.LATB8 = 0; // LED Off
        Idle(); // Save power
    }
    

    if (pb_state == 1) // PB1 pressed
    {   
        
        while (start_pause1 == 1) // If start
        { 
            
            seconds1 = seconds1 - 1; // Decrement seconds 
            // 1s delay blinking in LED
            LATBbits.LATB8 = 1;  // LED On
            timer3_delay(1000); // Timer 3 delay in ms
            LATBbits.LATB8 = 0;  // LED Off
            
            if ((stop == 0) && (stop_reset == 0)) { // for debugging rising and falling edges
                // Displaying Countdown
                XmitUART2('\r', 1); // next line
                Disp2Dec(minutes1);  // The following 4 lines will print "<minutes>m: <seconds>s", both minutes and seconds will be printed with their corresponding decrement on the countdown
                Disp2String("m: ");   
                Disp2Dec(seconds1);
                Disp2String("s");
                XmitUART2('\r', 1); // next line
            }
 
            if ((minutes1 != 0) && (seconds1 == 0) && (stop_reset == 0)) // Re-start seconds for next minute
                {
                seconds1 = 60; // Begin second countdown in next minute
                minutes1 = minutes1 - 1; // Move on with next minute
                }                                         
            
            if ((minutes1 == 0) && (seconds1 == 1)) // When countdown is complete 
                {
                LED_STAYS_ON = 1; // Debugs LED 
                Disp2String("\rPlayer 2 is the winner\r"); // Display winner by timeout
                
                while(LED_STAYS_ON == 1)
                   { 
                   LATBbits.LATB8 = 1;  // LED stays On 
                   }
                }
 
            
        if ((start_pause1 == 0) && (stop_reset == 0)) // adding 5s to player's 1 timer at player's 2 turn
        {
            if (seconds1 > 52) // if seconds>52, increment minute and accommodate seconds accordingly. 52+7=59 (minutes can't go over 59 seconds)
            {
                minutes1 = minutes1 + 1; 
                seconds1 = seconds1 - 3 - 50;
            }
            else
            {
                              
            seconds1 = seconds1 + 7; // incrementing 5s (7s were used to account for rising and falling edges delay)
            }
            
        }
        }
        }

                
    
    if (pb_state == 2) // PB2 pressed
    {
        
               
        while (start_pause2 == 1) // If start 
        {
          
            seconds2 = seconds2 - 1; // Decrement seconds in timer 
            // 1s delay blinking in LED
            LATBbits.LATB8 = 1;  // LED On
            timer2_delay(1000); // Timer 2 delay in ms
            LATBbits.LATB8 = 0;  // LED Off
            
            if ((stop == 1) && (stop_reset == 0)){
                // Displaying Countdown
                XmitUART2('\r', 1); // next line
                Disp2Dec(minutes2);  // The following 4 lines will print "<minutes>m: <seconds>s", both minutes and seconds will be printed with their corresponding decrement on the countdown
                Disp2String("m: ");   
                Disp2Dec(seconds2);
                Disp2String("s");
                XmitUART2('\r', 1); // next line
            }
            
            if ((minutes2 != 0) && (seconds2 == 0) && (stop_reset == 0)) // Re-start seconds for next minute
                {
                seconds2 = 60; // Begin second countdown in next minute
                minutes2 = minutes2 - 1; // Move on with next minute
                }
            
                
            if ((minutes2 == 0) && (seconds2 == 1)) // When countdown is complete 
                {
                LED_STAYS_ON = 1; // Debugs LED 
                Disp2String("\rPlayer 1 is the winner\r"); // Display winner (timeout)
                
                while(LED_STAYS_ON == 1)
                   { 
                   LATBbits.LATB8 = 1;  // LED stays On 
                   }
                }
            
            if ((start_pause2 == 0) && (stop_reset == 0)) // to debug for first time running (secs and min don't update until start_pause2 is updated at player's 1 second turn)
                 
            {
                if (seconds2 > 52) // if seconds>52, increment minute and accomodate seconds accordingly. 52+7=59 (minutes can't go over 59 seconds)
                {
                    minutes2 = minutes2 + 1; 
                    seconds2 = seconds2 - 3 - 50;
                }
                else
                {
                seconds2 = seconds2 + 7; // incrementing 5s (7s were used to account for rising and falling edges delay)
                }
       
            }

        }
    
    }
    

 return;
}
}

        

// Change of Notification pin Interrupt subroutine
// When a button is pressed, cpu wakes up and comes here, sets flag to 1, goes back to line after idle in main
// To avoid interrupt nesting, clearing interrupts to avoid jumping between addresses and just reseting your code
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void)
{
  //  alarm = 0;  // To debug alarm message and LED On
 
    if(IFS1bits.CNIF ==1){  
        
        //Check the state of each button when the interrupt is triggered
    
        if((PORTAbits.RA2 == 0) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 1)) // PB1 pressed, PLAYER 1 TURN
            {
                start_pause1 = 1; // Player 1 timer starts
                start_pause2 = 0; // Player 2 timer pauses
                stop = 0;
                stop_reset = 0;
          
                       
                   XmitUART2('\r', 1); // next line
                   XmitUART2(' ', 10);  // spaces 
                   XmitUART2('\r', 1);   
                   Disp2String("Player 1"); // Displays player's timer turn
                   XmitUART2('\r', 1);  
                   Disp2String("----------------"); // Displays player's timer turn
                   XmitUART2('\r', 1); // next line
                   
                pb_state = 1; 
               
                if (seconds1 == 0) // When seconds reach 0 re start count to 60 for the next min
                {
                   seconds1 = 60;
                }
            }
         
         

        if ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 0) && (PORTAbits.RA4 == 1)) // PB2 pressed, PLAYER 2 TURN
            {
                start_pause1 = 0; // Player 1 timer pauses
                start_pause2 = 1; // Player 2 timer starts
                stop = 1;
                stop_reset = 0;

                        
                   XmitUART2('\r', 1); // next line
                   XmitUART2(' ', 10);  // spaces 
                   XmitUART2('\r', 1);   
                   Disp2String("Player 2"); // Displays player's timer turn
                   XmitUART2('\r', 1);  
                   Disp2String("----------------"); // Displays player's timer turn
                   XmitUART2('\r', 1); // next line           
             
                pb_state = 2; 
           
                
                if (seconds2 == 0) // When seconds reach 0 re start count to 60 for the next min
                {
                   seconds2 = 60;
                }
            }
         
        
         if ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 0)) //  PB3 pressed, RESET
            {
             
               stop_reset = 1;     

               minutes1 = 4; // back to initial time (4:59s for each player)
               seconds1 = 0;
               start_pause1 = 0;
               minutes2 = 4;
               seconds2 = 0;
               start_pause2 = 0;
               pb_state = 0;
            }
        }
 
    IFS1bits.CNIF = 0;  // Clear IF flag

}
