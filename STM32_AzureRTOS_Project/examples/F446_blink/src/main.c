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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart4;
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
#define MAX_THREAD_POOL 20
#define PRIORITY_RANGE 32


TX_THREAD led_blink_thread;
TX_THREAD math_thread_1;
TX_THREAD math_thread_2;
TX_THREAD math_thread_3;
TX_THREAD math_thread_4;
TX_THREAD math_thread_5;
TX_THREAD math_thread_6;
TX_THREAD math_thread_7;
TX_THREAD scheduler_thread;

TX_THREAD* threads[] = {&math_thread_1, &math_thread_2, &math_thread_3, &math_thread_4, &math_thread_5, &math_thread_6, &math_thread_7};

UCHAR                   led_blink_thread_stack[1024];
UCHAR                   math_thread_1_stack[1024];
UCHAR                   math_thread_2_stack[1024];
UCHAR                   math_thread_3_stack[1024];
UCHAR                   math_thread_4_stack[1024];
UCHAR                   math_thread_5_stack[1024];
UCHAR                   math_thread_6_stack[1024];
UCHAR                   math_thread_7_stack[1024];

UCHAR                   scheduler_thread_stack[1024];

unsigned long counters_array[7] = {0};
unsigned long last_counters_array[7] = {0};

UINT App_ThreadX_Init(VOID *memory_ptr);
void MX_ThreadX_Init(void);
void math_thread_1_entry(ULONG thread_input);
void math_thread_2_entry(ULONG thread_input);

void math_thread_3_entry(ULONG thread_input);
void math_thread_4_entry(ULONG thread_input);
void math_thread_5_entry(ULONG thread_input);
void math_thread_6_entry(ULONG thread_input);
void math_thread_7_entry(ULONG thread_input);

void print_counter();
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);

/* USER CODE BEGIN PFP */

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
        ulong_to_str((unsigned long)test, buffer);
       // UART_Print("TX_TIMER_TICKS_PER_SECOND: ");
       // UART_Print(buffer);
       // UART_Print("\r\n");

        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 1); // Затримка на 250 мілісекунд
        test++;
        counter_value++; // Інкремент лічильника
      //  UART_Print("LED BBB\r\n");
        print_counter(counter_value);
    }

}

void math_thread_1_entry(ULONG thread_input) {
    float result;
    float array[100];
    while (1) {
        for (int i = 0; i < 100; ++i) {
            result = (float)i * 2.5 / 1.5;
            array[i] = result;
        }
        counters_array[0]++;
        tx_thread_relinquish();
    }
}

void math_thread_2_entry(ULONG thread_input) {

    float result;
    float array[100];
    while (1) {
        for (int i = 0; i < 100; ++i) {
            //result = (float)i / 3.7 * 0.4 *1.2 * 1.131;
            result = (float)i * 2.5 / 1.5;

            array[i] = result;
        }

        counters_array[1]++;
        tx_thread_relinquish();

    }

}
void math_thread_3_entry(ULONG thread_input) {
    float result;
    float array[100];
    while (1) {
        for (int i = 0; i < 100; ++i) {
            result = (float)i * 2.5 / 1.5;
            array[i] = result;
        }
        counters_array[2]++;
        tx_thread_relinquish();
    }
}
void math_thread_4_entry(ULONG thread_input) {
    float result;
    float array[100];
    while (1) {
        for (int i = 0; i < 100; ++i) {
            result = (float)i * 2.5 / 1.5;
            array[i] = result;
        }
        counters_array[3]++;
        tx_thread_relinquish();
    }
}
void math_thread_5_entry(ULONG thread_input) {
    float result;
    float array[100];
    while (1) {
        for (int i = 0; i < 100; ++i) {
            result = (float)i * 2.5 / 1.5;
            array[i] = result;
        }
        counters_array[4]++;
        tx_thread_relinquish();
    }
}
void math_thread_6_entry(ULONG thread_input) {
    float result;
    float array[100];
    while (1) {
        for (int i = 0; i < 100; ++i) {
            result = (float)i * 2.5 / 1.5;
            array[i] = result;
        }
        counters_array[5]++;
        tx_thread_relinquish();
    }
}
void math_thread_7_entry(ULONG thread_input) {
    float result;
    float array[100];
    while (1) {
        for (int i = 0; i < 100; ++i) {
            result = (float)i * 2.5 / 1.5;
            array[i] = result;
        }
        counters_array[6]++;
        tx_thread_relinquish();
    }
}
void print_counter() {
    char buffer[50];
    int thread_count = sizeof(threads) / sizeof(threads[0]);

	int new_steps_count[MAX_THREAD_POOL];
    int total_sum = 0;
    for(int i = 0; i < thread_count; i++){
    	new_steps_count[i] = counters_array[i] - last_counters_array[i];
    	last_counters_array[i] = counters_array[i];
    	total_sum += new_steps_count[i];
    }
    for(int i = 0; i < thread_count; i++){
        ulong_to_str(  (new_steps_count[i]*100)/total_sum, buffer);
        UART_Print_Num("Thread Operations Count number ", i);
        UART_Print(buffer);
        UART_Print("\r\n");
    }

}


void SetThreadPriorities(TX_THREAD *threadArray[], UINT arraySize, UINT highPriorityIndex) {
    int test1 = 31;
	UINT i;
	UINT status;
    for (i = 0; i < arraySize; ++i) {
        if (i == highPriorityIndex) {
        	status = tx_thread_priority_change(threadArray[i], 2, &test1);
        } else {
        	status = tx_thread_priority_change(threadArray[i], 31, &test1);
        }
        if (status != TX_SUCCESS) {
        	UART_Print_Num("error ----------------------- ", status);
        }
    }
}

