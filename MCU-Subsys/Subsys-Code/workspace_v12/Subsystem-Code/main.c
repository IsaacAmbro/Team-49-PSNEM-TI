#include <msp430.h> // Include the MSP430 header file


// function declarations
void setupGPIO(void);
void initGPIO(void);
void initUART(void);
void initSPI(void);
void checkMode(void);
void remoteMode(void);
void manualMode(void);
void spi_send_data_with_command(unsigned int command, unsigned int data);
void updateRampSignal(void);


// global variables
volatile int mode = 0; // 0 = Manual, 1 = Remote
volatile int ledState = 1; // Which LED is on
// Constants for ramp generation
const unsigned int MAX_DAC_VALUE = 0xFFF; // 12-bit max value (4095 for DAC80501)
unsigned int dacValue = 0;                // Current DAC value in the ramp


int main(void) {

    WDTCTL = WDTPW + WDTHOLD; // Stop the watchdog timer

    setupGPIO();              // Configure GPIO pins for buttons and LEDs
    initGPIO();               // Initialize GPIO pin states
    initUART();               // Initialize UART for communication
    initSPI();                // Initialize SPI for communication with DAC
    PM5CTL0 &= ~LOCKLPM5;     // Disable the GPIO high-impedance mode to enable I/O


    // Infinite loop to handle system operations
   while (1) {
       checkMode(); // Check the mode switch and update the mode variable
       if (mode == 0) {
           manualMode(); // Execute manual mode logic
       } else {
           remoteMode(); // Execute remote mode logic
       }

       updateRampSignal();    // Update the DAC output with a ramp signal
       __delay_cycles(10000);   // Small delay for ramp rate adjustment
   }



}

//-- Setup Functions

//Set pins/directions
void setupGPIO(){
    // buttons as inputs
    // LDO Button P1.1
    P1DIR &= ~BIT1; //set bit 1 to input
    P1SEL1 &= ~BIT1;
    P1SEL0 &= ~BIT1;

    // Buck Button P1.2
    P1DIR &= ~BIT2; //set bit 2 to input
    P1SEL1 &= ~BIT2;
    P1SEL0 &= ~BIT2;

    // PS Button P3.0
    P3DIR &= ~BIT0; //set bit 0 to input
    P3SEL1 &= ~BIT0;
    P3SEL0 &= ~BIT0;

    //Mode Switch P3.7
    P3DIR &= ~BIT7; //set bit 7 to input
    P3SEL1 &= ~BIT7;
    P3SEL0 &= ~BIT7;

    // LEDs as outputs
    // LDO LED on at first
    // LDO LED P3.1
    P3DIR |= BIT1; //set bit 1 to output
    P3SEL1 &= ~BIT1;
    P3SEL0 &= ~BIT1;

    // Buck LED P3.2
    P3DIR |= BIT2; //set bit 2 to output
    P3SEL1 &= ~BIT2;
    P3SEL0 &= ~BIT2;

    // PS LED P3.3
    P3DIR |= BIT3; //set bit 3 to output
    P3SEL1 &= ~BIT3;
    P3SEL0 &= ~BIT3;
}



//-- Initialization Functions

//enable interrupts and establish starting values
void initGPIO() {
    // LEDs as outputs
    // LDO LED on at first
    // LDO LED P3.1
    P3OUT |= BIT1; //turn on LDO LED

    // Buck LED P3.2
    P3OUT &= ~BIT2; //turn off Buck LED

    // PS LED P3.3
    P3OUT &= ~BIT3; //turn off PS LED

}


void initUART(){
    UCA1CTLW0 |= UCSWRST;  // put UART A1 into SW Reset
    UCA1CTLW0 |= UCSSEL__SMCLK; //Choose SMCLK for UART A0;

    // Setup modulation
    // 1 MHz clock
    // 115200  baud rate
    UCA1BRW = 8;
    UCA1MCTLW |= 0xD600;

    // Setup Pins
    // TXD
    P2SEL1 |= BIT5;
    P2SEL0 &= ~BIT5;

    //RXD
    P2SEL1 |= BIT6;
    P2SEL0 &= ~BIT6;



    UCA1CTLW0 &= ~UCSWRST; //take UART A1 out of SW Reset


}


void initSPI(){

    UCA0CTLW0 |= UCSWRST;       // Put SPI module into software reset

    UCA0CTLW0 |= UCSSEL__SMCLK; // Select SMCLK as the clock source
    UCA0CTLW0 |= UCSYNC;        // Enable synchronous mode
    UCA0CTLW0 |= UCMST;         // Set as SPI master
    UCA0CTLW0 |= UCSTEM;        // Enable STE pin
    UCA0CTLW0 &= ~UCMODE1;       // Set 4-wire SPI with STE active low
    UCA0CTLW0 &= ~UCMODE0;       // Set 4-wire SPI with STE active low
    UCA0CTLW0 &= ~UCCKPH;
    UCA0CTLW0 |= UCMSB;        // MSB First

    //Setup Pins
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

    // Release SPI module from reset
    UCA0CTLW0 &= ~UCSWRST;

}

// Helper Functions

