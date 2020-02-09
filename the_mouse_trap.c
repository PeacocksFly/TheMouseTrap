/*
 * File:   first_run.c
 * Author: Quentin
 *
 * Created on 11 October 2019, 16:41
 */


// PIC18F45K22 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = HSMP      // Oscillator Selection bits (Internal oscillator block)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock is always enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<5:0> pins are configured as analog input channels on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTD2   // ECCP2 B output mux bit (P2B is on RD2)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <pic18f45k22.h>
#define _XTAL_FREQ 8000000

void __interrupt(high_priority) myIsrHigh(void);
void __interrupt(low_priority) myIsrLow(void);
void stringToSIM800L(void*);
void charToSIM800L(uint8_t);
uint8_t sendATCommand(void*, const uint8_t);
void delay(uint16_t);
uint8_t resultCodeAnalysis(uint8_t);

uint8_t RX_buffer[50] __at(0x050);
enum result_codes{OK, CONNECT, RING, NO_CARRIER, ERROR, NO_DIALTONE, BUSY, NO_ANSWER};
uint8_t* head = RX_buffer;
uint8_t* tail = &RX_buffer[49];
uint8_t* temp;

inline void ptrReset(void){temp = head;}

void main(void) {
    
    //Oscillator configuration
    OSCTUNEbits.PLLEN = 0;
    OSCCONbits.SCS = 0b00;
            
    //EUSART Reception Control Register Configuration
    TRISCbits.TRISC6 = 1;              //RC6 as an output - TX
    TRISCbits.TRISC7 = 1;              //RC7 as an input  - RX
    ANSELCbits.ANSC6 = 0;              //digital enabled
    ANSELCbits.ANSC7 = 0;              //digital enabled
    
    RCSTA1bits.SPEN = 1;               //serial port enabled
    RCSTA1bits.RX9 = 0;                //8-bits reception
    RCSTA1bits.CREN = 1;               //receiver enabled
       
    //EUSART Transmit Control Register Configuration
    TXSTA1bits.SYNC = 0;               //asynchronous mode
    TXSTA1bits.TXEN = 1;               //transmitter enabled
    TXSTA1bits.TX9 = 0;                //8-bits transmission
    TXSTA1bits.BRGH = 0;               //low speed baud rate selected 
    BAUDCON1bits.BRG16 = 0;            //8-bit baud rate generator
    SPBRG1 = 12;                       //baud rate = 9600
       
    //RB1 pin for InfraRed Sensor Configuration
    TRISDbits.RD1 = 0;                 //RD1 as output
    LATDbits.LATD1 = 0;                //RD1 at 0
    TRISBbits.RB1 = 1;                 //INT1 as input
    ANSELBbits.ANSB1 = 0;              //digital input enabled
    WPUBbits.WPUB1 = 0;                //weak pull-up on PORT B disabled
    
    //RB0 pin used with Push-Button to reset the push-pull magnet configuration
    TRISBbits.RB0 = 1;                 //INT0 as input
    ANSELBbits.ANSB0 = 0;              //digital input enabled
    WPUBbits.WPUB0 = 0;                //weak pull-up on PORT B disabled
       
    delay(60000);                      //delay for the infrared sensor to stabilize
    
    //Interrupts configuration
    INTCONbits.GIE = 1;                 //general interrupt enabled
    RCONbits.IPEN = 1;                  //priority levels enabled
    INTCONbits.PEIE = 1;                //peripheral interrupt enabled for UART  
    INTCON3bits.INT1IP = 0;             //INT1 interrupt low priority
    INTCON2bits.INTEDG1 = 1;            //Interrupt on rising edge enabled
    INTCON2bits.INTEDG0 = 1;            //Interrupt on rising edge enabled
    PIE1bits.RC1IE = 1;                 //RC1REG interrupt enabled
    IPR1bits.RC1IP = 1;                 //RC1 interrupt high priority
    PIR1bits.RC1IF = 0;                 //RC1 interrupt flag reset
    


    ptrReset();                         //reset the stop pointer to start of the uart buffer
    if(sendATCommand("ATV0\r", 100))    //AT command for less verbose answer
         {}     //LCD message           //default timeout for commands not interacting with the network

    ptrReset();                         
    if(sendATCommand("ATE0\r", 100))    //AT command for echo off + default timeout for commands not interacting with the network
         {}     //LCD message           //LCD message to inform user of error message (to be implemented)
    
    ptrReset();                         
    if(sendATCommand("AT+CMGF=1\r", 5)) //AT command for sms mode + AT+CMGF command timeout
         {}     //LCD message           //LCD message to inform user of error message (to be implemented)
              
    INTCON3bits.INT1IF = 0;             //INT1 reset flag
    INTCON3bits.INT1IE = 1;             //INT1 interrupt enabled
    INTCONbits.INT0IF = 0;              //INT0 reset flag
    INTCONbits.INT0IE = 1;              //INT0 interrupt enabled
    
    while(1){}
 
    return;
}


void __interrupt(high_priority) myIsrHigh(void)
{  
    if(PIR1bits.RC1IF)                  //highest interrupt on RX pin to allow to 
    {                                   //interrupt the low interrupt on pin T1 when sending AT commands
        *temp++ = RCREG1;               //store the received byte
        if(temp == tail)                //circular buffer
          temp = head;
    }

    if(INTCONbits.INT0IF)               //push-button that enables to manually reset the trap
    {                                   //after catching something
        INTCON3bits.INT1IF = 0;         
        INTCON3bits.INT1IE = 1;         //re-enable INT1 pin
        INTCONbits.INT0IF = 0;
    }
}


void __interrupt(low_priority) myIsrLow(void)
{
    if(INTCON3bits.INT1IF)              //INT1 pin sensing a signal coming from the infrared sensor
    {
        INTCON3bits.INT1IE = 0;         //interrupt disabled so that the caught animal does not continuously
                                        //trigger the door and send messages...a push-button enables to manually
                                        //re-trigger the system
        
        LATDbits.LATD1 = 1;             //trigger the door during 0.5s
        __delay_ms(500);
        LATDbits.LATD1 = 0; 
              
        ptrReset();                     //AT command to send a message + AT+CMGS command timeout
        if(sendATCommand("AT+CMGS=\"+352621981356\"\r", 180))  
            {}  //LCD message           //default timeout for commands not interacting with the network  
            
        ptrReset();                     //Message + AT+CMGS command timeout
        if(sendATCommand("Mouse caught!\x1A\r", 100)) 
            {}  //LCD message           //default timeout for commands not interacting with the network  
    }
        
}

uint8_t resultCodeAnalysis(uint8_t code)
{
    switch (code)
    {
        case OK:
        case CONNECT: 
        case RING:
        case NO_CARRIER: 
        case ERROR:
        case NO_DIALTONE: 
        case BUSY:
        case NO_ANSWER:
            return code;
            break;
        default: 
            return ERROR;
            break;            
    }
}

void stringToSIM800L(void* strg)
{
     while(*(uint8_t*)strg)    
         charToSIM800L(*(uint8_t*)strg++);
}

void charToSIM800L(uint8_t chtr)
{
         while(!PIR1bits.TX1IF);        
         TXREG1 = chtr;
}

uint8_t sendATCommand(void* cmd, uint8_t timeout)
{   
    stringToSIM800L(cmd);    
    delay(timeout);
    return resultCodeAnalysis(*(temp-4) & 0x01);
}

void delay(uint16_t ms)
{
    for(uint16_t i = 0; i< ms; i++)
        __delay_ms(1);
}

