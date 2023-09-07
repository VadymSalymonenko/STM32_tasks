#include <stm32g0xx_hal.h>

// Your board STM32G030F6P6 led - PA3
#define LED_PORT                GPIOA
#define LED_PIN                 GPIO_PIN_3
#define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOA_CLK_ENABLE

void SysTick_Handler(void)
{
    HAL_IncTick();

    // 1 Hz blinking (1 second ON, 1 second OFF)
    if ((HAL_GetTick() % 500) == 0)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
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

int main(void)
{
    HAL_Init();
    initGPIO();
    // 1kHz ticks
    HAL_SYSTICK_Config(SystemCoreClock / 1000);

    while(1);

    return 0;
}
