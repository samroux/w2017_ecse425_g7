/**
  ******************************************************************************
  * @file    main.c 
  * @author  CL
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   This application contains an example which shows how implementing
  *          a proprietary Bluetooth Low Energy profile: the sensor profile.
  *          The communication is done using a Nucleo board and a Smartphone
  *          with BTLE.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
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

#include <string.h>
#include <stdio.h>

#define TIMER_7SEG_DELAY 	10
#define UART_RX_BUFFER_SIZE	500

#define BDADDR_SIZE 6
#define DATA_SIZE 0xff
#define RX_ARR_SIZE 0xff
#define TIMEOUT   2000

 uint8_t pData_unfilt_x1[RX_ARR_SIZE];
 uint8_t pData_unfilt_x2[RX_ARR_SIZE];
 uint8_t pData_unfilt_y1[RX_ARR_SIZE];
uint8_t pData_unfilt_y2[RX_ARR_SIZE];
 uint8_t pData_unfilt_z1[RX_ARR_SIZE];
 uint8_t pData_unfilt_z2[RX_ARR_SIZE];
uint8_t pData_filt[DATA_SIZE];

int uart_transmission_done;
uint8_t data_a[500];
int uart_data_available;

/** @addtogroup X-CUBE-BLE1_Applications
 *  @{
 */

/** @defgroup SensorDemo
 *  @{
 */

/** @defgroup MAIN 
 * @{
 */
//void init_uart(void);
//GPIO_InitTypeDef Rx;
//GPIO_InitTypeDef Tx;
//GPIO_InitTypeDef Flg;
UART_HandleTypeDef huart;

/** @defgroup MAIN_Private_Defines 
 * @{
 */
/* Private defines -----------------------------------------------------------*/
#define BDADDR_SIZE 6

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer))
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE
  
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/**
 * @}
 */
 
/* Private macros ------------------------------------------------------------*/

/** @defgroup MAIN_Private_Variables
 * @{
 */
/* Private variables ---------------------------------------------------------*/
extern volatile uint8_t set_connectable;
extern volatile int connected;
extern AxesRaw_t axes_data;
uint8_t bnrg_expansion_board = IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */
/**
 * @}
 */

/** @defgroup MAIN_Private_Function_Prototypes
 * @{
 */
/* Private function prototypes -----------------------------------------------*/
void User_Process(AxesRaw_t* p_axes);
//HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
/**
 * @}
 */
 
/**
 * @brief  Main function to show how to use the BlueNRG Bluetooth Low Energy
 *         expansion board to send data from a Nucleo board to a smartphone
 *         with the support BLE and the "BlueNRG" app freely available on both
 *         GooglePlay and iTunes.
 *         The URL to the iTunes for the "BlueNRG" app is
 *         http://itunes.apple.com/app/bluenrg/id705873549?uo=5
 *         The URL to the GooglePlay is
 *         https://play.google.com/store/apps/details?id=com.st.bluenrg
 *         The source code of the "BlueNRG" app, both for iOS and Android, is
 *         freely downloadable from the developer website at
 *         http://software.g-maps.it/
 *         The board will act as Server-Peripheral.
 *
 *         After connection has been established:
 *          - by pressing the USER button on the board, the cube showed by
 *            the app on the smartphone will rotate.
 *          
 *         The communication is done using a vendor specific profile.
 *
 * @param  None
 * @retval None
 */
