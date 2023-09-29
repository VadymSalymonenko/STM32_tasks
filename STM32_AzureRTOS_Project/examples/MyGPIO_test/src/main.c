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

#include "MyGPIO.h"

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
UART_HandleTypeDef huart2;
void UART_Print(const char *str);
void UART_Print_value(const char *label, unsigned long value);
/* USER CODE BEGIN PV */
TX_THREAD led_blink_thread;
UCHAR led_blink_thread_stack[1024];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */

void ulong_to_str(unsigned long n, char *str) {
    int i = 0, j = 0;
    char temp[20];

    do {
        temp[i++] = (n % 10) + '0';
        n = n / 10;
    } while (n);

    while (i--) {
        str[j++] = temp[i];
    }
    str[j] = '\0';
}
void UART_Print(const char *str) {
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

void UART_Print_value(const char *label, unsigned long value) {
    char valueStr[21];
    char combinedStr[50];
    int i = 0, j = 0;

    ulong_to_str(value, valueStr);

    while (label[i] != '\0') {
        combinedStr[i] = label[i];
        i++;
    }

    while (valueStr[j] != '\0') {
        combinedStr[i] = valueStr[j];
        i++;
        j++;
    }

    combinedStr[i] = '\0';
    UART_Print(combinedStr);
}

void led_blink_thread_entry(ULONG thread_input) {
    char buttonState;
    char lastButtonState = GPIO_PIN_SET;
    char valueStr[10];

    while (1) {
        buttonState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

        ulong_to_str(GPIO_PIN_6, valueStr);
        //   UART_Print("GPIO_PIN_5 Value: ");
        //   UART_Print(valueStr);
        //  UART_Print("\r\n");

        if (buttonState == GPIO_PIN_RESET && lastButtonState == GPIO_PIN_SET) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            UART_Print("LED toggled due to button press\r\n");
        }

        lastButtonState = buttonState;
        // tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
    }
}

void tx_application_define(void *first_unused_memory) {
    tx_thread_create(&led_blink_thread, "LED Blink Thread",
                     led_blink_thread_entry, 0, led_blink_thread_stack,
                     sizeof(led_blink_thread_stack), 1, 1, TX_NO_TIME_SLICE,
                     TX_AUTO_START);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

    MyGPIO_Init(GPIOA, GPIO_PIN_0, MY_GPIO_INPUT, MY_GPIO_PULLUP,
                MY_GPIO_SPEED_LOW);
    MyGPIO_Init(GPIOA, GPIO_PIN_5, MY_GPIO_OUTPUT, MY_GPIO_NOPULL,
                MY_GPIO_SPEED_LOW);

    MX_USART2_UART_Init();

    UART_Print("start\r\n");

    uint32_t frequency = 24;
    uint32_t dutyCycle = 180;  // 0-255
    char valueStr[20];

    int result_PWM = MyGPIO_PWM_Init(GPIOA, GPIO_PIN_6, TIM16, TIM_CHANNEL_1,
                                     frequency, dutyCycle, GPIO_AF5_TIM16);
    //    int result_PWM = MyGPIO_PWM_Init(GPIOB, GPIO_PIN_8, TIM16,
    //    TIM_CHANNEL_1, frequency, dutyCycle, GPIO_AF2_TIM16); int result_PWM =
    //    MyGPIO_PWM_Init(GPIOA, GPIO_PIN_7, TIM17, TIM_CHANNEL_1, frequency,
    //    dutyCycle, GPIO_AF5_TIM17); int result_PWM = MyGPIO_PWM_Init(GPIOB,
    //    GPIO_PIN_0, TIM3, TIM_CHANNEL_3, frequency, dutyCycle, GPIO_AF1_TIM3);
    //    int result_PWM = MyGPIO_PWM_Init(GPIOA, GPIO_PIN_6, TIM16,
    //    TIM_CHANNEL_1, frequency, dutyCycle, GPIO_AF2_TIM16); int result_PWM =
    //    MyGPIO_PWM_Init(GPIOA, GPIO_PIN_6, TIM16, TIM_CHANNEL_1, frequency,
    //    dutyCycle, GPIO_AF2_TIM16); int result_PWM = MyGPIO_PWM_Init(GPIOA,
    //    GPIO_PIN_6, TIM16, TIM_CHANNEL_1, frequency, dutyCycle,
    //    GPIO_AF2_TIM16); int result_PWM = MyGPIO_PWM_Init(GPIOA, GPIO_PIN_6,
    //    TIM16, TIM_CHANNEL_1, frequency, dutyCycle, GPIO_AF2_TIM16); int
    //    result_PWM = MyGPIO_PWM_Init(GPIOA, GPIO_PIN_6, TIM16, TIM_CHANNEL_1,
    //    frequency, dutyCycle, GPIO_AF2_TIM16); int result_PWM =
    //    MyGPIO_PWM_Init(GPIOA, GPIO_PIN_6, TIM16, TIM_CHANNEL_1, frequency,
    //    dutyCycle, GPIO_AF2_TIM16);

    ulong_to_str(result_PWM, valueStr);
    UART_Print("MyGPIO_PWM_Init result = ");
    UART_Print(valueStr);
    UART_Print("\r\n");

    UART_Print("start22222\r\n");

    /* USER CODE BEGIN 2 */

    tx_kernel_enter();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
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
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {
    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