// Function to check and update the current mode (manual or remote)
void checkMode(){
     if (!(P3IN & BIT7)){ // If Mode Switch (P3.7) is set to remote side
        mode = 1;   // set mode to remote
    } else {
        mode = 0; // set mode to manual
    }
}

void manualMode() {
    // Check each button and update ledState only if no other buttons are pressed
    if (!(P1IN & BIT1) && (P1IN & BIT2) && (P3IN & BIT0)) { // LDO button only
        ledState = 1; // Set state to LED1
    } else if (!(P1IN & BIT2) && (P1IN & BIT1) && (P3IN & BIT0)) { // Buck button only
        ledState = 2; // Set state to LED2
    } else if (!(P3IN & BIT0) && (P1IN & BIT1) && (P1IN & BIT2)) { // PS button only
        ledState = 3; // Set state to LED3
    }

    // Update LEDs based on ledState value
    if (ledState == 1) {
        P3OUT |= BIT1;            // Turn on LED1
        P3OUT &= ~(BIT2 | BIT3);  // Turn off other LEDs
    } else if (ledState == 2) {
        P3OUT |= BIT2;            // Turn on LED2
        P3OUT &= ~(BIT1 | BIT3);  // Turn off other LEDs
    } else if (ledState == 3) {
        P3OUT |= BIT3;            // Turn on LED3
        P3OUT &= ~(BIT1 | BIT2);  // Turn off other LEDs
    }

    // Small delay for debouncing
    __delay_cycles(100);
}


void remoteMode(){
    // Poll the UART receive flag
      if (UCA1IFG & UCRXIFG) {  // Check if RX flag is set (data available)
          unsigned char received = UCA1RXBUF; // Read received data from RX buffer

          // Interpret received data to control LEDs
          if (received == '1') {
              ledState = 1;
          } else if (received == '2') {
              ledState = 2;
          } else if (received == '3') {
              ledState = 3;
          }
      }

  // Update LEDs based on ledState value
      if (ledState == 1) {
          P3OUT |= BIT1;            // Turn on LED1
          P3OUT &= ~(BIT2 | BIT3);  // Turn off other LEDs
      } else if (ledState == 2) {
          P3OUT |= BIT2;            // Turn on LED2
          P3OUT &= ~(BIT1 | BIT3);  // Turn off other LEDs
      } else if (ledState == 3) {
          P3OUT |= BIT3;            // Turn on LED3
          P3OUT &= ~(BIT1 | BIT2);  // Turn off other LEDs
      }
      __delay_cycles(100);
}

void spi_send_data_with_command(unsigned int command, unsigned int data) {
    unsigned long word = ((unsigned long)0x08 << 16) | ((unsigned long)data);
    int i;
    P1OUT &= ~BIT4;
    __delay_cycles(100);
    unsigned long prog = ((unsigned long)0x04 << 16) | ((unsigned long)0x101);
    // Transmit the 24-bit word

    for (i = 2; i >= 0; i--) {
        UCA0TXBUF = (prog >> (i * 8)) & 0xFF; // Send 8 bits at a time starting with MSB
    }
    __delay_cycles(1000);
    P1OUT |= BIT4;
    __delay_cycles(1000);
    // Combine command byte and data bytes into a 24-bit word
    P1OUT &= ~BIT4;
    __delay_cycles(100);
    // Transmit the 24-bit word

    for (i = 2; i >= 0; i--) {
        UCA0TXBUF = (word >> (i * 8)) & 0xFF; // Send 8 bits at a time starting with MSB
    }
    __delay_cycles(1000);
    P1OUT |= BIT4;
}

void updateRampSignal() {
    // Define the 4 ramp values
       unsigned int rampValues[5] = {0x00, 0xF000, 0xF00F, 0xFFF0, 0xFFFF};
       static int index = 0; // Static to retain value across function calls
       spi_send_data_with_command(0x08, rampValues[index]); // Send current value to the DAC
       index = (index + 1) % 4;         // Increment index with wrap-around
}



/*
//-- ISRs





//Interrupt for LDO and Buck button
#pragma vector = PORT1_VECTOR
__interrupt void ISR_PORT_1(void){
    if (P1IFG & BIT1){ //if ldo button is pressed
        led_state = 1;

        P3OUT |= BIT1;
        P3OUT &= ~BIT2;
        P3OUT &= ~BIT3;

        P1IFG &= ~BIT1;
    } else if (P1IFG & BIT2){ //if buck button is pressed
        led_state = 2;

        P3OUT |= BIT2;
        P3OUT &= ~BIT1;
        P3OUT &= ~BIT3;

        P1IFG &= ~BIT2;
    }
}

#pragma vector = PORT3_VECTOR
__interrupt void ISR_PORT_3(void){
    if (P3IFG & BIT0){ //if ps button is pressed
        led_state = 3;

        P3OUT |= BIT3;
        P3OUT &= ~BIT2;
        P3OUT &= ~BIT1;

        P3IFG &= ~BIT0;
    } else if (P3IFG & BIT7){ // if mode is switched
        mode = !(P3IN & BIT7)

        P13FG &= ~BIT7;
    }
}

*/
