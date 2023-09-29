#ifndef TEST
#include "stm32g0xx_hal.h"
#endif

#include "stm32g0xx_hal.h"

typedef enum {
    MY_GPIO_INPUT,
    MY_GPIO_OUTPUT,
    MY_GPIO_AF,
    MY_GPIO_ANALOG
} MyGPIOMode;

typedef enum { MY_GPIO_NOPULL, MY_GPIO_PULLUP, MY_GPIO_PULLDOWN } MyGPIOPull;

typedef enum {
    MY_GPIO_SPEED_LOW,
    MY_GPIO_SPEED_MEDIUM,
    MY_GPIO_SPEED_HIGH
} MyGPIOSpeed;

typedef struct {
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    TIM_TypeDef *timerInstance;
    uint32_t channel;
} PWM_Config;

#define bool char
#define true 1
#define false 0

int validate_timer_config(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                          TIM_TypeDef *timerInstance, uint32_t channel);

#ifndef TEST
int MyGPIO_PWM_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                    TIM_TypeDef *timerInstance, uint32_t new_channel,
                    uint32_t frequency, uint32_t dutyCycle, uint32_t GPIO_AF);
int MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, MyGPIOMode mode,
                MyGPIOPull pull, MyGPIOSpeed speed);
#endif
