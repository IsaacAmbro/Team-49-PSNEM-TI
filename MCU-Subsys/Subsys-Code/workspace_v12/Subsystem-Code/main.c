#include <msp430.h> // Include the MSP430 header file

// function declarations
void setupGPIO(void);
void initGPIO(void);
void initUART(void);
void initSPI(void);
void checkMode(void);
void remoteMode(void);
void manualMode(void);
void spiCommand(unsigned long command);
void updateRampSignal(void);

// global variables
volatile int mode = 0;      // 0 = Manual, 1 = Remote
volatile int ledState = 0;  // Which LED is selected
volatile int goCommand = 0; // Flag for "go" command in remote mode

// Constants for ramp generation
const unsigned int MAX_DAC_VALUE = 0xCCC; // 16-bit max value
unsigned int dacValue = 0;                // Current DAC value in the ramp

int main(void) {

    WDTCTL = WDTPW + WDTHOLD; // Stop the watchdog timer

    setupGPIO();              // Configure GPIO pins for buttons and LEDs
    initGPIO();               // Initialize GPIO pin states
    initUART();               // Initialize UART for communication
    initSPI();                // Initialize SPI for communication with DAC
    PM5CTL0 &= ~LOCKLPM5;     // Disable the GPIO high-impedance mode to enable I/O

    __enable_interrupt();

    // Infinite loop to handle system operations
    while (1) {
       checkMode(); // Check the mode switch and update the mode variable
       if (mode == 0) {
           manualMode(); // Execute manual mode logic
       } else {
           remoteMode(); // Execute remote mode logic
       }
       // Optionally update the ramp signal if needed:
       updateRampSignal();
   }
}

//-- Setup Functions

// Set pins/directions
void setupGPIO(){
    // buttons as inputs
    // LDO Button P1.1
    P1DIR &= ~BIT1; // set bit 1 to input
    P1SEL1 &= ~BIT1;
    P1SEL0 &= ~BIT1;

    // Buck Button P1.2
    P1DIR &= ~BIT2; // set bit 2 to input
    P1SEL1 &= ~BIT2;
    P1SEL0 &= ~BIT2;

    // PS Button P3.0
    P3DIR &= ~BIT0; // set bit 0 to input
    P3SEL1 &= ~BIT0;
    P3SEL0 &= ~BIT0;

    // Mode Switch P3.7
    P3DIR &= ~BIT7; // set bit 7 to input
    P3SEL1 &= ~BIT7;
    P3SEL0 &= ~BIT7;

    // GO Button P4.3 (for manual mode)
    P4DIR &= ~BIT3; // set bit 3 to input
    P4SEL1 &= ~BIT3;
    P4SEL0 &= ~BIT3;

    // LEDs as outputs
    // LDO LED on P3.1
    P3DIR |= BIT1; // set bit 1 to output
    P3SEL1 &= ~BIT1;
    P3SEL0 &= ~BIT1;

    // Buck LED on P3.2
    P3DIR |= BIT2; // set bit 2 to output
    P3SEL1 &= ~BIT2;
    P3SEL0 &= ~BIT2;

    // PS LED on P3.3
    P3DIR |= BIT3; // set bit 3 to output
    P3SEL1 &= ~BIT3;
    P3SEL0 &= ~BIT3;
}

//-- Initialization Functions

// Enable interrupts and establish starting values
void initGPIO() {
    // Initialize LED states (assume LDO LED on initially)
    P3OUT &= ~BIT1; // Turn off LDO LED
    P3OUT &= ~BIT2; // Turn off Buck LED
    P3OUT &= ~BIT3; // Turn off PS LED
}

void initUART(){
    UCA1CTLW0 |= UCSWRST;             // Put UART A1 into SW Reset
    UCA1CTLW0 |= UCSSEL__SMCLK;        // Choose SMCLK for UART A1

    // Setup modulation
    // 1 MHz clock, 115200 baud rate
    UCA1BRW = 8;
    UCA1MCTLW |= 0xD600;

    // Setup Pins for TXD and RXD
    P2SEL1 |= BIT5;   // TXD
    P2SEL0 &= ~BIT5;
    P2SEL1 |= BIT6;   // RXD
    P2SEL0 &= ~BIT6;

    UCA1CTLW0 &= ~UCSWRST; // Release UART A1 from SW Reset

    UCA1IE |= UCRXIE; // Enable RX interrupt
}

void initSPI(){
    UCA0CTLW0 |= UCSWRST;       // Put SPI module into software reset

    UCA0CTLW0 |= UCSSEL__SMCLK;  // Select SMCLK as the clock source
    UCA0CTLW0 |= UCSYNC;         // Enable synchronous mode
    UCA0CTLW0 |= UCMST;          // Set as SPI master
    UCA0CTLW0 |= UCSTEM;         // Enable STE pin
    UCA0CTLW0 &= ~UCMODE1;       // 4-wire SPI with STE active low
    UCA0CTLW0 &= ~UCMODE0;
    UCA0CTLW0 &= ~UCCKPH;
    UCA0CTLW0 |= UCMSB;          // MSB First

    // Setup SPI Pins:
    // P1.4 (STE)
    P1SEL1 &= ~BIT4;
    P1SEL0 &= ~BIT4;
    P1DIR |= BIT4;
    P1OUT |= BIT4;

    // P1.5 (CLK)
    P1SEL1 |= BIT5;
    P1SEL0 &= ~BIT5;

    // P2.0 (SIMO)
    P2SEL1 |= BIT0;
    P2SEL0 &= ~BIT0;

    // P2.1 (SOMI)
    P2SEL1 |= BIT1;
    P2SEL0 &= ~BIT1;

    UCA0CTLW0 &= ~UCSWRST; // Release SPI module from reset
}

