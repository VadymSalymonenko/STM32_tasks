#include "MyGPIO.h"

#ifndef TEST
#include "stm32g0xx_hal_conf.h"
#endif

#define MAX_TIMERS 10

const PWM_Config valid_timers_configs[] = {
    {GPIOA, GPIO_PIN_6, TIM3, TIM_CHANNEL_1},
    {GPIOA, GPIO_PIN_6, TIM16, TIM_CHANNEL_1},
    {GPIOA, GPIO_PIN_7, TIM3, TIM_CHANNEL_2},
    {GPIOA, GPIO_PIN_7, TIM17, TIM_CHANNEL_1},
    {GPIOB, GPIO_PIN_0, TIM3, TIM_CHANNEL_3},
    {GPIOB, GPIO_PIN_1, TIM14, TIM_CHANNEL_1},
    {GPIOB, GPIO_PIN_1, TIM3, TIM_CHANNEL_4},
    {GPIOB, GPIO_PIN_3, TIM1, TIM_CHANNEL_2},
    {GPIOB, GPIO_PIN_4, TIM3, TIM_CHANNEL_1},
    {GPIOB, GPIO_PIN_5, TIM3, TIM_CHANNEL_2},
    {GPIOB, GPIO_PIN_6, TIM1, TIM_CHANNEL_3},
    {GPIOB, GPIO_PIN_8, TIM16, TIM_CHANNEL_1},
    {GPIOB, GPIO_PIN_9, TIM17, TIM_CHANNEL_1},
    {GPIOA, GPIO_PIN_8, TIM1, TIM_CHANNEL_1},
    {GPIOA, GPIO_PIN_9, TIM1, TIM_CHANNEL_2},
    {GPIOA, GPIO_PIN_10, TIM1, TIM_CHANNEL_3},
    {GPIOA, GPIO_PIN_11, TIM1, TIM_CHANNEL_4},
    {GPIOC, GPIO_PIN_6, TIM3, TIM_CHANNEL_1},
    {GPIOC, GPIO_PIN_7, TIM3, TIM_CHANNEL_2}};

static uint32_t timer_channel = 0;

#ifndef TEST
int MyGPIO_ValidateParams(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    uint16_t validPinsA = 0b0011100011111111;  // 0-7, 11-14
    uint16_t validPinsB = 0b0000000111111111;  // 0-7, 8
    uint16_t validPinsC = 0b1100000000000000;  // 14, 15

    if (GPIOx == GPIOA) {
        if ((GPIO_Pin & ~validPinsA) != 0) {
            return -1;
        }
    } else if (GPIOx == GPIOB) {
        if ((GPIO_Pin & ~validPinsB) != 0) {
            return -1;
        }
    } else if (GPIOx == GPIOC) {
        if ((GPIO_Pin & ~validPinsC) != 0) {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

int MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, MyGPIOMode mode,
                MyGPIOPull pull, MyGPIOSpeed speed) {
    if (MyGPIO_ValidateParams(GPIOx, GPIO_Pin) == -1) {
        return -1;
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Configure GPIO pin */
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pull;
    GPIO_InitStruct.Speed = speed;

    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

    return 0;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

TIM_HandleTypeDef htim16;

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base) {
    if (htim_base->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
    if (htim_base->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    if (htim_base->Instance == TIM14) {
        __HAL_RCC_TIM14_CLK_ENABLE();
    }
    if (htim_base->Instance == TIM16) {
        __HAL_RCC_TIM16_CLK_ENABLE();
    }
    if (htim_base->Instance == TIM17) {
        __HAL_RCC_TIM17_CLK_ENABLE();
    }
}

void Universal_HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim, GPIO_TypeDef *GPIOx,
                                   uint16_t GPIO_Pin, uint8_t GPIO_Alternate) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (GPIOx == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (GPIOx == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (GPIOx == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    } else if (GPIOx == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }

    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_Alternate;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

int MX_TIM1_Init(TIM_HandleTypeDef *htim, uint32_t prescaler, uint32_t pulse,
                 uint32_t period, TIM_TypeDef *instance, GPIO_TypeDef *GPIOx,
                 uint16_t GPIO_Pin, uint32_t GPIO_AF) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim->Instance = instance;
    htim->Init.Prescaler = prescaler;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = period;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.RepetitionCounter = 0;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(htim) != HAL_OK) {
        return -10;
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK) {
        return -11;
    }
    if (HAL_TIM_PWM_Init(htim) != HAL_OK) {
        return -12;
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK) {
        return -13;
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, timer_channel) != HAL_OK) {
        return -14;
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(htim, &sBreakDeadTimeConfig) != HAL_OK) {
        return -15;
    }
    Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF);
    return -0;
}

int MX_TIM3_Init(TIM_HandleTypeDef *htim, uint32_t prescaler, uint32_t pulse,
                 uint32_t period, TIM_TypeDef *instance, GPIO_TypeDef *GPIOx,
                 uint16_t GPIO_Pin, uint32_t GPIO_AF) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim->Instance = instance;
    htim->Init.Prescaler = prescaler;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = period;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(htim) != HAL_OK) {
        return -10;
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK) {
        return -11;
    }

    if (HAL_TIM_PWM_Init(htim) != HAL_OK) {
        return -12;
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK) {
        return -13;
    }

    Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, timer_channel) != HAL_OK) {
        return -14;
    }
    return 0;
}

