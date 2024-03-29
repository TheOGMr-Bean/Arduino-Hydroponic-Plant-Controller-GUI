# Arduino-Hydroponic-Plant-Controller-GUI
<img align="center" src="main.jpg" />

## Hydroponic Multi-Purpose plant controller using a 128x64 OLED display, Arduino UNO, DS1307 RTC, and Rotary Encoder.

## Controls Lighting and Watering for an indoor Hydroponic Garden.

###Plant Controller Features:

  * For use with either Ebb &amp; Flow systems, Bubbler (DWC) systems, NFT systems, Wicking systems, and Misting systems (aeroponics)
  * Arduino Based - Written for UNO, easily ported to Mega. NOT AVAILABLE FOR NANO
  * Rotary Encoder driven GUI
  * Interactive Menu System
  * Independent Manual ON, Off Auto settings for each Timer/Output
  * Real-Time Alert for Lights and Pump/Drain operation on Home Screen
  * Power Supply Switching Schematic includes: 
    * 2 = 110VAC outputs for Lights
    * FOR EBB & FLOW SYSTEM:
      * 1 = 110VAC Output for Pump 
      * 1 = 110VAC Output for Drain Valve (N/O operation)
    * FOR ALL OTHER SYSTEMS:
      * 2 = 110VAC Outputs for Aeration/H2O Pumps
    
    
###Pump/Drain Timer Characteristics: 

  * User selectable 
  * Adjustable ON time up to 60 Minutes
  * Off time adjustable between hours and minutes
    * Off Hours adjustable up to 24
    * Off Minutes adjustable up to 60
  * Pump Timer executes "Auto ON" as soon as Auto is turned on (handy for priming system on first use)

###Light Timer Characteristics:

  * On Time can be set any time between 12:00 AM and 11:59 PM
  * Off Time can be set any time between 12:00 AM and 11:59 PM
  * Controls two 110VAC Light Outputs using included Power Supply Switching Schematic

###Download Plant_Controller_v2_2.ZIP for Source Code and Technical Drawings. 

###Schematic is written to provide 120v to all devices, schematic/code will need to be adjusted to use devices of different voltage or PWM controlled devices. PWM and variable voltage control will be available in the next version. 

###Schematic:
<img align="left" src="Plant Controller Schematic.bmp" />
