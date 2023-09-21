#include "stm32g0xx_hal.h"

typedef enum {
    MY_GPIO_INPUT,
    MY_GPIO_OUTPUT,
    MY_GPIO_AF,
    MY_GPIO_ANALOG
} MyGPIOMode;

typedef enum {
    MY_GPIO_NOPULL,
    MY_GPIO_PULLUP,
    MY_GPIO_PULLDOWN
} MyGPIOPull;

typedef enum {
    MY_GPIO_SPEED_LOW,
    MY_GPIO_SPEED_MEDIUM,
    MY_GPIO_SPEED_HIGH
} MyGPIOSpeed;

typedef enum {
    MY_TIM1,
    MY_TIM17
} MyTimerEnum;

typedef struct {
    uint32_t timer;
    uint32_t channel;
    uint8_t inUse;
} MyTimerChannelInfo;

typedef enum {
    MY_TIM_CHANNEL1,
    MY_TIM_CHANNEL2,
    MY_TIM_CHANNEL3,
    MY_TIM_CHANNEL4
} MyTimerChannelEnum;

int MyGPIO_PWM_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_TypeDef *timerInstance, uint32_t new_channel, uint32_t frequency, uint32_t dutyCycle);
int MyGPIO_ValidateParams(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
int MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, MyGPIOMode mode, MyGPIOPull pull, MyGPIOSpeed speed);

