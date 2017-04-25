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
//#include "sensor_service.h"
#include "services.h"
#include "debug.h"
#include "stm32_bluenrg_ble.h"
#include "bluenrg_utils.h"
#include "stm32f4xx_hal.h"

#include <string.h>
#include <stdio.h>

/** @addtogroup X-CUBE-BLE1_Applications
 *  @{
 */

/** @defgroup SensorDemo
 *  @{
 */

/** @defgroup MAIN 
 * @{
 */

/** @defgroup MAIN_Private_Defines 
 * @{
 */
/* Private defines -----------------------------------------------------------*/
#define BDADDR_SIZE 6
#define RBUFFERSIZE_FROMDISCOVERY 750
#define RBUFFERSIZE_TODISCOVERY 500
#define TIMEOUT 1000

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
int counter_aws;
int aws_write;
uint8_t bnrg_expansion_board = IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */
GPIO_InitTypeDef uart_GPIO_struct;
GPIO_InitTypeDef toggle_GPIO_struct;
GPIO_InitTypeDef read_GPIO_struct;
UART_HandleTypeDef uart_handle_struct;
UART_InitTypeDef uart_init_struct;

uint8_t data_R[RBUFFERSIZE_FROMDISCOVERY]; 
uint8_t data_to_phone[RBUFFERSIZE_FROMDISCOVERY] = {1, 5, 7, 9, 11, 15, 12};
/**
 * @}
 */




/** @defgroup MAIN_Private_Function_Prototypes
 * @{
 */
/* Private function prototypes -----------------------------------------------*/
void User_Process(uint8_t* data_phone, AxesRaw_t* p_axes);
/**
 * @}
 */
 
 void UART_init(){	
	// Enable clocks
	__HAL_RCC_USART1_CLK_ENABLE();
	
	// Configure UART
	uart_init_struct.BaudRate = 115200;
	uart_init_struct.HwFlowCtl = UART_HWCONTROL_NONE; //////
	uart_init_struct.Mode = USART_MODE_TX_RX;
	uart_init_struct.OverSampling = UART_OVERSAMPLING_16; //////
	uart_init_struct.Parity = USART_PARITY_NONE; 
	uart_init_struct.StopBits = USART_STOPBITS_1; 
	uart_init_struct.WordLength = USART_WORDLENGTH_8B; 
	
	uart_handle_struct.Init = uart_init_struct;
	uart_handle_struct.Instance = USART1;
	
	HAL_UART_Init(&uart_handle_struct);
}

void uart_GPIO_init(){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	read_GPIO_struct.Mode = GPIO_MODE_INPUT;
	read_GPIO_struct.Pin = GPIO_PIN_4; //Read Pin
	read_GPIO_struct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB,&read_GPIO_struct);
	
	toggle_GPIO_struct.Mode = GPIO_MODE_OUTPUT_PP;
	toggle_GPIO_struct.Pin = GPIO_PIN_5; //Writing pin
	toggle_GPIO_struct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB,&toggle_GPIO_struct);
	
	// Configure GPIOs
	uart_GPIO_struct.Alternate = GPIO_AF7_USART1;
	uart_GPIO_struct.Mode = GPIO_MODE_AF_PP;
	uart_GPIO_struct.Pin = GPIO_PIN_9 | GPIO_PIN_10; // TX: PA2, RX: PA3 
	uart_GPIO_struct.Pull = GPIO_PULLUP;
	uart_GPIO_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOA,&uart_GPIO_struct);
}

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
  const char *name = "SPICY";
  uint8_t SERVER_BDADDR[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x10};
  uint8_t bdaddr[BDADDR_SIZE];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  
  uint8_t  hwVersion;
  uint16_t fwVersion;
	
