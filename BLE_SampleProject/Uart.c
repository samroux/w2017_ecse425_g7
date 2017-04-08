/***************************************************/
//Author: Omar Ba mahmos
//Microprocessor Systems
//ECSE 426
//Final project
//this file inclues the functions that handle the UART
/*************************************************/
#include "cube_hal.h"

#include "osal.h"
#include "sensor_service.h"
#include "debug.h"
#include "stm32_bluenrg_ble.h"
#include "bluenrg_utils.h"

#include "UART.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_conf.h"
#define BDADDR_SIZE 6
#define DATA_SIZE 0xff
#define RX_ARR_SIZE 0xff
#define TIMEOUT   2000


extern uint8_t pData_unfilt_x1[RX_ARR_SIZE];
extern uint8_t pData_unfilt_x2[RX_ARR_SIZE];
extern uint8_t pData_unfilt_y1[RX_ARR_SIZE];
extern uint8_t pData_unfilt_y2[RX_ARR_SIZE];
extern uint8_t pData_unfilt_z1[RX_ARR_SIZE];
extern uint8_t pData_unfilt_z2[RX_ARR_SIZE];
extern uint8_t pData_filt[DATA_SIZE];

GPIO_InitTypeDef Rx;
GPIO_InitTypeDef Tx;


int Tx_FLAG=RESET;
int Rx_FLAG=RESET;

//void init_UART(UART_HandleTypeDef* huart2){
//	//declare the uart
//	printf("uart\n");
//	__HAL_RCC_USART2_CLK_ENABLE();
//	
//	init_UARTGPIO();

//  huart2->Instance = USART1;
//	
//  huart2->Init.BaudRate = 9600;
//  huart2->Init.WordLength = UART_WORDLENGTH_8B;
//  huart2->Init.StopBits = UART_STOPBITS_1;
//  huart2->Init.Parity = UART_PARITY_NONE;
//  huart2->Init.Mode = UART_MODE_TX_RX;
//  huart2->Init.HwFlowCtl = UART_HWCONTROL_NONE;
//	huart2->Init.OverSampling=UART_OVERSAMPLING_8;
//	
//  HAL_UART_Init(&huart);

//}

UART_InitTypeDef uart_init_struct;

void init_uart() 
{	
	// Enable clocks
	__HAL_RCC_USART2_CLK_ENABLE();
	
	// Initialize UART
	huart.Instance        = USART2;
  huart.Init.BaudRate   = 9600;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits   = UART_STOPBITS_1;
  huart.Init.Parity     = UART_PARITY_NONE;
  huart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  huart.Init.Mode       = UART_MODE_TX_RX;
  
  if(HAL_UART_Init(&huart) != HAL_OK)
  {
    
  }
	
	// Choose UART instance
//	huart.Init = uart_init_struct;
//	huart.Instance = USART2;
	
	// Enable UART interrupts
//	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(USART2_IRQn);
	
	//HAL_UART_Init(&huart);
	//printf("UART enabled\n");
} 

//INIT GPIO
void init_gpio()
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	Rx.Pin        = GPIO_PIN_10;
	Tx.Pin       = GPIO_PIN_9;
	
	Rx.Mode       = GPIO_MODE_AF_PP;
	Tx.Mode      = GPIO_MODE_AF_PP;
	
	Rx.Pull       = GPIO_PULLUP;
	Tx.Pull      = GPIO_PULLUP;
	
	Rx.Speed      = GPIO_SPEED_FREQ_HIGH;
	Tx.Speed     = GPIO_SPEED_FREQ_HIGH;
	
	Rx.Alternate  = GPIO_AF7_USART2;
	Tx.Alternate = GPIO_AF7_USART2;
	
	HAL_GPIO_Init(GPIOA, &Rx);
	HAL_GPIO_Init(GPIOA, &Tx);
	//HAL_GPIO_Init(GPIOA, &Flg);
	//printf("GPIO enabled\n");
}


