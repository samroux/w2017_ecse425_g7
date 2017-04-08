/*******************************************************************************
  * @file    Thread.c
  * @author  ECSE426 Group 13: Mohamad Nizar Kezzo, Ray Wu
	* @version V1.0.0
  * @date    16-March-2017
  * @brief   This file implements the threads required for LAB 4	
  ******************************************************************************
  */
	
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "accelerometer.h"
#include "lis3dsh.h"
#include "gpio.h"
#include "keypad.h"
#include "adc.h"
#include "uart.h"
#include "display.h"
#include "stm32f4xx_it.h"

/*******************************************************************************
*****THREADS*****
Running threads:
	> Main thread: Initilializes peripherals and clocks, and starts the rest of the threads
	> UART_receive: Real-time priority thread, updates display with data it receives 
	> Temperature: Obtains temperature periodically, non-stop 
	> Accelerometer: Updates and controls the PWM with data received from accelerometer
	> Keypad: Listens to the keypad continuously and reads its input. This thread controls the mode (temp or accel)

*****SIGNALS*****
Exchanged signals:
	> 0x0001: Temperature mode signal, notifies that the current mode is temperature mode, 
						sent from keypad thread to 7-seg display and accelerometer threads
	> 0x0010: Accelerometer mode signal, notifies that the current mode is accelerometer mode, 
						sent from keypad thread to 7-seg display and accelerometer threads
	> 0x0100: Accelerometer-data-is-ready signal, sent from the accelerometer GPIO PE0 interrupt 
						to the accelerometer thread to read data
	> 0x1000: Overheat signal, sent from the temperature thread to the 7-seg display thread 
						to blink the display
	> 0x1100: Normal heat signal, sent from the temperature thread to the 7-seg display thread 
						to stop display blinking
	
*****MESSAGES*****
Exchanged messages
	>	temp_q: Carries temperature readings from the temperature thread to the 7-seg display thread
	> keypad_q: Carries keypad inputs from the keypad thread to the 7-seg display thread
	
*****INTERRUPTS*****
*******************************************************************************/
#define TIMER_7SEG_DELAY 	10
#define UART_RX_BUFFER_SIZE	500
//#define 

typedef struct{
	float temperature;
} temp_T;
typedef struct{
	float angle;
} angle_T;
typedef struct{
	float pitch;
} pitch_T;
typedef struct{
	float roll;
} roll_T;
// Interrupt flag set by accelerometer
int ACC_data_ready_flag = 0;
// Initialize sample_count to 0 always in the code
struct FIR_internal_pipe {
	float output;
	float samples[5];
	int sample_count;
};
//define filter coeff 
struct FIR_coeff {
	float f;
};
int32_t signals_ACC_data_ready;
extern UART_HandleTypeDef uart_handle_struct;
int uart_transmission_done;
uint8_t data_a[500];
int uart_data_available;


/*----------------------------------------------------------------------------
 *      Create any required timers
 *---------------------------------------------------------------------------*/
void Timer_7seg_Callback (void const *arg);
osTimerId timer_7seg_id;
osTimerDef(Timer_7seg, Timer_7seg_Callback);

/*----------------------------------------------------------------------------
 *      Create any required message Qs
 *---------------------------------------------------------------------------*/
osPoolDef(temp_pool,16,temp_T); 
osPoolId temp_pool_id;
osMessageQDef(temp_q, 1, float);
osMessageQId temp_q_id;

osPoolDef(accel_pool,16,temp_T); 
osPoolId accel_pool_id;
osMessageQDef(accel_q, 1, float);
osMessageQId accel_q_id;

osPoolDef(pitch_pool,16,temp_T); 
osPoolId pitch_pool_id;
osMessageQDef(pitch_q, 1, float);
osMessageQId pitch_q_id;

osPoolDef(roll_pool,16,temp_T); 
osPoolId roll_pool_id;
osMessageQDef(roll_q, 1, float);
osMessageQId roll_q_id;



/*----------------------------------------------------------------------------
 *      Create the thread definitions
 *---------------------------------------------------------------------------*/
//--------UART_receive thread--------//
void Thread_UART (void const *argument);                 // thread function
osThreadId tID_Thread_UART;                              // thread id
osThreadDef(Thread_UART, osPriorityRealtime, 1, 0);			 // Priority set as real time

//--------Accelerometer thread--------//
void Thread_accelerometer (void const *argument);                 // thread function
osThreadId tID_Thread_accelerometer;                              // thread id
osThreadDef(Thread_accelerometer, osPriorityNormal, 1, 0);			 // Priority set as normal

//--------UART_send thread--------//
void Thread_UART_send (void const *argument);                 // thread function
osThreadId tID_Thread_UART_send;                              // thread id
osThreadDef(Thread_UART_send, osPriorityRealtime, 1, 0);			 // Priority set as real time.


