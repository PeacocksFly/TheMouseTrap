# TheMouseTrap

This project aims to build an electronic mouse trap. An infrared sensor will detect any mouse motion. A push-pull magnet will then actuate the door in case of detection. A GSM module will finally send out a text to inform of the presence of mouse.

The infrared sensor is the HC-SR501 PIR motion detector. It needs a minimum voltage of 5V. The main battery powering the whole system being a 9V battery, a buck converter LM2596 is used to step it down. In case of detected motion, the sensor sets its output pin high at 3.3V. This output pin is connected to the INT1 pin of the PIC18F45K22 microcontroller.  

The push-pull magnet needs a minimum voltage of 6V. Its resistance being 4 ohms, it draws a current of 1.5 A. The maximum output current of the microcontroller being 25 mA, an external switch is needed to turn on/off the push-pull. A darlington transistor TIP110 able to withstand a continuous current equal to 2A acts as the switch, with its base driven by the RD1 pin of the microcontroller. In order to obtain 6V at the terminals of the magnet, a 5k ohms is connected between the RD1 pin and the TIP110's base. A flyback diode is connected in parallel with the push-pull to take care of the voltage spikes arising when turning off the solenoid. A capacitor is connected between the collector and emitter of the TIP110 to help the battery as the latter cannot provide more than 300 mA approximatively.

The GSM module is the SIM800L. It ideally needs a 4V supply voltage so that another buck converter is used. It communicates