/*****************TRANSMISSION********************/
void UART_Transmit(int pitch, int roll, UART_HandleTypeDef* uart_handler){
	int size=4;			
	uint8_t buffer[size];
	int* pitch_re = convertToBytes(pitch);
	int* roll_re = convertToBytes(roll);
	//put bytes in the buffer
	buffer[0]=pitch_re[0];
	buffer[1]=pitch_re[1];
	
	buffer[2]=roll_re[0];
	buffer[3]=roll_re[1];

	
	printf("buffer 0   %d\n", buffer[0]);
	printf("buffer 1   %d\n", buffer[1]);
	
	while (HAL_UART_GetState(uart_handler) == HAL_BUSY);
	

	HAL_UART_Transmit(uart_handler, buffer, size, 1000);
		
}

//convert a signed number into an unsigned number repreesnting it in two's complement

int* convertToBytes(int number){
	int bytes[2];
	
	if(number>=0){
		bytes[0]=number%256;
		bytes[1]=number/256;
	
	}else{
		int new_number=2048+(2048+number);			//the resultant unsigned integer of the two's complement of the original number
		bytes[0]=new_number%256;
		bytes[1]=new_number/256;
	}
	return bytes;

}


int* UART_Receive(UART_HandleTypeDef* uart_handler)
{
	printf("start receive\n");
	int size = 6;
	int buffer[size];
	int processed_val[3];
	
	HAL_StatusTypeDef status = HAL_TIMEOUT;
	
	//while(status != HAL_OK) status = 
	HAL_UART_Receive(uart_handler, (uint8_t*)buffer, size, TIMEOUT);
	
	
	printf("buffer 0: %d buffer 1: %d\n", buffer[0], buffer[1]);
	
	//convert to integers for pithc and roll values
	processed_val[0] = convertToInteger(buffer[0], buffer[1]);
	HAL_Delay(100);
	printf("processed_val[0]: %d", processed_val[0]);
	processed_val[1] = convertToInteger(buffer[2], buffer[3]);
	HAL_Delay(100);
	printf("processed_val[1]: %d", processed_val[1]);
	processed_val[2] = convertToInteger(buffer[4], buffer[5]);
	HAL_Delay(100);
	printf("processed_val[2]: %d", processed_val[2]);
	
	HAL_Delay(100);

	
	return processed_val;
	

}

//this function convert a bytes in two's complement into an integer
int convertToInteger(int bytes0, int bytes1){
	//assuming recevied values are greater than or equal to zero
	return bytes0+256*bytes1;

}

void receive() 
{
	HAL_StatusTypeDef status = HAL_TIMEOUT;
	
	printf("Before Receive");
	
	while(status != HAL_OK) status 	= HAL_UART_Receive(&huart, &pData_unfilt_x1[0], RX_ARR_SIZE, TIMEOUT);
	printf("finish 1");
	while(status != HAL_OK) status 	= HAL_UART_Receive(&huart, &pData_unfilt_x2[0], RX_ARR_SIZE, TIMEOUT);
	while(status != HAL_OK) status 	= HAL_UART_Receive(&huart, &pData_unfilt_y1[0], RX_ARR_SIZE, TIMEOUT);
	while(status != HAL_OK) status 	= HAL_UART_Receive(&huart, &pData_unfilt_y2[0], RX_ARR_SIZE, TIMEOUT);
	while(status != HAL_OK) status 	= HAL_UART_Receive(&huart, &pData_unfilt_z1[0], RX_ARR_SIZE, TIMEOUT);
	while(status != HAL_OK) status 	= HAL_UART_Receive(&huart, &pData_unfilt_z2[0], RX_ARR_SIZE, TIMEOUT);

}

//void transmit() 
//{
//	HAL_StatusTypeDef status = HAL_TIMEOUT;
//	while(status != HAL_OK) 
//	{
//			status = HAL_UART_Transmit(&huart, &pData_filt[0], DATA_SIZE, TIMEOUT);
//	}
//}

//void test_transmit()
//{
//	int size = 4;
//	uint8_t test[size];
//	
//	test[0] = 1;
//	test[1] = 2;
//	test[2] = 3;
//	test[3] = 4;
//	
//	while(1) {
//		
//	HAL_StatusTypeDef status = HAL_TIMEOUT;

//		while(status != HAL_OK)
//		{
//			status = 	HAL_UART_Transmit(&huart, test, size, 2000);
//			printf("Status: %d\n", status);
//			printf("Test[0]: %d\n", test[0]);
//		}
//	}
//}

