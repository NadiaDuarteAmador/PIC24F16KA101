# -*- coding: utf-8 -*-
"""
DRIVER PROJECT 6
Receives timestamps, ADCBUF, voltage
Saves in Dataframe
Saves in csv
Plots received data versus time
@author: Justin Kuhn
"""

import numpy as np
import math 
import csv
import serial  # pip install pyserial  or conda install pyserial
import time
import pandas as pd
import plotly.express as px

## IMPORTS BELOW ONLY NEEDED FOR SPYDER IDE
import plotly.io as pio  # needed to plot plotly graphs in spyder
# pio.renderers.default = 'svg'  # to plot plotly graphs in spyder
pio.renderers.default = 'browser' # to plot plotly graphs in browser


# OPEN SERIAL PORT
ser = serial.Serial(port= "COM3", baudrate = 9600, bytesize = 8, timeout =2, stopbits = serial.STOPBITS_ONE)


## INITIALIZATIONS
ADCNumsStr = ''      #string to store received ADC uint16_t numbers 
ADCNumsList = []      #List to store received ADC uint16_t numbers in int form 
voltageNumsStr = ''      #string to store received voltage float numbers 
voltageNumsList = []      #List to store received voltage float numbers in float form 
rxTimesList = []   #list to store time stamps of received uint16_t numbers
startTime = time.time()   

## UART DATA CAPTURE OVER TIME INTERVAL
timeInterval = 10 # 10 second interval is required
while(time.time() - startTime < timeInterval):  # record data for 10 sec
    line =ser.readline() # reads uint16_t nums as single bytes till \n n stores in string
    if ((line != b' \n') and (line != b'\n')) : #removes and '\n' without num captures
        superString = line.decode('Ascii') # Converts a received UART line into a single string
        superString = superString.strip() # Remove excess whitespace from the string
        arr = superString.split(' ') # Create a list where arr[0] = adcbuf, arr[1] = voltage
        ADCNumsStr = ADCNumsStr + arr[0] + ' ' # Converts string of received uint16_t num to ASCII and combines ADC nums into 1 string
        voltageNumsStr = voltageNumsStr + arr[1] + ' ' # Converts string of received float num to ASCII and combines voltage nums into 1 string
        timeMeas = time.time() -startTime # Time stamp received number
        rxTimesList.append(timeMeas) #save time stamps in a list
        
## CLOSE SERIAL PORT
ser.close()  # close any open serial ports


### Converting the String of ADC values into a list of ints
ADCNumsStr = ADCNumsStr.strip() # remove unwanted chars and spaces 
ADCNumsList = ADCNumsStr.split(' ')  # split string by ' ' and store in list
ADCNumsList = [int(elem) for elem in ADCNumsList]  # convert char in List into int

### Converting the String of voltage values into a list of floats
voltageNumsStr = voltageNumsStr.strip() # remove unwanted chars and spaces 
voltageNumsList = voltageNumsStr.split(' ')  # split string by ' ' and store in list
voltageNumsList = [float(elem.rstrip('\x00')) for elem in voltageNumsList]  # convert char in List into int


### CONVERT DATA INTO DATA FRAME
dF = pd.DataFrame()
dF['Time (sec)'] = rxTimesList
dF['ADCBUF'] = ADCNumsList
dF['Voltage (volts)'] = voltageNumsList


### DATA STATISTICS
dF.describe()


### COPY VOLTAGE, ADC DATA AND TIME INTO A CSV FILE
dF.to_csv('DP-6.csv', index = True)


### PLOT ADC DATA VS TIME
fig1 = px.line(dF, x='Time (sec)', y='ADCBUF', title = 'Digital ADC Value vs Time')
fig1.show()

### PLOT Rx DATA VS Rx TIME
fig2 = px.line(dF, x='Time (sec)', y='Voltage (volts)', title = 'Analogue Voltage vs Time')
fig2.show() 