/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
//--------UART_receive thread--------//
int start_Thread_UART (void) {
  tID_Thread_UART = osThreadCreate(osThread(Thread_UART), NULL); // Start 7 seg display Thread
  if (!tID_Thread_UART) return(-1); 
  return(0);
}

//--------Accelerometer thread--------//
int start_Thread_accelerometer (void) {
  tID_Thread_accelerometer = osThreadCreate(osThread(Thread_accelerometer), NULL); // Start accelerometer Thread
  if (!tID_Thread_accelerometer) return(-1); 
  return(0);
}

//--------UART_send thread--------//
int start_Thread_UART_send(void) {
  tID_Thread_UART_send = osThreadCreate(osThread(Thread_UART_send), NULL); // Start temperature Thread
  if (!tID_Thread_UART_send) return(-1); 
  return(0);
}

//--------Timer--------//
int start_Timer_7seg (void) 
{
	osStatus status;
  timer_7seg_id = osTimerCreate(osTimer(Timer_7seg), osTimerPeriodic, NULL); // Start temperature Thread
  if (timer_7seg_id){
		status = osTimerStart(timer_7seg_id,TIMER_7SEG_DELAY); 
		if(status != osOK){
			return(-1);
		}
		return(0);
	}
  return(-1);
}

 /*----------------------------------------------------------------------------
*      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/

//--------Accelerometer thread--------//
void Thread_accelerometer(void const *argument) 
{	
	osEvent evt_accel_data_ready;
	int acc_points_count = 0;
	float raw_ACC_xyz[3];
	float raw_ACC_X_buffer[250]; // raw_ACC[0] = X acceleration, raw_ACC[1] = Y acceleration, raw_ACC[2] = Z acceleration
	float raw_ACC_Y_buffer[250];
	float raw_ACC_Z_buffer[250];
	
	// Wait for a push button for the first time
	
	while(1){
			evt_accel_data_ready = osSignalWait(0x0100, 250);
			if(evt_accel_data_ready.status == osEventSignal){ // Wait for accelerometer data to be ready

				LIS3DSH_ReadACC(&raw_ACC_xyz[0]); // Collect data once ready
				raw_ACC_X_buffer[acc_points_count] = raw_ACC_xyz[0];
				raw_ACC_Y_buffer[acc_points_count] = raw_ACC_xyz[1];
				raw_ACC_Z_buffer[acc_points_count] = raw_ACC_xyz[2];
				
				acc_points_count++;
				
				ACC_data_ready_flag = 0; // Reset data ready flag
			}	
			
			if(acc_points_count >= 249){
				// Reset point count to zero
				acc_points_count = 0;				
				// Wait forever for a push button
			}
			
		}
}


//--------UART_receive thread--------//
void Thread_UART(void const *argument) 
{
	//char mode = 'R'; // 'R' = Receive, 'T' = Transmit
	//uint8_t data = 201;
	//HAL_StatusTypeDef uart_receive_status;
	int i;
	uint8_t data_RX[UART_RX_BUFFER_SIZE];
	
	HAL_UART_Receive_IT(&uart_handle_struct, &data_a[0], UART_RX_BUFFER_SIZE);
	
	
	while(1){
		if(uart_data_available == 1){
			for(i = 0; i<UART_RX_BUFFER_SIZE; i++){
				printf("-->%i\n", data_a[i]);	
			}
			uart_data_available = 0;
		}
	}
}

//--------UART_send thread--------//
void Thread_UART_send(void const *argument) 
{
	uint8_t data_T[500];
	//data_T = 8;
	
	while(1){
		//uart_send_byte(data, 10);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		//HAL_UART_Transmit_IT(&uart_handle_struct, data_T, 500);
		//data_T++;
		//if(data_T > 100){
			//data_T = 8;
		//}
		//osDelay(500);
	}
}


//--------Temperature thread--------//
void Timer_7seg_Callback (void const *arg)
{
	// Set a flag based on the timer
}

/*----------------------------------------------------------------------------
*      Start threads
 *---------------------------------------------------------------------------*/
void start_Threads(void)
{
	temp_q_id = osMessageCreate(osMessageQ(temp_q), NULL);
	temp_pool_id = osPoolCreate(osPool(temp_pool));
	accel_q_id = osMessageCreate(osMessageQ(accel_q), NULL);
	accel_pool_id = osPoolCreate(osPool(accel_pool));
	pitch_q_id = osMessageCreate(osMessageQ(pitch_q), NULL);
	pitch_pool_id = osPoolCreate(osPool(pitch_pool));
	roll_q_id = osMessageCreate(osMessageQ(roll_q), NULL);
	roll_pool_id = osPoolCreate(osPool(roll_pool));

	start_Thread_UART_send();	
	start_Thread_UART();
	start_Thread_accelerometer();
	//start_Thread_keypad();

}


