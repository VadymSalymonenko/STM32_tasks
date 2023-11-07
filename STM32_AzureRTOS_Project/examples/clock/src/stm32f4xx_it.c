/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern unsigned long millis_counter;
extern unsigned long sleep_time_estimate;
extern unsigned long sleep_veto_time_estimate;
extern const unsigned long SLEEP_VETO_DEBOUNCE;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
float* CalculateProbabilities(TX_THREAD* threads, int thread_count);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
/* USER CODE BEGIN EV */

extern void UART_Print_Num(const char *message, unsigned long number);
extern void buttonSetInterruptHandler(void);
extern void buttonESCInterruptHandler(void);
extern void buttonLeftInterruptHandler(void);
extern void buttonRightInterruptHandler(void);
extern void Enter_SleepMode(void);
extern void Exit_SleepMode(void);
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */
	Exit_SleepMode();
	sleep_veto_time_estimate = SLEEP_VETO_DEBOUNCE;
	buttonESCInterruptHandler();

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI1_IRQn 1 */

  /* USER CODE END EXTI1_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */
	//if(millis_counter%100 == 0) UART_Print_Num("TIM3_IRQHandler ", millis_counter);
	/*if(sleep_time_estimate == 1 ){
		sleep_time_estimate--;
		Exit_SleepMode();
		 UART_Print_Num("sleep_time_estimate 11111 ", sleep_time_estimate);

	}
	if(sleep_time_estimate > 1 ){
		sleep_time_estimate--;
		 UART_Print_Num("sleep_time_estimate ", sleep_time_estimate);
		    UART_Print_Num("sleep_time_estimate <<<<<<<<<<<<>><> ", sleep_time_estimate);

	}
*/
	if(sleep_veto_time_estimate != 0) sleep_veto_time_estimate--;
	millis_counter++;
  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */

void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
	  Exit_SleepMode();
	  sleep_veto_time_estimate = SLEEP_VETO_DEBOUNCE;

	  if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_13)) {
		  buttonLeftInterruptHandler();
	      __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_13);
	      return;
	  }

	  if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_14)) {
		  buttonSetInterruptHandler();
	      __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_14);
	      return;
	  }

	  if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_15)) {
		  buttonRightInterruptHandler();
;	      __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_15);
	      return;
	  }
  /* USER CODE END EXTI15_10_IRQn 0 */

  //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//UART_Print_Num("HAL_TIM_PeriodElapsedCallback ", millis_counter);
	//millis_counter++;
}
/* USER CODE END 1 */
