/*
 * UART.h
 * Antonio Dominguez
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Ever since the new API update in mid 2019, the UART configuration struct's
// name changed from [eUSCI_UART_Config] to [eUSCI_UART_ConfigV1]. If your code
// does not compile, uncomment the alternate #define and comment the original
// one.
// -----------------------------------------------------------------------------
// #define UART_Config eUSCI_UART_Config
#define UART_Config eUSCI_UART_ConfigV1


// TODO: define these macros based on the UART module that is connected to USB on Launchpad via XDS110

#define USB_UART_PORT GPIO_PORT_P1 //test port 3
#define USB_UART_PINS (GPIO_PIN2 | GPIO_PIN3) // The pins are given to you for guidance. Also, because many students miss the parentheses
#define USB_UART_INSTANCE EUSCI_A0_BASE
/************************************
 * NOTE: USB_UART_INSTANCE is **NOT** the same as the GPIO MODULE INSTANCE!
 *
 * There are multiple UART modules on this board. You need to specify which one will be used.
 * Which UART module instance can be used for USB-UART communication?
 ************************************/

// An enum outlining what baud rates the UART_construct() function can use in
// its initialization.
enum _UART_Baudrate
{
    BAUD_9600, BAUD_19200, BAUD_38400, BAUD_57600, NUM_BAUD_CHOICES
};
typedef enum _UART_Baudrate UART_Baudrate;

// TODO: Write an overview explanation of what this UART struct does, and how it
//       interacts with the functions below. Consult <HAL/Button.h> and
//       <HAL/LED.h> for examples on how to do this.

// -- This struct creates a UART that can receive and send chars to the designated
//    pins, ports and instance
struct _UART
{
    UART_Config config;

    uint32_t moduleInstance;
    uint32_t port;
    uint32_t pins;
};
typedef struct _UART UART;

// Constructs a uart using the moduleInstance at the given port and pin
UART UART_construct(uint32_t moduleInstance, uint32_t port, uint32_t pins);

// Sets the baudrate and enables a UART based on the given baudrate enum
void UART_SetBaud_Enable(UART *, UART_Baudrate baudrate);


// TODO: Write a comment which explains what each of these functions does. In the
//       header, prefer explaining WHAT the function does, as opposed to HOW it is
//       implemented.
bool UART_hasChar(UART* uart_p);          // sees if there is a char in the UART
char UART_receiveChar(UART* uart_p);      // Receives the char from the UART
bool UART_canSend(UART* uart_p);          // checks to see if UART can send Char
void UART_sendChar(UART* uart_p, char c); // sends the chars in MOBAX to UART

// Updates the UART baudrate to use the new baud choice.
void UART_updateBaud(UART* uart_p, UART_Baudrate baudChoice);

#endif /* HAL_UART_H_ */
