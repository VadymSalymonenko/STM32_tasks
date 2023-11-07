/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "app_threadx.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "i2c-lcd.h"
#include <stdio.h>
#include "i2c-lcd.h"

enum directions {
    LEFT = -1,
	STAY,
    RIGHT,

};

enum MenuState {
    MENU_HOME,
    MENU_ITEM_1,
	MENU_ITEM_2,
	MENU_ITEM_3,
    MENU_SET_TIME,
    MENU_SET_ALARM,
    MENU_SET_REMINDER
} currentMenuState = MENU_HOME, last_menu_state = MENU_HOME;

typedef struct {
    unsigned int year;
    unsigned char month;
    unsigned char date;
    unsigned char hour;
    unsigned char min;
} TimeStamp;

typedef struct {
    TimeStamp *data;
    size_t size;
    size_t capacity;
} DynamicArray;

#define MAX_THREAD_POOL 20
#define PRIORITY_RANGE 32
#define MAX_MENU_ITEM MENU_ITEM_3
#define DEBOUNCE_THRESHOLD 150
#define SLEEP_TIME 1000
const unsigned long SLEEP_VETO_DEBOUNCE = 60000;


unsigned long millis_counter = 0;
int curr_mode_value = 0;
int curr_mode_status = 0;
unsigned char esc_flag = 0;
unsigned long sleep_time_estimate = 0;
unsigned long sleep_veto_time_estimate = 0;

unsigned long lastSetTime = 0;
unsigned long lastESCTime = 0;
unsigned long lastLeftTime = 0;
unsigned long lastRightTime = 0;

DynamicArray alarms;
DynamicArray reminders;

TX_THREAD led_blink_thread;
TX_THREAD screen_manager_thread;
TX_THREAD rtc_get_time_thread;
TX_THREAD reminders_check_thread;
TX_THREAD alarms_check_thread;
TX_THREAD sleep_thread;

UCHAR                   led_blink_thread_stack[1024];
UCHAR                   screen_manager_thread_stack[1024];
UCHAR                   rtc_get_time_thread_stack[1024];
UCHAR                   reminders_check_thread_stack[1024];
UCHAR                   alarms_check_thread_stack[1024];
UCHAR                   sleep_thread_stack[512];

RTC_TimeTypeDef currTime;
RTC_DateTypeDef currDate;


UINT App_ThreadX_Init(VOID *memory_ptr);
void MX_ThreadX_Init(void);
void delay_TIM3(unsigned long delay_time);
void initializeDynamicArray(DynamicArray *array, size_t initialCapacity);
void start_reminder_signal();
void start_alarm_signal();
void addElement(DynamicArray *array, int year, int month, int date, int hour, int min);
void removeElement(DynamicArray *array, TimeStamp element);

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void show_initial_screen(void);
void print_number_at_position(unsigned long number, int row, int col);
void update_initial_screen(void);

void buttonSetInterruptHandler(void);
void buttonESCInterruptHandler(void);
void buttonLeftInterruptHandler(void);
void buttonRightInterruptHandler(void);
void update_menu();
void displayMainMenu(char *menuText);
int elementExists(DynamicArray *array, TimeStamp element);
void display_action_icon();

