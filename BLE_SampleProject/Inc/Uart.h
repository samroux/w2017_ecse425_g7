#include "stm32f4xx.h"

#define TIMER_7SEG_DELAY 	10
#define UART_RX_BUFFER_SIZE	500

extern GPIO_InitTypeDef Rx;
extern GPIO_InitTypeDef Tx;
extern GPIO_InitTypeDef Flg;
extern UART_HandleTypeDef huart;

extern int uart_transmission_done;
extern uint8_t data_a[500];
extern int uart_data_available;

void init_gpio(void);
void init_UART(UART_HandleTypeDef* huart2);
void init_UARTGPIO(void);
void UART_Transmit(int pitch, int roll, UART_HandleTypeDef* uart_handler);
int* convertToBytes(int number);
int* UART_Receive(UART_HandleTypeDef* uart_handler);
int convertToInteger(int bytes0, int bytes1);
void transmit(void);
void test_transmit(void);
void receive(void);
void init_uart(void);
