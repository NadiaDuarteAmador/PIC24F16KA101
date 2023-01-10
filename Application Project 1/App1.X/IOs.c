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

extern uint8_t CNflag; // Global variable CNflag in main.c
uint8_t pb_state = 0; // X is a counter for what buttons are being pressed. 0 inital state
uint8_t minutes = 0; // Minutes counter
uint8_t seconds = 0; // Seconds counter
uint8_t start_pause = 0; // Toggles for start/pause function in PB3
uint8_t alarm = 0; // Used for debugging the alarm message
uint8_t stop = 0; // Used for debugging debouncing effects from rising and falling edges of the interrupt.
//Stop debugs the extra printed second after stopping the timer (after printing 'Stop Timer' an extra second was printed due to interrrupt rising/falling edges)

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
    if (pb_state == 0) // None PBs are pressed
    {       
        LATBbits.LATB8 = 0; // LED Off
        Idle(); // Save power
    }
    
    else if (pb_state == 1) // PB1 pressed
    {
        minutes = minutes + 1; // Increment minutes
        if (minutes >= 60) // No more than 60 min 
        {
            minutes = 59; // Minutes can't go over 60, set back to 59
            pb_state = 0; // Back to initial state               
        }
        // Display minutes increment when pressing PB1 
        Disp2Dec(minutes);  // The following 4 lines will print "<minutes>m: <seconds>s", only minutes are incremented here
        Disp2String("m : ");
        Disp2Dec(seconds);
        Disp2String("s");
        XmitUART2('\r', 1); // next line
       
        if(PORTAbits.RA2 == 1) // PB1 no longer being pressed
        {
            pb_state = 0; // Back to initial state, Led Off and Idle
   
        }
    }
    
    else if (pb_state == 2) // PB2 pressed
    {
        seconds = seconds + 1; // Increment seconds
        if (seconds > 59) // No more than 59 seconds in one minute
        {
            seconds = 59; // Seconds can't go over 59, set back to 59
            pb_state = 0; // Back to initial state, Led Off and Idle
          
        }
        // Display seconds increment when pressing PB2
        Disp2Dec(minutes);  // The following 4 lines will print "<minutes>m: <seconds>s", only seconds are incremented here
        Disp2String("m : ");
        Disp2Dec(seconds);
        Disp2String("s");
        XmitUART2('\r', 1); // next line
        
        if(PORTBbits.RB4 == 1) // PB2 no longer being pressed
        {
            pb_state = 0; // Back to initial state, Led Off and Idle
           
        
        }
    }
    
    else if (pb_state == 4) // PB3 pressed for more than 3s
    {
        
       // Display reseted timer
        Disp2String("00m : 00s\r"); // Reset timer 
        seconds = 0; // Reset seconds
        minutes = 0; // Reset minutes
        start_pause = 0; // Initialize timer again (paused state)
        pb_state = 0; // Back to initial state, Led Off and Idle

        
    }
   
    
    else if (pb_state == 3) // PB3 pressed for less than 3s
    {

        if (start_pause == 1) // If start 
        {
            LATBbits.LATB8 = 1;  // LED On
            seconds = seconds - 1; // Decrement seconds in timer 
            // 1s delay blinking in LED
            LATBbits.LATB8 = 1;  // LED On
            delay_ms(1000); // Calling delay_ms function in TimeDelay.c with 1s
            LATBbits.LATB8 = 0;  // LED Off
            delay_ms(1000); // Calling delay_ms function in TimeDelay.c with 1s
     
            if(stop == 0){ // For debugging debouncing effects from rising and falling edges of the interrupt
                
  
                // Displaying Countdown
                XmitUART2('\r', 1); // next line
                Disp2Dec(minutes);  // The following 4 lines will print "<minutes>m: <seconds>s", both minutes and seconds will be printed with their corresponding decrement on the countdown
                Disp2String("m: ");   
                Disp2Dec(seconds);
                Disp2String("s");
                XmitUART2('\r', 1); // next line
            }
  
            
            
            if ((minutes != 0) && (seconds == 0)) // Re-start seconds for next minute
            {
                seconds = 60; // Begin second countdown in next minute
                minutes = minutes - 1; // Move on with next minute
            }
            
            if ((minutes == 0) && (seconds == 1)) // When countdown is complete 
            {
                alarm = 1; // Debugs alarm LED 
                Disp2String("\r00:00 -- ALARM\r"); // Display alarm
                start_pause = 0;
                pb_state = 0; //  Back to initial state, Led Off and Idle
                while(alarm == 1){ 
                   LATBbits.LATB8 = 1;  // LED stays On 
                }
             
            }
            
            if (start_pause == 0) // If paused
            {
                pb_state = 0; // Back to initial state, Led Off and Idle
               
            }
            
        }
    }
}
 return;
}  
        

        
    


// Change of Notification pin Interrupt subroutine
// When a button is pressed, cpu wakes up and comes here, sets flag to 1, goes back to line after idle in main
// To avoid interrupt nesting, clearing interrupts to avoid jumping between addresses and just reseting your code
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void)
{
  //  alarm = 0;  // To debug alarm message and LED On
 
    if(IFS1bits.CNIF ==1){  
        
        //Check the state of each button when the interrupt is triggered to filter debounce effect in buttons
         uint16_t seconds_counter = 0;

            if((PORTAbits.RA2 == 0) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 1)) // PB1 pressed
            {
                pb_state = 1; // Push button state is set to 1 (arbitrarily chosen)
            }
            else if ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 0) && (PORTAbits.RA4 == 1)) // PB2 pressed
            {
                pb_state = 2;
            }
            while ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 0)) // While PB3 pressed 
            {
              
                seconds_counter = seconds_counter + 1; // Count the seconds PB3 is being pressed
                
                XmitUART2('\r', 1);   // next line                    
                XmitUART2(' ', 10);   //spaces                   
                XmitUART2('\r', 1);   //next line                         
                
               if ((PORTAbits.RA4 == 1) && (seconds_counter >= 3)) // If PB3 is pressed for more than 3s
          
               {
                    stop = 1;  // For debugging interrupt rising/fall edges delay
                    pb_state = 4; 
                   
                } 
                
               else if ((PORTAbits.RA4 == 1) && (seconds_counter  < 3))  // If PB3 is pressed for less than 3s
                { 
 
                if (start_pause == 0) // Toggle each time PB3 is pressed for less than 3s (one for start, one for pause)
                {
                    stop = 0;  // For debugging interrupt rising/fall edges delay
                    start_pause = 1; 
                    
                    XmitUART2('\r', 1);   
                    Disp2String("Start timer"); // Displays alarm
                    XmitUART2('\r', 1); // next line
                    XmitUART2(' ', 10);  // spaces                         
                    XmitUART2('\r', 1); // next line
    
                    pb_state = 3; // PB3 pressed for less than 3s
  
                }
                else
                {
                    stop = 1;
                    seconds = seconds + 1; // Debugging delay effect with stop
                    start_pause = 0;

                    XmitUART2('\r', 1);
                    Disp2String("Stop timer"); // Displays alarm
                    XmitUART2('\r', 1); // next line
                    XmitUART2(' ', 10);    //spaces   
                    XmitUART2('\r', 1);  // next line

                    pb_state = 0;// Back to initial state after timer being paused

                }
                 
                }
  
                if (seconds == 0) // When seconds reach 0 re start count to 60 for the next min
                {
                   seconds = 60;
                }
                }
  
         
         }
    
    IFS1bits.CNIF = 0;  // Clear IF flag

}
