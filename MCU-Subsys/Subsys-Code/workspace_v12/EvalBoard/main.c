#include <msp430.h> 

void initUART(void);

/**
 * main.c
 */
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    initUART();

    PM5CTL0 &= ~LOCKLPM5;
	
    while (1){
	    if (UCA1IFG & UCRXIFG) {  // Check if RX flag is set (data available)
	              unsigned char received = UCA1RXBUF; // Read received data from RX buffer
	              while (!(UCA0IFG & UCTXIFG)){}
	              UCA0TXBUF = received;
	          }
	}

	return 0;
}

void initUART(){
    //A0 for terminal
    //A1 for talk

    UCA1CTLW0 |= UCSWRST;  // put UART A1 into SW Reset
    UCA1CTLW0 |= UCSSEL__SMCLK; //Choose SMCLK for UART A0;

    UCA0CTLW0 |= UCSWRST;  // put UART A1 into SW Reset
    UCA0CTLW0 |= UCSSEL__SMCLK; //Choose SMCLK for UART A0;

    // Setup modulation
    // 1 MHz clock
    // 115200  baud rate
    UCA1BRW = 8;
    UCA1MCTLW |= 0xD600;

    UCA0BRW = 8;
    UCA0MCTLW |= 0xD600;

    // Setup Pins
    // TXD
    P2SEL1 |= BIT5;
    P2SEL0 &= ~BIT5;

    //RXD
    P2SEL1 |= BIT6;
    P2SEL0 &= ~BIT6;

    // TXD
    P2SEL1 |= BIT0;
    P2SEL0 &= ~BIT0;

    //RXD
    P2SEL1 |= BIT1;
    P2SEL0 &= ~BIT1;



    UCA1CTLW0 &= ~UCSWRST; //take UART A1 out of SW Reset
    UCA0CTLW0 &= ~UCSWRST; //take UART A1 out of SW Reset


}
