# Arduino-Hydroponic-Plant-Controller-GUI
Hydroponic Ebb &amp; Flow plant controller using a 128x64 OLED display, Arduino UNO, DS1307 RTC, and Rotary Encoder.

Controls Lighting and Watering for an indoor Ebb & Flow Hydroponic Garden.
  
#Plant Controller Features:

  * Arduino Based - Written for UNO, easily ported to Mega. NOT AVAILABLE FOR NANO
  * Rotary Encoder driven GUI
  * Interactive Menu System
  * Independent Manual ON, Off Auto settings for each Timer/Output
  * Real-Time Alert for Lights and Pump/Drain operation on Home Screen
  * Power Supply includes 
    * 1 = 110VAC output for Pump 
    * 1 = 110VAC output for Drain Valve (N/O operation)
    * 2 = 110VAC outputs for Lights
    
#Pump/Drain Timer Characteristics: 

  * User selectable 
  * Adjustable ON time up to 60 Minutes
  * Off time adjustable between hours and minutes
    * Off Hours adjustable up to 24
    * Off Minutes adjustable up to 60
  * Pump Timer executes "Auto ON" as soon as Auto is turned on (handy for priming system on first use)

#Light Timer Characteristics:

  * On Time can be set any time between 12:00 AM and 11:59 PM
  * Off Time can be set any time between 12:00 AM and 11:59 PM
  * Can control two 110VAC lights using included Power Supply schematic
   
