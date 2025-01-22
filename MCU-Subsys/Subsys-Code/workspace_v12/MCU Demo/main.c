#include <msp430.h> // Include the MSP430 header file

void initUART(void);         // Forward declaration of initUART
void blinkLEDs(void);        // Forward declaration of blinkLEDs

int main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop the watchdog timer


    UCA0CTLW0 |= UCSWRST;  // put UART A0 into SW Reset
    UCA0CTLW0 |= UCSSEL__SMCLK; //Choose SMCLK for UART A0;

    // Setup modulation
    UCA0BRW = 8;
    UCA0MCTLW |= 0xD600;

    // Setup Pins
    P2SEL1 |= BIT0;
    P2SEL0 &= ~BIT0;

    PM5CTL0 &= ~LOCKLPM5;
    UCA0CTLW0 &= ~UCSWRST; //take UART A0 out of SW Reset
    char message = '1';


    //Send message through UART TXD pin character at a time
    int pos;
    int j = 0;
    int k;
    while (j < 10){
        for (pos = 0; pos < sizeof(message); pos++){
                UCA0TXBUF = message;
                for (k = 0; k < 3000; k++){}
            }
        __delay_cycles(20000);
        j ++;
    }


    blinkLEDs(); // Blink the LEDs

}

// Function to blink LEDs
void blinkLEDs(void) {
    P1DIR |= 0x01;                          // Set P1.0 to output direction

    for(;;) {
        volatile unsigned int i;            // volatile to prevent optimization

        P1OUT ^= 0x01;                      // Toggle P1.0 using exclusive-OR

        i = 100000;                          // SW Delay
        do i--;
        while(i != 0);
    }
}