void Enter_SleepMode(void);
void Exit_SleepMode(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void UART_Print(const char *str) {
    HAL_UART_Transmit(&huart4, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void ulong_to_str(unsigned long n, char *str) {
    int i = 0, j = 0;
    char temp[20]; // Тимчасовий масив для зберігання рядка

    do {
        temp[i++] = (n % 10) + '0';
        n = n / 10;
    } while (n);

    while (i--) {
        str[j++] = temp[i];
    }
    str[j] = '\0';
}
void UART_Print_Num(const char *message, unsigned long number) {
    char buffer[50];
    ulong_to_str(number, buffer);

    UART_Print(message);
    UART_Print(": ");
    UART_Print(buffer);
    UART_Print("\r\n");
}


void led_blink_thread_entry(ULONG thread_input) {


    char buffer[50];
    unsigned long test = 0;
    unsigned long counter_value = 0;
    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
       // UART_Print("LED AAAA\r\n");
        ulong_to_str(millis_counter, buffer);
       // UART_Print("millis_counter: ");
      //  UART_Print(buffer);
       // UART_Print("\r\n");

        //tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 1);
        tx_thread_relinquish();
        test++;
        counter_value++; // Інкремент лічильника
      //  UART_Print("LED BBB\r\n");
    }

}
void screen_manager_entry(ULONG thread_input) {
    while (1) {
        update_menu();
    	//tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 25);
        tx_thread_relinquish();

    }
}

void rtc_get_time_entry(ULONG thread_input) {
    while (1) {
         HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
         HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);
         UART_Print_Num("Current Hours: ", currTime.Hours);
         UART_Print_Num("Current Minutes: ", currTime.Minutes);
         //tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
         tx_thread_relinquish();

    }

}

void reminders_check_entry(ULONG thread_input) {
    while (1) {
        TimeStamp currentTimeStamp = {
            .year = currDate.Year,
            .month = currDate.Month,
            .date = currDate.Date,
            .hour = currTime.Hours,
            .min = currTime.Minutes
        };
        if (elementExists(&reminders, currentTimeStamp)) {
            start_reminder_signal();
            removeElement(&reminders, currentTimeStamp);
        }
        //tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
        tx_thread_relinquish();

    }
}


void alarms_check_entry(ULONG thread_input) {
    while (1) {
        UART_Print_Num("alarms size--------->>>>>>: ", alarms.size);
        TimeStamp currentTimeStamp = {
            .year = currDate.Year,
            .month = currDate.Month,
            .date = currDate.Date,
            .hour = currTime.Hours,
            .min = currTime.Minutes
        };
        if (elementExists(&alarms, currentTimeStamp)) {
            start_alarm_signal();
            removeElement(&alarms, currentTimeStamp);
        }
        //tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
        tx_thread_relinquish();

    }

}


void Enter_SleepMode(void) {
    HAL_SuspendTick();

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void Exit_SleepMode(void) {
	if(sleep_time_estimate > 0){
		sleep_time_estimate = 0;
		HAL_ResumeTick();
	}
}
void sleep_entry(ULONG thread_input) {
    while (1) {
    	if(sleep_veto_time_estimate == 0){
			UART_Print_Num(">>>>>>>>>>sleep_entry: ", millis_counter);

			sleep_time_estimate = SLEEP_TIME;
			unsigned long sleep_start = millis_counter;

			while (millis_counter - sleep_start < sleep_time_estimate){
				Enter_SleepMode();
			}

			Exit_SleepMode();
			UART_Print_Num(">>>>>>>>>>sleep_out: ", millis_counter);
    	}
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 20);

    }
}
void tx_application_define(void *first_unused_memory){


    tx_thread_create(&led_blink_thread, "LED Blink Thread",
                    led_blink_thread_entry, 0,
                    led_blink_thread_stack, sizeof(led_blink_thread_stack),
                    25, 25, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&screen_manager_thread, "screen manager Thread",
    		        screen_manager_entry, 0,
					screen_manager_thread_stack, sizeof(screen_manager_thread_stack),
                    2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&rtc_get_time_thread, "rtc get time Thread",
    		        rtc_get_time_entry, 0,
					rtc_get_time_thread_stack, sizeof(rtc_get_time_thread_stack),
                    2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&reminders_check_thread, "reminders check Thread",
    				reminders_check_entry, 0,
					reminders_check_thread_stack, sizeof(reminders_check_thread_stack),
					2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&alarms_check_thread, "alarms check Thread",
    				alarms_check_entry, 0,
					alarms_check_thread_stack, sizeof(alarms_check_thread_stack),
                    2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&sleep_thread, "sleep mode Thread",
    				sleep_entry, 0,
					sleep_thread_stack, sizeof(sleep_thread_stack),
                    1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  UART_Print("main 00-----\r\n");
  initializeDynamicArray(&alarms, 10);
  initializeDynamicArray(&reminders, 10);

  HAL_TIM_Base_Start_IT(&htim3);
  delay_TIM3(100);
  lcd_init_via_TIM3 ();
  delay_TIM3(100);

  show_initial_screen();
  update_initial_screen();

  free(alarms.data);
  free(reminders.data);

  UART_Print("main 11\r\n");


  //HAL_TIM_Base_Start_IT(&htim4);

  /* USER CODE END 2 */

  MX_ThreadX_Init();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};

    // Включити внутрішній LSI генератор
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        // Initialization Error
        Error_Handler();
    }

    // Вибрати LSI як джерело годинника для RTC
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    RCC_PeriphCLKInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if(HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
    {
        // Initialization Error
        Error_Handler();
    }

    // Включити периферію RTC
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x17;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
  sDate.Month = RTC_MONTH_OCTOBER;
  sDate.Date = 0x13;
  sDate.Year = 0x23;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 80-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

static void MX_RTC_Init_parameters(unsigned int year, unsigned char month, unsigned char date,
                                   unsigned char hour, unsigned char min)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    RCC_PeriphCLKInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if(HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_RCC_RTC_ENABLE();

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }

    sTime.Hours = ((hour / 10) << 4) | (hour % 10);
    sTime.Minutes = ((min / 10) << 4) | (min % 10);
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = ((month / 10) << 4) | (month % 10);
    sDate.Date = ((date / 10) << 4) | (date % 10);
    sDate.Year = ((year / 10) << 4) | (year % 10);
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
}


void delay_TIM3(unsigned long delay_time){
	unsigned long start_time = millis_counter;
	while (millis_counter - start_time < delay_time){

	}
}

void print_number_at_position(unsigned long number, int row, int col) {
    char num_str[4];

    ulong_to_str(number, num_str);

    if (number < 10) {
        num_str[1] = num_str[0];
        num_str[0] = '0';
        num_str[2] = '\0';
    }
    lcd_put_cur(row, col);
    delay_TIM3(2);
    lcd_send_string(num_str);
}

void show_empty_time(void){
    lcd_clear();
    delay_TIM3(2);
    lcd_put_cur(0, 3);
    delay_TIM3(2);
    lcd_send_string ("20  .  .  ");
    delay_TIM3(2);
    lcd_put_cur(1, 5);
    delay_TIM3(2);
    lcd_send_string ("  :  ");
    delay_TIM3(2);
}

void show_initial_screen(void){
	show_empty_time();
}

void displayMainMenu(char *menuText) {
    lcd_clear();
    delay_TIM3(2);

    lcd_put_cur(0, 0);
    delay_TIM3(2);

    lcd_send_string(menuText);
    delay_TIM3(2);
}

void update_initial_screen(void){
    print_number_at_position(currDate.Year, 0, 5);
    delay_TIM3(2);
    print_number_at_position(currDate.Month, 0, 8);
    delay_TIM3(2);
    print_number_at_position(currDate.Date, 0, 11);
    delay_TIM3(2);
    print_number_at_position(currTime.Hours, 1, 5);
    delay_TIM3(2);
    print_number_at_position(currTime.Minutes, 1, 8);
    delay_TIM3(2);
}
int display_number_on_coordinates(int x, int y, int minRange, int maxRange) {
    int last_number = -1;
    int last_status = curr_mode_status;

    while (curr_mode_status == last_status) {
        UART_Print_Num("last_status: ", last_status);
        UART_Print_Num("curr_mode_status: ", curr_mode_status);
        if(esc_flag == 1){
        	break;
        	return -1;
        }
        if (curr_mode_value != last_number) {
        	last_number = curr_mode_value;

            if (curr_mode_value < minRange) curr_mode_value = minRange;
            if (curr_mode_value > maxRange) curr_mode_value = maxRange;

            print_number_at_position(curr_mode_value, x , y);
        }
    }
    return curr_mode_value;
}
void process_input_to_menu_item(){
	unsigned int year;
	unsigned char month;
	unsigned char date;
	unsigned char hour;
	unsigned char min;

	year = display_number_on_coordinates(0, 5, 23, 30);
	UART_Print("step--1\r\n");
	month = display_number_on_coordinates(0, 8, 1, 12);
	UART_Print("step--2\r\n");
	date = display_number_on_coordinates(0, 11, 1, 31);
	UART_Print("step--3\r\n");
	hour = display_number_on_coordinates(1, 5, 0, 23);
	UART_Print("step--4\r\n");
	min = display_number_on_coordinates(1, 8, 0, 59);
	if(esc_flag == 1){
		esc_flag = 0;
		return;
	}

	if(currentMenuState  == MENU_SET_TIME)MX_RTC_Init_parameters(year, month, date, hour, min);
	if(currentMenuState  == MENU_SET_ALARM)addElement(&alarms, year, month, date, hour, min);
	if(currentMenuState  == MENU_SET_REMINDER)addElement(&reminders, year, month, date, hour, min);


	displayMainMenu("Saved!");
    while (esc_flag == 0) {
    }
    esc_flag = 0;
    curr_mode_status = 0;
    currentMenuState = 0;
}
void update_menu() {
    switch(currentMenuState) {
        case MENU_ITEM_1:
        	UART_Print("-----------------MENU_ITEM_1\r\n");
        	if(last_menu_state != MENU_ITEM_1){
                displayMainMenu("Set new data/time");
        		last_menu_state = MENU_ITEM_1;
        	}
            break;
        case MENU_ITEM_2:
        	UART_Print("-----------------MENU_ITEM_2\r\n");
        	if(last_menu_state != MENU_ITEM_2){
                displayMainMenu("set alarm clock");
        		last_menu_state = MENU_ITEM_2;
        	}
            break;
        case MENU_ITEM_3:
        	UART_Print("-----------------MENU_ITEM_3\r\n");
        	if(last_menu_state != MENU_ITEM_3){
                displayMainMenu("set reminding");
        		last_menu_state = MENU_ITEM_3;
        	}
            break;


        case MENU_SET_TIME:
        	UART_Print("-----------------MENU_SET_TIME\r\n");
        	if(last_menu_state != MENU_SET_TIME){
        		show_empty_time();
        		last_menu_state = MENU_SET_TIME;
        	}
        	process_input_to_menu_item();
            break;
        case MENU_SET_ALARM:
        	UART_Print("-----------------MENU_SET_ALARM\r\n");
        	if(last_menu_state != MENU_SET_ALARM){
        		show_empty_time();
        		last_menu_state = MENU_SET_ALARM;
        	}
        	process_input_to_menu_item();
            break;
        case MENU_SET_REMINDER:
        	UART_Print("-----------------MENU_SET_REMINDER\r\n");
        	if(last_menu_state != MENU_SET_REMINDER){
        		show_empty_time();
        		last_menu_state = MENU_SET_REMINDER;
        	}
        	process_input_to_menu_item();
            break;

        default:
        case MENU_HOME:
        	//show_initial_screen();
        	if(last_menu_state != MENU_HOME){
        		show_initial_screen();
        		last_menu_state = MENU_HOME;
        	}
        	update_initial_screen();
        	display_action_icon(&reminders, 1, 0);
        	display_action_icon(&alarms, 1, 15);
            break;
    }
}

void changeMenuState(int direction) {
    if (direction > STAY) {
        if (currentMenuState == MAX_MENU_ITEM) {
            currentMenuState = MENU_ITEM_1;
        } else {
            currentMenuState++;
        }
    } else if (direction < STAY) {
        if (currentMenuState == MENU_ITEM_1) {
            currentMenuState = MAX_MENU_ITEM;
        } else {
            currentMenuState--;
        }
    }
}



void buttonSetInterruptHandler() {

	UART_Print("Set\r\n");
    if ((millis_counter - lastSetTime) > DEBOUNCE_THRESHOLD) {
		switch (currentMenuState) {
			case MENU_HOME:
				currentMenuState = MENU_ITEM_1;
				break;
			case MENU_ITEM_1:
				currentMenuState = MENU_SET_TIME;
				break;
			case MENU_ITEM_2:
				currentMenuState = MENU_SET_ALARM;
				break;
			case MENU_ITEM_3:
				currentMenuState = MENU_SET_REMINDER;
				break;
			default:
				curr_mode_status++;
				break;
		}
    lastSetTime = millis_counter;
}
}
void buttonESCInterruptHandler() {
	UART_Print("ESC\r\n");
    if ((millis_counter - lastESCTime) > DEBOUNCE_THRESHOLD) {
		if (currentMenuState <= MAX_MENU_ITEM && currentMenuState >= MENU_ITEM_1) {
			currentMenuState = MENU_HOME;
			return;
		}
		esc_flag = 1;
		switch (currentMenuState) {
			case MENU_SET_TIME:
				currentMenuState = MENU_ITEM_1;
				break;
			case MENU_SET_ALARM:
				currentMenuState = MENU_ITEM_2;
				break;
			case MENU_SET_REMINDER:
				currentMenuState = MENU_ITEM_3;
				break;
			default:
				break;
		}
    lastESCTime = millis_counter;
    }
}
void buttonLeftInterruptHandler() {
	UART_Print("Left\r\n");
    if ((millis_counter - lastLeftTime) > DEBOUNCE_THRESHOLD) {
		if (currentMenuState >= MENU_ITEM_1 && currentMenuState <= MAX_MENU_ITEM) {
			changeMenuState(LEFT);
		}
		if (currentMenuState >= MENU_SET_TIME && currentMenuState <= MENU_SET_REMINDER) {
			curr_mode_value--;
		}
    lastLeftTime = millis_counter;
    }
}

void buttonRightInterruptHandler() {
	UART_Print("Right\r\n");
    if ((millis_counter - lastRightTime) > DEBOUNCE_THRESHOLD) {
		if (currentMenuState >= MENU_ITEM_1 && currentMenuState <= MAX_MENU_ITEM) {
			changeMenuState(RIGHT);
		}
		if (currentMenuState >= MENU_SET_TIME && currentMenuState <= MENU_SET_REMINDER) {
			curr_mode_value++;
		}
    lastRightTime = millis_counter;
    }
}
void initializeDynamicArray(DynamicArray *array, size_t initialCapacity) {
    array->data = (TimeStamp *)malloc(initialCapacity * sizeof(TimeStamp));
    if (array->data == NULL) {
        exit(1);
    }
    array->size = 0;
    array->capacity = initialCapacity;
}

void reallocateMemory(DynamicArray *array) {
    size_t newCapacity = array->capacity * 2;
    TimeStamp *newData = (TimeStamp *)realloc(array->data, newCapacity * sizeof(TimeStamp));
    if (newData == NULL) {
        exit(1);
    }
    array->data = newData;
    array->capacity = newCapacity;
}

void addElement(DynamicArray *array, int year, int month, int date, int hour, int min) {
    if (array->size >= (array->capacity * 0.8)) {
        reallocateMemory(array);
    }
    TimeStamp newElement = {
        .year = year,
        .month = month,
        .date = date,
        .hour = hour,
        .min = min
    };
    array->data[array->size] = newElement;
    array->size++;
}

char isTimeStampsEqual(TimeStamp a, TimeStamp b) {
    return a.year == b.year &&
           a.month == b.month &&
           a.date == b.date &&
           a.hour == b.hour &&
           a.min == b.min;
}
int check_action_for_today(DynamicArray *array, RTC_TimeTypeDef currTime, RTC_DateTypeDef currDate) {
    for (size_t i = 0; i < array->size; ++i) {
        TimeStamp current = array->data[i];
        if (current.date == currDate.Date &&
            current.month == currDate.Month &&
            current.year ==  currDate.Year) {
            return 1;
        }
    }
    return 0;
}

void removeElement(DynamicArray *array, TimeStamp element) {
    for (size_t i = 0; i < array->size; ++i) {
        if (isTimeStampsEqual(array->data[i], element)) {
            for (size_t j = i; j < array->size - 1; ++j) {
                array->data[j] = array->data[j + 1];
            }
            array->size--;
            break;
        }
    }
}
int elementExists(DynamicArray *array, TimeStamp element) {
    for (size_t i = 0; i < array->size; ++i) {
        TimeStamp current = array->data[i];
        if (current.min == element.min &&
            current.hour == element.hour &&
            current.date == element.date &&
            current.month == element.month &&
            current.year == element.year) {
            return 1;
        }
    }
    return 0;
}

void display_action_icon(DynamicArray *array, int y, int x) {
    char num_str[2];

    if (check_action_for_today(array, currTime, currDate)) {
        num_str[0] = '!';
    } else {
        num_str[0] = ' ';
    }
    num_str[1] = '\0';
    lcd_put_cur(y, x);
    delay_TIM3(2);
    lcd_send_string(num_str);
    delay_TIM3(2);
}
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void start_reminder_signal() {
    UART_Print_Num("start_reminder_signal start_reminder_signal: ", 1);
}
void start_alarm_signal() {
    UART_Print_Num("start_alarm_signal start_alarm_signal: ", 1);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
