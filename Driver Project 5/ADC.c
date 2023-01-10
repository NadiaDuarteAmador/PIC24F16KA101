/*
 * File:   ADC.c
 * Author: user
 *
 * Created on November 5, 2022, 10:08 PM
 */

#include "xc.h"
#include "UART2.h"
#include "ChangeClk.h"
#include "ADC.h"


   uint16_t ADCvalue; // 16 bit register used to hold ADC converted digital output ADC1BUF0

uint16_t do_ADC(void) 
{    
 
    
  /* ------------- ADC INITIALIZATION ------------------*/
    
  /* Sample and conversion trigger configuration*/
    AD1CON1bits.ADON=1; // turn on ADC module
    // data output format bit - integer
    AD1CON1bits.FORM0=0;
    AD1CON1bits.FORM1=0; 
    // internal counter ends sampling and starts conversion (auto convert)
    AD1CON1bits.SSRC0=1;
    AD1CON1bits.SSRC1=1;
    AD1CON1bits.SSRC2=1;
    AD1CON1bits.ASAM=0; // sampling begins when SAMP bit is set
    AD1CON1bits.SAMP=0; //  A/D sample/hold amplifier is holding
   /* Vref select configuration*/
    AD1CON2bits.VCFG=0b000; // Vr+= AVdd, Vr-= AVss 
    AD1CON2bits.CSCNA=0; // do not scan inputs for MUX A input multiplexer setting bit
    // interrupts at the completion of conversion for each sample/convert sequence
    AD1CON2bits.SMPI0=0;
    AD1CON2bits.SMPI1=0;
    AD1CON2bits.SMPI2=0;
    AD1CON2bits.SMPI3=0;
    AD1CON2bits.BUFM=0;// buffer configured as one 16-word buffer
    AD1CON2bits.ALTS=0;// always uses MUX A input multiplexer settings  
   /* Sample time configuration*/
    AD1CON3bits.ADRC = 0; // use system clock
    // slowest sampling time = 5*2/fclk
    AD1CON3bits.SAMC0=1;
    AD1CON3bits.SAMC1=0; 
    AD1CON3bits.SAMC2=1; 
    AD1CON3bits.SAMC3=0; 
    AD1CON3bits.SAMC4=0; 
   /* input selection if MUX B used configuration*/
    AD1CHSbits.CH0NA=0;// channel 0 negative input is Vr-. Ensure sample time is 1/10th of signal being sampled
    //Channel 0 positive input is AN5
    AD1CHSbits.CH0SA0=1;
    AD1CHSbits.CH0SA1=0;
    AD1CHSbits.CH0SA2=1;
    AD1CHSbits.CH0SA3=0;
  /* input setup*/
    AD1PCFGbits.PCFG5=0; // ADC on AN5
  /* input scan setup*/
    AD1CSSLbits.CSSL5=0; // analog channel omitted from input scan

    
        /* ------------- ADC SAMPLING AND CONVERSION ------------------*/
    
    AD1CON1bits.SAMP=1; //Start Sampling, Conversion starts automatically after SSRC and SAMC settings
    while(AD1CON1bits.DONE==0)
    {}
    ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
    AD1CON1bits.SAMP=0; //Stop sampling
    AD1CON1bits.ADON=0; //Turn off ADC, ADC value stored in ADC1BUF0;
    
    return (ADCvalue); //returns 10 bit ADC output stored in ADC1BIF0 to calling function
}

    
   
void display_graph(uint16_t ADC_VALUE)
{

    NewClk(8); // Setting clock to 8Mhz
    
    // To draw the graph
    
    uint16_t length_max = 60; // Let max length of bar is 60

    uint16_t length_bar = ADC_VALUE * 60 / 1024; // Voltage level formula, scaler
    
    if (ADC_VALUE == 1024)
    {
        ADC_VALUE = 1024; // when bar length max is reached
    }
    
    XmitUART2('=', length_bar); // Width of bar changes in proportion to adc's digital output
    XmitUART2(' ', length_max - length_bar); // next line
    
    Disp2Hex(ADC_VALUE); // Display adc digital output value in HEX at the end of bar on pic terminal
    Disp2String("\r");  // jump line
    
    NewClk(32); // Set clk back to 32kHz
   
    
    return ADC_VALUE;
}