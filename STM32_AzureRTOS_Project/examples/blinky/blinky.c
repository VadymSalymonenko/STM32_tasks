#include <stm32g0xx_hal.h>
#include <string.h>

void UARTPrint(const char *str);

#define LED_PORT                GPIOA
#define LED_PIN                 GPIO_PIN_5
#define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOA_CLK_ENABLE

UART_HandleTypeDef huart;

void SysTick_Handler(void)
{
    HAL_IncTick();

    if ((HAL_GetTick() % 1000) == 0)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        // Print the LED status
        if (HAL_GPIO_ReadPin(LED_PORT, LED_PIN) == GPIO_PIN_SET)
        {
            UARTPrint("LED ON\r\n");
        }
        else
        {
            UARTPrint("LED OFF\r\n");
        }
    }
}

void initGPIO()
{
    GPIO_InitTypeDef GPIO_Config;
    GPIO_Config.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Config.Pin = LED_PIN;

    LED_PORT_CLK_ENABLE();
    HAL_GPIO_Init(LED_PORT, &GPIO_Config);
}

void initUART()
{
    __HAL_RCC_USART2_CLK_ENABLE();

    huart.Instance = USART2;
    huart.Init.BaudRate = 115200;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE;
    huart.Init.Mode = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;
}

void UARTPrint(const char *str)
{
    HAL_UART_Transmit(&huart, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

int main(void)
{
    HAL_Init();
    initGPIO();
    initUART();

    UARTPrint("Starting program\r\n");

    HAL_SYSTICK_Config(SystemCoreClock / 1000);

    while(1);

    return 0;
}