int SelectThread(float* probabilities, int thread_count) {
    rand();
    float random_num = (float)rand() / (float)RAND_MAX;

    float accumulator = 0.0;
    for (int i = 0; i < thread_count; ++i) {
        accumulator += probabilities[i];
        if (random_num <= accumulator) {
            return i;
        }
    }
    return -1;
}
/*
float* CalculateProbabilities(TX_THREAD** threads, int thread_count) {
    static float probabilities[MAX_THREAD_POOL];
    int total_time_quantums_count = 1;
    for (int i = 0; i < thread_count; i++) {
    	//total_time_quantums_count += MAX_PRIORITY - threads[i].get_priority();
    }
    for (int i = 0; i < thread_count; i++) {
    	//probabilities[i] = (float)( ( ((MAX_PRIORITY - threads[i].get_priority()))*100)/  total_time_quantums_count)/100;
    	if(probabilities[i] > 1) probabilities[i] = 1;
    }
    return probabilities;
}
*/
void PrintProbabilities(float probabilities[], int count) {
    char index_str[10];
    char buffer[50];
    for (int i = 0; i < count; ++i) {
    	ulong_to_str(i, index_str);
        strcpy(buffer, "probabilities[");
        strcat(buffer, index_str);
        strcat(buffer, "] ");
        UART_Print_Num(buffer, probabilities[i] * 100);
    }
}

float* CalculateProbabilities(TX_THREAD* threads[], int thread_count) {
    static float probabilities[MAX_THREAD_POOL];
    int total_time_quantums_count = 0; // Змінимо на 0, оскільки ми будемо додавати до нього

    for (int i = 0; i < thread_count; i++) {
        TX_THREAD *current_thread = threads[i];
        UINT current_priority;
        tx_thread_info_get(current_thread, NULL, NULL, NULL, &current_priority, NULL, NULL, NULL, NULL);
        total_time_quantums_count += PRIORITY_RANGE - current_priority;

    }

    for (int i = 0; i < thread_count; i++) {
        TX_THREAD *current_thread = threads[i];
        UINT current_priority;
        tx_thread_info_get(current_thread, NULL, NULL, NULL, &current_priority, NULL, NULL, NULL, NULL);
        probabilities[i] = (float)(((PRIORITY_RANGE - current_priority) * 100) / total_time_quantums_count)/100;
        if (probabilities[i] > 1) probabilities[i] = 1;

    }
    PrintProbabilities(probabilities, thread_count);
    UART_Print_Num("total_time_quantums_count ", total_time_quantums_count);

    return probabilities;
}

void scheduler_thread_entry(ULONG thread_input) {
	int thread_count = sizeof(threads) / sizeof(threads[0]);
	float* probabilities = CalculateProbabilities(threads, thread_count);
	while (1) {
		//UART_Print_Num("probabilities[0]", (float)probabilities[0]*100);

		int selected_thread_index = SelectThread(probabilities, thread_count);

		//UART_Print_Num("selected_thread_index", selected_thread_index);

		//selected_thread_index = 1;

		SetThreadPriorities(threads, thread_count, selected_thread_index);
		tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND/100);
	}
}

void tx_application_define(void *first_unused_memory){


    tx_thread_create(&led_blink_thread, "LED Blink Thread",
                    led_blink_thread_entry, 0,
                    led_blink_thread_stack, sizeof(led_blink_thread_stack),
                    1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&math_thread_1, "Math Thread 1",
                    math_thread_1_entry, 0,
                    math_thread_1_stack, sizeof(math_thread_1_stack),
                    5, 5, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&math_thread_2, "Math Thread 2",
                    math_thread_2_entry, 0,
                    math_thread_2_stack, sizeof(math_thread_2_stack),
                    10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&math_thread_3, "Math Thread 3",
                    math_thread_3_entry, 0,
                    math_thread_3_stack, sizeof(math_thread_3_stack),
                    5, 5, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&math_thread_4, "Math Thread 4",
                    math_thread_4_entry, 0,
                    math_thread_4_stack, sizeof(math_thread_4_stack),
                    5, 5, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&math_thread_5, "Math Thread 5",
                    math_thread_5_entry, 0,
                    math_thread_5_stack, sizeof(math_thread_5_stack),
                    30, 30, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&math_thread_6, "Math Thread 6",
                    math_thread_6_entry, 0,
                    math_thread_6_stack, sizeof(math_thread_6_stack),
                    10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&math_thread_7, "Math Thread 7",
                    math_thread_7_entry, 0,
                    math_thread_7_stack, sizeof(math_thread_7_stack),
                    30, 30, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_thread_create(&scheduler_thread, "Scheduler Thread",
    		        scheduler_thread_entry, 0,
					scheduler_thread_stack, sizeof(scheduler_thread_stack),
                    1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

}


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* USER CODE BEGIN 2 */

  tx_kernel_enter();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	   // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // Зміна піну на PA5
	   // HAL_Delay(1000);  // Затримка на 100 мілісекунд
	   // UART_Print("LED toggled\r\n");  // Вивід тексту через UART
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
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
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 1599;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
  sSlaveConfig.InputTrigger = TIM_TS_ITR1;
  if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
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
void TIM1_IRQHandler()
{
        UART_Print("TIM1_IRQHandler--------------- \r\n");
  
}


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
