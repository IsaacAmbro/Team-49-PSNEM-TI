#include <msp430.h> // Include MSP430 header


// function prototypes
void setupUART(void);
void setupGPIO(void);
void sendUARTCommand(char command);
void sendUARTToMCU(char command);
void setupSPI(void);
void readSPIFromMain(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the watchdog timer

    setupGPIO();              // Configure GPIO pins for LEDs
    setupUART();              // Initialize UART communication
    setupSPI();               // Initialize SPI for communication with the main board
    PM5CTL0 &= ~LOCKLPM5;     // Disable GPIO high-impedance mode to enable I/O functionality


    // Main loop: simulates UART commands and reads SPI signals
       while (1) {
           sendUARTToMCU('1');   // Send command to turn on LED 1 on the main board
           readSPIFromMain();    // Read SPI signal from the main board
           __delay_cycles(500000); // Delay for visual distinction

           sendUARTToMCU('2');   // Send command to turn on LED 2
           readSPIFromMain();    // Read SPI signal from the main board
           __delay_cycles(500000); // Delay for visual distinction

           sendUARTToMCU('3');   // Send command to turn on LED 3
           readSPIFromMain();    // Read SPI signal from the main board
           __delay_cycles(500000); // Delay for visual distinction
       }
}



// Function to configure GPIO pins for LED output
void setupGPIO(void) {
    // Configure P1.0 (LED 1)
    P1DIR |= BIT0;     // Set P1.0 as an output
    P1SEL1 &= ~BIT0;   // Set P1.0 as a GPIO pin
    P1SEL0 &= ~BIT0;   // Set P1.0 as a GPIO pin
    P1OUT &= ~BIT0;    // Initialize LED 1 as OFF

    // Configure P4.6 (LED 2)
    P4DIR |= BIT6;     // Set P4.6 as an output
    P4SEL1 &= ~BIT6;   // Set P4.6 as a GPIO pin
    P4SEL0 &= ~BIT6;   // Set P4.6 as a GPIO pin
    P4OUT &= ~BIT6;    // Initialize LED 2 as OFF
}



// Function to configure UART for communication
void setupUART(void) {
    // UART A1 for sending commands to the main board
    UCA1CTLW0 |= UCSWRST;        // Put UART A1 into software reset
    UCA1CTLW0 |= UCSSEL__SMCLK;  // Select SMCLK as the clock source
    UCA1BRW = 8;                 // Set baud rate to 115200 (assuming 1 MHz clock)
    UCA1MCTLW |= 0xD600;         // Configure modulation for the baud rate

    // Configure UART A1 pins
    P2SEL1 |= BIT5;  // Configure P2.5 as TXD
    P2SEL0 &= ~BIT5;
    P2SEL1 |= BIT6;  // Configure P2.6 as RXD
    P2SEL0 &= ~BIT6;

    UCA1CTLW0 &= ~UCSWRST;       // Release UART A1 from reset

    // UART A0 for sending SPI received data to the terminal
    UCA0CTLW0 |= UCSWRST;        // Put UART A0 into software reset
    UCA0CTLW0 |= UCSSEL__SMCLK;  // Select SMCLK as the clock source
    UCA0BRW = 8;                 // Set baud rate to 115200
    UCA0MCTLW |= 0xD600;         // Configure modulation

    // Configure UART A0 pins
    P2SEL1 |= BIT0;  // Configure P2.0 as TXD
    P2SEL0 &= ~BIT0;

    UCA0CTLW0 &= ~UCSWRST;       // Release UART A0 from reset
}


// Function to send a command via UART A1 to the main board
void sendUARTToMCU(char command) {
    while (!(UCA1IFG & UCTXIFG)); // Wait until TX buffer is ready
    UCA1TXBUF = command;          // Transmit the command
}

// Function to send data via UART A0 to the terminal
void sendUARTCommand(char command) {
    while (!(UCA0IFG & UCTXIFG)); // Wait until TX buffer is ready
    UCA0TXBUF = command;          // Transmit the data
}


// Function to configure SPI communication
void setupSPI() {
    UCB0CTLW0 |= UCSWRST;       // Hold eUSCI_B0 in reset
    UCB0CTLW0 |= UCSYNC;        // Set SPI mode (synchronous)
    UCB0CTLW0 &= ~UCMST;        // Set as SPI slave
    UCB0CTLW0 |= UCMODE_2;      // 4-wire SPI with STE active low

    // Configure SPI pins
    P1SEL1 |= BIT3;  // P1.3 as STE
    P1SEL0 &= ~BIT3;

    P2SEL1 |= BIT2;  // P2.2 as CLK
    P2SEL0 &= ~BIT2;

    P1SEL1 |= BIT6;  // P1.6 as SIMO
    P1SEL0 &= ~BIT6;

    P1SEL1 |= BIT7;  // P1.7 as SOMI
    P1SEL0 &= ~BIT7;

    UCB0CTLW0 &= ~UCSWRST;      // Release SPI from reset
}


// Function to read SPI data from the main board
void readSPIFromMain(void) {
    while (!(UCB0IFG & UCRXIFG));      // Wait until SPI data is received
    char receivedData = UCB0RXBUF;    // Read the received data from the SPI RX buffer

    // Control LEDs based on received data
    if (receivedData == '1') {        // Turn off both LEDs
        P1OUT &= ~BIT0;
        P4OUT &= ~BIT6;
    } else if (receivedData == '2') { // Turn on LED 1 only
        P1OUT |= BIT0;
        P4OUT &= ~BIT6;
    } else if (receivedData == '3') { // Turn on LED 2 only
        P1OUT &= ~BIT0;
        P4OUT |= BIT6;
    } else if (receivedData == '4') { // Turn on both LEDs
        P1OUT |= BIT0;
        P4OUT |= BIT6;
    }

    // Send the received data to the terminal via UART A0
    while (!(UCA0IFG & UCTXIFG));     // Wait until TX buffer is ready
    sendUARTCommand(receivedData);    // Send the received data
}
