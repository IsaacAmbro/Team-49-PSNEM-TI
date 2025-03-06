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
const unsigned int MAX_DAC_VALUE = 0xFFFF; // 16-bit max value
unsigned int dacValue = 0;                // Current DAC value in the ramp

int main(void) {

    WDTCTL = WDTPW + WDTHOLD; // Stop the watchdog timer

    setupGPIO();              // Configure GPIO pins for buttons and LEDs
    initGPIO();               // Initialize GPIO pin states
    PM5CTL0 &= ~LOCKLPM5;     // Disable the GPIO high-impedance mode to enable I/O

    __enable_interrupt();


    while(1){
        // Check each button and update ledState only if no other buttons are pressed
            if (!(P1IN & BIT1)) { // LDO button only
                ledState = 1; // Set state to LED1
            } else if (!(P1IN & BIT2)) { // Buck button only
                ledState = 2; // Set state to LED2
            } else if (!(P3IN & BIT0)) { // PS button only
                ledState = 3; // Set state to LED3
            }



            // Only update LEDs if the GO Button (P4.3) is pressed (active low)
            if (!(P4IN & BIT3)) {
                // Turn off all LEDs
                P3OUT &= ~BIT1;
                P3OUT &= ~BIT2;
                P3OUT &= ~BIT3;
                __delay_cycles(1000000); // Delay so no power supplies are connected at once

                // Turn on the correct LED based on ledState
                if (ledState == 1) {
                    P3OUT |= BIT1;
                }
                if (ledState == 2) {
                    P3OUT |= BIT2;
                }
                if (ledState == 3) {
                    P3OUT |= BIT3;
                }
                //__delay_cycles(100000);
                // Reconfigure DAC after changing power rail
                unsigned long prog = ((unsigned long)0x04 << 16) | ((unsigned long)0x101);
                //spiCommand(prog);
            }
            __delay_cycles(1000); // Debounce delay
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






