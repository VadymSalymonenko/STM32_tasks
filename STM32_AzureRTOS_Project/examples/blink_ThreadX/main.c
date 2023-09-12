#include <stm32g0xx_hal.h>
#include <tx_api.h>
#include <string.h>  

#define LED_PORT                GPIOA
#define LED_PIN                 GPIO_PIN_5
#define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOA_CLK_ENABLE

// ThreadX resources
TX_THREAD  my_thread;
UINT      my_thread_stack[512];

// UART Handle
UART_HandleTypeDef huart;

// Function prototypes
void initGPIO(void);
void initUART(void);
void UARTPrint(const char *str);
void blink_led(ULONG thread_input);

int main(void)
{
    // Hardware and peripheral initialization
    HAL_Init();
    initGPIO();
    initUART();
    
    UARTPrint("Main Function Start\r\n");

    // Real-Time OS initialization
    tx_kernel_enter();
    
    UARTPrint("Should never reach here\r\n");

    // Control should never reach here
    return 0;
}

void tx_application_define(void *first_unused_memory)
{
    UARTPrint("Defining ThreadX Application\r\n");
    
    // Create the main thread.
    tx_thread_create(&my_thread, "blink_led_thread", blink_led, 0,
                     my_thread_stack, sizeof(my_thread_stack),
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}

void initGPIO()
{
    GPIO_InitTypeDef GPIO_Config;

    // GPIO configuration
    GPIO_Config.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Config.Pin = LED_PIN;

    // Enable the GPIO Clock and initialize GPIO
    LED_PORT_CLK_ENABLE();
    HAL_GPIO_Init(LED_PORT, &GPIO_Config);
    
    UARTPrint("GPIO Initialized\r\n");
}

void initUART()
{
    // Initialize UART here (This is just a template, actual initialization may vary)
    huart.Instance = USART2;
    huart.Init.BaudRate = 115200;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE;
    huart.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&huart);
    
    UARTPrint("UART Initialized\r\n");
}

void UARTPrint(const char *str)
{
    HAL_UART_Transmit(&huart, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void blink_led(ULONG thread_input)
{
    UARTPrint("Blink LED thread started\r\n");
    
    while (1)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        UARTPrint("LED Toggled\r\n");
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
    }
}