// Helper Functions

// Check and update the current mode (manual or remote)
void checkMode(){
     if (!(P3IN & BIT7)) { // If Mode Switch (P3.7) is set to remote side
        mode = 1;   // Set mode to remote
    } else {
        mode = 0;   // Set mode to manual
    }
}

void manualMode() {
    // Check each button and update ledState only if no other buttons are pressed
    if (!(P1IN & BIT1) && (P1IN & BIT2) && (P3IN & BIT0) && (P4IN & BIT3)) { // LDO button only
        ledState = 1; // Set state to LED1
    } else if (!(P1IN & BIT2) && (P1IN & BIT1) && (P3IN & BIT0) && (P4IN & BIT3)) { // Buck button only
        ledState = 2; // Set state to LED2
    } else if (!(P3IN & BIT0) && (P1IN & BIT1) && (P1IN & BIT2) && (P4IN & BIT3)) { // PS button only
        ledState = 3; // Set state to LED3
    }

    // Only update LEDs if the GO Button (P4.3) is pressed (active low)
    if (!(P4IN & BIT3) && (P1IN & BIT1) && (P1IN & BIT2) && (P3IN & BIT0)) {
        // Turn off all LEDs
        P3OUT &= ~(BIT1 | BIT2 | BIT3);
        __delay_cycles(1000000); // Delay so no power supplies are connected at once

        // Turn on the correct LED based on ledState
        if (ledState == 1) {
            P3OUT |= BIT1;
        } else if (ledState == 2) {
            P3OUT |= BIT2;
        } else if (ledState == 3) {
            P3OUT |= BIT3;
        }
        __delay_cycles(100000);
        // Reconfigure DAC after changing power rail
        unsigned long prog = ((unsigned long)0x04 << 16) | ((unsigned long)0x101);
        spiCommand(prog);
    }
    __delay_cycles(100); // Debounce delay
}

void remoteMode(){
    if (goCommand) {
        // Turn off all LEDs
        // Turn off all LEDs
        P3OUT &= ~(BIT1 | BIT2 | BIT3);
        __delay_cycles(1000000); // Delay so no power supplies are connected at once

        // Turn on the correct LED based on ledState
        if (ledState == 1) {
            P3OUT |= BIT1;
        } else if (ledState == 2) {
            P3OUT |= BIT2;
        } else if (ledState == 3) {
            P3OUT |= BIT3;
        }
        __delay_cycles(10000);
        // Reconfigure DAC after changing power rail
        unsigned long prog = ((unsigned long)0x04 << 16) | ((unsigned long)0x101);
        spiCommand(prog);

        // Clear the go command flag after processing
        goCommand = 0;
    }
    __delay_cycles(100);
}

void spiCommand(unsigned long command) {
    int i;
    P1OUT &= ~BIT4; // Assert STE (active low)
    // Transmit the 24-bit command to the DAC
    for (i = 2; i >= 0; i--) {
        UCA0TXBUF = (command >> (i * 8)) & 0xFF;
    }
    P1OUT |= BIT4; // Deassert STE
}

void updateRampSignal() {
    dacValue += 1;  // Increment DAC output value to create a ramp signal

    // Wrap around if exceeding max value
    if (dacValue > MAX_DAC_VALUE) {
        dacValue = 0;  // Reset to 0 when the maximum DAC value is exceeded
    }

    // - Bits [23:16]: Command (0x08 for DAC update)
    // - Bits [15:4]:  12-bit DAC value shifted left by 4 bits
    unsigned long word = ((unsigned long)0x08 << 16) | ((unsigned long)(dacValue << 4));

    spiCommand(word);  // Send the command to the DAC via SPI
}


//-- ISRs
// UART RX Interrupt Service Routine
#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
    unsigned char received;
    // Process UART commands only in remote mode
    if (mode == 1) {
        switch (__even_in_range(UCA1IV, 4)) {
            case 0: break; // No interrupt
            case 2: // RXIFG (data received)
                  received = UCA1RXBUF; // Read the received byte

                  // Interpret received data to control LED selection
                  if (received == '1') {
                      ledState = 1;
                  } else if (received == '2') {
                      ledState = 2;
                  } else if (received == '3') {
                      ledState = 3;
                  } else if (received == '4') {
                      // Set flag to trigger LED update and DAC configuration
                      goCommand = 1;
                  }
                break;
            case 4: break; // TXIFG
            default: break;
        }
    } else {
        // If not in remote mode, discard the received data
        UCA1RXBUF;
    }
}

