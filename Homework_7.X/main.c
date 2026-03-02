/*
 * File:   main.c
 * Author: skyler
 *
 * Created on March 1, 2026, 3:50 PM
 */


#include <xc.h>

#pragma config OSC = IRCIO
#pragma config MCLRE = ON
#pragma config LVP = OFF
#pragma config WDTEN = OFF

#define _XTAL_FREQ 4000000

void init_eusart() {
    // 1. Set Pin Directions (RC6 = TX, RC7 = RX)
    TRISCbits.TRISC6 = 0; // TX output
    TRISCbits.TRISC7 = 1; // RX input

    // 2. Configure Baud Rate
    TXSTAbits.SYNC = 0;    // Asynchronous mode
    TXSTAbits.BRGH = 0;    // Low speed
    BAUDCONbits.BRG16 = 0; // 8-bit Baud rate generator
    SPBRG = 12;            // 12 for 9600 baud @ 8MHz

    // 3. Enable Serial Features
    TXSTAbits.TXEN = 1;    // Enable transmitter
    RCSTAbits.CREN = 1;    // Enable receiver
    RCSTAbits.SPEN = 1;    // Enable Serial Port
    
    // 4. Enable Receive Interrupts
    PIE1bits.RCIE = 1;     // Interrupt when data arrives
    INTCONbits.PEIE = 1;   // Peripheral Interrupt Enable
}

void init_external_interrupt(){
    //Enable external interrupts on INT0
    INTCONbits.PEIE = 1;
    INTCONbits.INT0IE = 1;
    INTCON2bits.INTEDG0 = 0;
    
    //disable priority interrupts
    RCONbits.IPEN = 0;
    
    //Enable all interrupts after configuration of INT0
    INTCONbits.GIE = 1;
}

void __interrupt(low_priority) other_pic_isr(){
    // Sender
    if (INTCONbits.INT0IF) {
        TXREG = 'X';         // Send toggle command to the other PIC
        INTCONbits.INT0IF = 0;
    }

    // Receiver
    if (PIR1bits.RCIF) {
        char receive = RCREG;  // Read the byte (clears the interrupt)
        if (receive == 'X') {
            PORTDbits.RD3 ^= 1; // Toggle local LED
        }
    }
}

//Green will indicate the 'heartbeat' for each PIC, blue the interrupt from the other 
//Green will be RD3 Blue RD2
void main(void) {
    //Set RD2-3 to output and to logic low
    
    TRISDbits.RD2 = 0;
    TRISDbits.RD3 = 0;
    
    PORTDbits.RD2 = 0;
    PORTDbits.RD3 = 0;
    
    init_eusart();
    init_external_interrupt();
    
    while(1){
        //Heartbeat on RD2
        __delay_ms(3);
        PORTDbits.RD2 = 1;
        
        __delay_ms(3);
        PORTDbits.RD2 = 0;
    }
    return;
}