int main(void)
{
  const char *name = "BlueNRG";
  uint8_t SERVER_BDADDR[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x03};
  uint8_t bdaddr[BDADDR_SIZE];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  
  uint8_t  hwVersion;
  uint16_t fwVersion;
  
  int ret;  
  
  /* STM32Cube HAL library initialization:
   *  - Configure the Flash prefetch, Flash preread and Buffer caches
   *  - Systick timer is configured by default as source of time base, but user 
   *    can eventually implement his proper time base source (a general purpose 
   *    timer for example or other time source), keeping in mind that Time base 
   *    duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
   *    handled in milliseconds basis.
   *  - Low Level Initialization
   */
  HAL_Init();
  
#if NEW_SERVICES
  /* Configure LED2 */
  BSP_LED_Init(LED2); 
#endif
  
  /* Configure the User Button in GPIO Mode */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* Configure the system clock */
	/* SYSTEM CLOCK = 32 MHz */
  SystemClock_Config();
  
  /* Initialize the BlueNRG SPI driver */
  BNRG_SPI_Init();
  
  /* Initialize the BlueNRG HCI */
  HCI_Init();

  /* Reset BlueNRG hardware */
  BlueNRG_RST();
    
  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);

  /* 
   * Reset BlueNRG again otherwise we won't
   * be able to change its MAC address.
   * aci_hal_write_config_data() must be the first
   * command after reset otherwise it will fail.
   */
  BlueNRG_RST();
  
	init_uart();
	init_gpio();
	
//	uint8_t data_T[10] = {15,1,2,3,4,5,6,7,8,9};
//	printf("data 0: %d\n", data_T[9]);
//	
//	//while(1)
//	{
//		//uart_send_byte(data, 10);
//		//HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
//		HAL_UART_Transmit_IT(&huart, (uint8_t *)data_T, 10);
//		HAL_Delay(500);
//	}
//	
//	int i;
//	uint8_t data_RX[10];
//	
//	HAL_UART_Receive_IT(&huart, (uint8_t *)data_RX, 10);
//	
//	
//	while(1){
//		if(uart_data_available == 1){
//			for(i = 0; i<10; i++){
//				printf("%d-->%i\n", i, data_RX[i]);	
//			}
//			uart_data_available = 0;
//		}
//	}
	
	int* vals;
	int* vals2;
	int* vals3;
	int i = 0;
	
	while (HAL_UART_GetState(&huart) == HAL_BUSY);
	printf("not busy\n");
	uint8_t a[] = {6};
	
	

 //Buffer used for reception 
uint8_t aTxBuffer[] = {5, 6, 9, 11};
uint8_t aRxBuffer[RXBUFFERSIZE];

	int j= 0;
	while (j < RXBUFFERSIZE)
	{
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	
		
		
		if(HAL_UART_Transmit(&huart, aTxBuffer, TXBUFFERSIZE, 2000)== HAL_OK)
		{
			printf("trans\n");
		}
		printf("bs:%d\n", aTxBuffer[0]);
		printf("bs2:%d\n", aTxBuffer[TXBUFFERSIZE-1]);
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
		//HAL_Delay(500);
		
		HAL_StatusTypeDef status = HAL_TIMEOUT;
		//##-3- Put UART peripheral in reception process ########################### 
		if(HAL_UART_Receive(&huart, aRxBuffer, RXBUFFERSIZE, 2000) == HAL_OK)
		{  
			printf("waddup\n");
		}
		printf("receive\n");
		printf("r:%d\n", aRxBuffer[0]);
		printf("r0:%d\n", *aRxBuffer);
		printf("r1:%d\n", aRxBuffer[RXBUFFERSIZE-1]);
		j++;
		aRxBuffer[0] = 0;
		
	}
	
	
	while (1)
	{
		//i++;
		//HAL_UART_Transmit(&huart, a, 15, 10);
		HAL_Delay(100);
		//vals = UART_Receive(&huart);
		//vals2 = UART_Receive(&huart);
		//vals3 = UART_Receive(&huart);
//		printf("finish a loop\n");
//		printf("Val[0]: %d\n", vals[0]);
//		printf("Val[1]: %d\n", vals[1]);
//		printf("Val[2]: %d\n", vals[2]);
		
	}
