# The Mouse Trap

This project aims to build an electronic mouse trap. An infrared sensor will detect any mouse motion inside a box. A push-pull magnet will actuate the door in case of detection. A GSM module will then send out a text to inform of the presence of a mouse.

The infrared sensor is the HC-SR501 PIR motion detector. It needs a minimum voltage of 5V. The main battery powering the whole system being a 9V battery, a buck converter LM2596 is used to step it down. In case of detected motion, the sensor sets its output pin high at 3.3V. This output pin is connected to the INT1 pin of the PIC18F45K22 microcontroller.  

The push-pull magnet needs a minimum voltage of 6V. Its resistance being 4 ohms, it draws a current of 1.5 A. The maximum output current of the microcontroller being 25 mA, an external switch is needed to turn on/off the push-pull. A darlington transistor TIP110 able to withstand a continuous current equal to 2A acts as a switch, with its base driven by the RD1 pin of the microcontroller. In order to obtain 6V at the terminals of the magnet, a 5k ohms is connected between the RD1 pin and the TIP110 base. A flyback diode is connected in parallel with the push-pull to take care of the voltage spikes arising when turning off the solenoid. A capacitor is connected between the collector and the emitter of the TIP110 to help the battery as the latter cannot provide approximatively more than 300 mA.

The GSM module is the SIM800L. It ideally needs a 4V supply voltage so that another buck converter is used. The module uses a serial port to communicate with the microcontroller. Different supply voltage levels could have been used for the PIC microcontroller. However, using a 5V power requires to use a level shifter between the infrared output pin (at 3.3V) and the microcontroller since the I/O ports of the PIC18F45K22 are Schmitt triggers with minimum input high voltage of 0.8 * Vdd. The UART high voltage level of the GSM module being 2.8V, it easily communicates to a MCU powered at 3.3V with the help of a few resistors. The microcontroller is therefore powered at 3.3V so that a level shifter is no longer needed between the IR motion detector and the MCU. On the flip side, a third step down converter is used to provide the 3.3V.

When a motion is detected inside the box, the interrupt used on the infrared pin is disabled so that no additional movement from the same mouse would actuate the door and send useless messages. A push-button connected to the INT0 pin needs to be pressed to reset the trap. 

Video link: https://youtu.be/y9_TCBD_b2k