//	for(int j = 0; j < RBUFFERSIZE_FROMDISCOVERY; j++)
//	{
//		data_to_phone[j] = (uint8_t)j%250;
//	}
  
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
	
	UART_init();
	uart_GPIO_init();
	
  
  PRINTF("HWver %d, FWver %d", hwVersion, fwVersion);
	PRINTF("\n\n");
  
  if (hwVersion > 0x30) { /* X-NUCLEO-IDB05A1 expansion board is used */
    bnrg_expansion_board = IDB05A1; 
    /*
     * Change the MAC address to avoid issues with Android cache:
     * if different boards have the same MAC address, Android
     * applications unless you restart Bluetooth on tablet/phone
     */
    SERVER_BDADDR[5] = 0x11;
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
  
	/*--------Start Modifications here for services-------*/
	
  ret = Add_Acc_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Acc service added successfully.\n");
  else
    PRINTF("Error while adding Acc service.\n");
	
	ret = Add_PData_Service();
	
	if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Write PData service added successfully.\n");
  else
    PRINTF("Error while adding PData service.(0x%02x)\n", ret);

  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1,4);
	
	counter_aws = 1;
	aws_write = 0;
	printf("PIN out 5: %d\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5));
	printf("PIN out 4: %d\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4));
	//HAL_Delay(1000);
	int i = 0;
	
	int read_from_discovery = 0;
	//int write_to_discovery = 0;
  while(1)
  {
		HCI_Process();
		//read_from_discovery++;
		/*{Start} Uncomment this section when using UART (Testing)*/
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))
		{
			read_from_discovery++;
			printf("pin is 1\n");
			HAL_UART_Receive(&uart_handle_struct, data_R, RBUFFERSIZE_FROMDISCOVERY, TIMEOUT);
			//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
//			//HAL_Delay(10);
			//HAL_UART_Transmit(&uart_handle_struct, data_R, RBUFFERSIZE_TODISCOVERY, TIMEOUT);
			//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
			
			/*Uncomment when using UART*/
			for(int j = 0; j < RBUFFERSIZE_FROMDISCOVERY; j++)
			{
				//Copies the first 750 bytes
				if (read_from_discovery == 1)
				{
					data_to_phone[j] = data_R[j];
				}
				//Copies the next 750 bytes
				else if(read_from_discovery == 2)
				{
					data_to_phone[j*2] = data_R[j];
				}
				else
				{
					printf("Not supposed to be here");
				}
			}
				/*Uncomment when using UART*/
		}
		
		//Write to aws once we have received data twice (all the data from one round of sampling)
		//Change read_from_discovery to 2 if we are reading all 1500 values
		if(read_from_discovery >= 1)
		{
			//Not sure how to pass phone data, might be this
			aws_write = 1;
			User_Process(data_to_phone, &axes_data);
			if(read_from_discovery == 2)
			{
				read_from_discovery = 0;
			}
			aws_write = 0;
			User_Process(data_to_phone, &axes_data);
		}
		/**{end} Using UART**/
		
		//When data_amount == 500, it means that we have received half the data from the phone
		//Then we must transmit with UART
		//Currently discovery board must know that it needs to receive twice
		if (data_amount == 500)
		{
			//SET transmit pin
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
			HAL_UART_Transmit(&uart_handle_struct, data_from_phone, RBUFFERSIZE_TODISCOVERY, TIMEOUT);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		}
		
		
		/*{Start}Uncomment this section when not using UART (Testing)*/
//		if (counter_aws % 1000 < 800 || counter_aws % 1000 > 900 ){
//			aws_write = 0;
//		}else{
//			aws_write = 1;
//		}
//		
//		if (counter_aws % 1000 == 0){
//			printf ("exiting..");
//			break;
//		}
//		User_Process(data_to_phone, &axes_data);
		/*{end} not using UART*/
		
    
  }
}

/**
 * @brief  Process user input (i.e. pressing the USER button on Nucleo board)
 *         and send the updated acceleration data to the remote client.
 *
 * @param  AxesRaw_t* p_axes
 * @retval None
 */
void User_Process(uint8_t* data_phone, AxesRaw_t* p_axes)
{
  if(set_connectable){
    setConnectable();
    set_connectable = FALSE;
  } 

	if (connected){
		
		//counter_aws++;
		
		//This puts one byte at a time into each axis
		//Should probably change axis from int32 to uint8
		
		/*{Start} Uncomment when using UART*/
		for(int j = 0; j < RBUFFERSIZE_FROMDISCOVERY; j++)
		{
			if(j%6 == 0 || j%6 == 1)
			{
				p_axes->AXIS_X = data_phone[j];
			}
			if(j%6 == 2 || j%6 == 3)
			{
				p_axes->AXIS_Y = data_phone[j];
			}
			if(j%6 == 4 || j%6 == 5)
			{
				p_axes->AXIS_Z = data_phone[j];
			}
			p_axes->AWS = aws_write;	//this must be set to one when UART is sending data
		
			//PRINTF("ACC: X=%6d Y=%6d Z=%6d\r\n", p_axes->AXIS_X, p_axes->AXIS_Y, p_axes->AXIS_Z);
			Acc_Update(p_axes);
			
		}
		/*{end} Using UART*/
		
		
		/*{Start} Uncomment when NOT using UART*/
//		p_axes->AXIS_X = 10;
//		p_axes->AXIS_Y = 100;
//		p_axes->AXIS_Z = 200;
		/*{end} Not using UART*/
		
		
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
