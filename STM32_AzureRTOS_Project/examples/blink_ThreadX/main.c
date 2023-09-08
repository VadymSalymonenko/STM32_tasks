#include <stm32g0xx_hal.h>
#include "tx_api.h"

#define LED_PORT                GPIOA
#define LED_PIN                 GPIO_PIN_3
#define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOA_CLK_ENABLE

TX_THREAD  my_thread;
UINT8      my_thread_stack[1024];

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

void blink_led(ULONG thread_input)
{
    while (1)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
    }
}

int main(void)
{
    // HAL initialization
    HAL_Init();
    initGPIO();

    // ThreadX initialization
    tx_kernel_enter();

    return 0;
}

void tx_application_define(void *first_unused_memory)
{
    // Create the main thread.
    tx_thread_create(&my_thread, "blink_led_thread", blink_led, 0,
                     my_thread_stack, sizeof(my_thread_stack),
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}