int MX_TIM14_Init(TIM_HandleTypeDef *htim, uint32_t prescaler, uint32_t pulse,
                  uint32_t period, TIM_TypeDef *instance, GPIO_TypeDef *GPIOx,
                  uint16_t GPIO_Pin, uint32_t GPIO_AF) {
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim->Instance = instance;
    htim->Init.Prescaler = prescaler;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = period;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(htim) != HAL_OK) {
        return -10;
    }
    if (HAL_TIM_PWM_Init(htim) != HAL_OK) {
        return -11;
    }
    Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, timer_channel) != HAL_OK) {
        return -12;
    }
    return 0;
}

int MX_TIM16_Init(TIM_HandleTypeDef *htim, uint32_t prescaler, uint32_t pulse,
                  uint32_t period, TIM_TypeDef *instance, GPIO_TypeDef *GPIOx,
                  uint16_t GPIO_Pin, uint32_t GPIO_AF) {
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim->Instance = instance;
    htim->Init.Prescaler = prescaler;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = period;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.RepetitionCounter = 0;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(htim) != HAL_OK) {
        return -10;
    }
    if (HAL_TIM_PWM_Init(htim) != HAL_OK) {
        return -11;
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, timer_channel) != HAL_OK) {
        return -12;
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(htim, &sBreakDeadTimeConfig) != HAL_OK) {
        return -13;
    }
    Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF);
    return 0;
}

int MX_TIM17_Init(TIM_HandleTypeDef *htim, uint32_t prescaler, uint32_t pulse,
                  uint32_t period, TIM_TypeDef *instance, GPIO_TypeDef *GPIOx,
                  uint16_t GPIO_Pin, uint32_t GPIO_AF) {
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim->Instance = instance;
    htim->Init.Prescaler = prescaler;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = period;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.RepetitionCounter = 0;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(htim) != HAL_OK) {
        return -10;
    }
    if (HAL_TIM_PWM_Init(htim) != HAL_OK) {
        return -11;
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, timer_channel) != HAL_OK) {
        return -12;
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(htim, &sBreakDeadTimeConfig) != HAL_OK) {
        return -13;
    }
    Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF);
    return 0;
}
#endif

int validate_timer_config(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                          TIM_TypeDef *timerInstance, uint32_t channel) {
    for (int i = 0;
         i < sizeof(valid_timers_configs) / sizeof(valid_timers_configs[0]);
         ++i) {
        if (valid_timers_configs[i].GPIOx == GPIOx &&
            valid_timers_configs[i].GPIO_Pin == GPIO_Pin &&
            valid_timers_configs[i].timerInstance == timerInstance &&
            valid_timers_configs[i].channel == channel) {
            return 0;
        }
    }
    return -1;
}

#ifndef TEST
int PWM_Init(TIM_HandleTypeDef *htim, uint32_t channel) {
    int result = 0;
    result = HAL_TIM_PWM_Start(htim, channel);
    return result;
}
int MyGPIO_PWM_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                    TIM_TypeDef *timerInstance, uint32_t new_channel,
                    uint32_t frequency, uint32_t dutyCycle, uint32_t GPIO_AF) {
    if (validate_timer_config(GPIOx, GPIO_Pin, timerInstance, new_channel) !=
        0) {
        return validate_timer_config(GPIOx, GPIO_Pin, timerInstance,
                                     new_channel);
    }

    TIM_HandleTypeDef htim;
    GPIO_TypeDef *GPIOx_temp;
    uint16_t GPIO_Pin_temp;

    (void)(GPIOx_temp);
    (void)(GPIO_Pin_temp);
    // GPIO_Pin_temp = ( (int) GPIOx_temp) + GPIO_Pin_temp;

    uint32_t mcuClockFrequency = HAL_RCC_GetSysClockFreq();
    uint32_t counterPeriod = 255;
    uint32_t prescaler = mcuClockFrequency / (frequency * counterPeriod);

    timer_channel = new_channel;
    if (timerInstance == TIM1) {
        MX_TIM1_Init(&htim, prescaler, dutyCycle, counterPeriod, timerInstance,
                     GPIOx, GPIO_Pin, GPIO_AF);
    } else if (timerInstance == TIM3) {
        MX_TIM3_Init(&htim, prescaler, dutyCycle, counterPeriod, timerInstance,
                     GPIOx, GPIO_Pin, GPIO_AF);
    } else if (timerInstance == TIM14) {
        MX_TIM14_Init(&htim, prescaler, dutyCycle, counterPeriod, timerInstance,
                      GPIOx, GPIO_Pin, GPIO_AF);
    } else if (timerInstance == TIM16) {
        MX_TIM16_Init(&htim, prescaler, dutyCycle, counterPeriod, timerInstance,
                      GPIOx, GPIO_Pin, GPIO_AF);
    } else if (timerInstance == TIM17) {
        MX_TIM17_Init(&htim, prescaler, dutyCycle, counterPeriod, timerInstance,
                      GPIOx, GPIO_Pin, GPIO_AF);
    } else {
        return -1;
    }

    if (PWM_Init(&htim, ((uint32_t)TIM_CHANNEL_1)) != 0) {
        return -1;
    }

    return 0;
}

#endif
