# PIDMotorControl
 PID Motor Control Project in an Introductory Mechatronics Course at Northwestern University
 
 A PIC32 microcontroller, Programmed in C using Visual Studio Editor, communicates with a motor encoder, H-bridge, current sensor and MATLAB as a client to direct a motor to go to certain paths along its trajectory. 
 
 In this Repository you will find `encoder.h, encoder.c, main.c` and varius MATLAB client programs. `encoder.c/.h` initialize/create functions to read the count in angles and position from the motor encoder as well as to reset the motor encoder to its initial count state. `main.c` implements code to communicate with the MATLAB client depending on keyboard input from the user to perform various tasks such as performing a PID current tracking test with the current sensor, read the encoder counts (ADC), reset the encoder counts, quit out of the program, read the PWM, telling the motor to stay at a certain position, setting current gains and position gains, and creating a trajectory for the motor.