//		printf("Val[0]: %d\n", vals[0]);
//		printf("Val[1]: %d\n", vals[1]);
//		printf("Val[2]: %d\n", vals[2]);
//	printf("Val[0]: %d\n", vals2[0]);
//		printf("Val[1]: %d\n", vals2[1]);
//		printf("Val[2]: %d\n", vals2[2]);
//	printf("Val[0]: %d\n", vals3[0]);
//		printf("Val[1]: %d\n", vals3[1]);
//		printf("Val[2]: %d\n", vals3[2]);
//	
//		printf("Loop num: %d\n", i);

	receive();
	
	printf("x1 %d\n", pData_unfilt_x1[0]);
	printf("x1 %d\n", pData_unfilt_x2[0]);
	
  PRINTF("HWver %d, FWver %d", hwVersion, fwVersion);
	PRINTF("\n\n");
  
  if (hwVersion > 0x30) { /* X-NUCLEO-IDB05A1 expansion board is used */
    bnrg_expansion_board = IDB05A1; 
    /*
     * Change the MAC address to avoid issues with Android cache:
     * if different boards have the same MAC address, Android
     * applications unless you restart Bluetooth on tablet/phone
     */
    SERVER_BDADDR[5] = 0x02;
  }

  /* The Nucleo board must be configured as SERVER */
  Osal_MemCpy(bdaddr, SERVER_BDADDR, sizeof(SERVER_BDADDR));
  
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);
  if(ret){
    PRINTF("Setting BD_ADDR failed.\n");
  }
  
  ret = aci_gatt_init();    
  if(ret){
    PRINTF("GATT_Init failed.\n");
  }

  if (bnrg_expansion_board == IDB05A1) {
    ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x03, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }
  else {
    ret = aci_gap_init_IDB04A1(GAP_PERIPHERAL_ROLE_IDB04A1, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }

  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("GAP_Init failed.\n");
  }

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   strlen(name), (uint8_t *)name);

  if(ret){
    PRINTF("aci_gatt_update_char_value failed.\n");            
    while(1);
  }
  
  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
                                     OOB_AUTH_DATA_ABSENT,
                                     NULL,
                                     7,
                                     16,
                                     USE_FIXED_PIN_FOR_PAIRING,
                                     123456,
                                     BONDING);
  if (ret == BLE_STATUS_SUCCESS) {
    PRINTF("BLE Stack Initialized.\n");
  }
  
  PRINTF("SERVER: BLE Stack Initialized\n");
  
  ret = Add_Acc_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Acc service added successfully.\n");
  else
    PRINTF("Error while adding Acc service.\n");
  
  ret = Add_Environmental_Sensor_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Environmental Sensor service added successfully.\n");
  else
    PRINTF("Error while adding Environmental Sensor service.\n");

#if NEW_SERVICES
  /* Instantiate Timer Service with two characteristics:
   * - seconds characteristic (Readable only)
   * - minutes characteristics (Readable and Notifiable )
   */
  ret = Add_Time_Service(); 
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Time service added successfully.\n");
  else
    PRINTF("Error while adding Time service.\n");  
  
  /* Instantiate LED Button Service with one characteristic:
   * - LED characteristic (Readable and Writable)
   */  
  ret = Add_LED_Service();

  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("LED service added successfully.\n");
  else
    PRINTF("Error while adding LED service.\n");  
#endif

  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1,4);

  while(1)
  {
    HCI_Process();
    User_Process(&axes_data);
#if NEW_SERVICES
    Update_Time_Characteristics();
#endif
  }
}

/**
 * @brief  Process user input (i.e. pressing the USER button on Nucleo board)
 *         and send the updated acceleration data to the remote client.
 *
 * @param  AxesRaw_t* p_axes
 * @retval None
 */
void User_Process(AxesRaw_t* p_axes)
{
  if(set_connectable){
    setConnectable();
    set_connectable = FALSE;
  }  

  /* Check if the user has pushed the button */
  if(BSP_PB_GetState(BUTTON_KEY) == RESET)
  {
    while (BSP_PB_GetState(BUTTON_KEY) == RESET);
    
    //BSP_LED_Toggle(LED2); //used for debugging (BSP_LED_Init() above must be also enabled)
    
    if(connected)
    {
      /* Update acceleration data */
      p_axes->AXIS_X += 1;
      p_axes->AXIS_Y -= 1;
      p_axes->AXIS_Z += 2;
      //PRINTF("ACC: X=%6d Y=%6d Z=%6d\r\n", p_axes->AXIS_X, p_axes->AXIS_Y, p_axes->AXIS_Z);
      Acc_Update(p_axes);
    }
  }
}

/**
 * @}
 */
 
/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
