#include "MyGPIO.h"
#include "stm32g0xx_hal_conf.h"


#define MAX_TIMERS 10
MyTimerChannelInfo timerChannels[MAX_TIMERS] = {0};


int MyGPIO_ValidateParams(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    uint16_t validPinsA = 0b0011100011111111; // 0-7, 11-14
    uint16_t validPinsB = 0b0000000111111111; // 0-7, 8
    uint16_t validPinsC = 0b1100000000000000; // 14, 15

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
int MyGPIO_ValidateTimerParams(uint32_t timer, uint32_t channel) {
    // 
    return 0;
}

TIM_HandleTypeDef MyTimer_Init(MyTimerEnum timer) {
    TIM_HandleTypeDef htimx;
    TIM_TypeDef *instance = NULL; 

    switch (timer) {
        case MY_TIM1:
            instance = TIM1;
            break;
        case MY_TIM17:
            instance = TIM17;
            break;
        default:
            htimx.Instance = NULL; 
            return htimx;
    }

    htimx.Instance = instance;
    htimx.Init.Prescaler = (uint32_t)(SystemCoreClock / 1000000) - 1;
    htimx.Init.CounterMode = TIM_COUNTERMODE_UP;
    htimx.Init.Period = 0;
    htimx.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    return htimx; 
}

uint32_t MyTimer_GetChannel(MyTimerChannelEnum channel) {
    switch (channel) {
        case MY_TIM_CHANNEL1:
            return TIM_CHANNEL_1;
        case MY_TIM_CHANNEL2:
            return TIM_CHANNEL_2;
        case MY_TIM_CHANNEL3:
            return TIM_CHANNEL_3;
        case MY_TIM_CHANNEL4:
            return TIM_CHANNEL_4;
        default:
            return 0xFFFFFFFF;
    }
}


int MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, MyGPIOMode mode, MyGPIOPull pull, MyGPIOSpeed speed) {
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




void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM1)
  {
    __HAL_RCC_TIM1_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM14)
  {
    __HAL_RCC_TIM14_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM16)
  {
    __HAL_RCC_TIM16_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM17)
  {
    __HAL_RCC_TIM17_CLK_ENABLE();
  }
}



void Universal_HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_Alternate)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  if(GPIOx == GPIOA) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  }
  else if(GPIOx == GPIOB) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  
  GPIO_InitStruct.Pin = GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_Alternate;
  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}


int Universal_PWM_Init(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t pulse, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_Alternate)
{
    
  int result = HAL_TIM_PWM_Start(htim, channel);
  if(result != 0)
  {
    return result;
  }

  return 0;
}
typedef struct {
	TIM_HandleTypeDef *htim;
	uint64_t channel;
	uint32_t prescaler;
	uint32_t pulse;
	uint32_t period;
	TIM_TypeDef* instance;
}MX_TIM1_Params;

int MX_TIM1_Init_test(MX_TIM1_Params test)
{
  
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  test.htim->Instance = test.instance;
  test.htim->Init.Prescaler = test.prescaler;
  test.htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  test.htim->Init.Period = test.period;
  test.htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  test.htim->Init.RepetitionCounter = 0;
  test.htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(test.htim) != HAL_OK)
  {
    return -10;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(test.htim, &sClockSourceConfig) != HAL_OK)
  {
    return -11;
  }
  if (HAL_TIM_PWM_Init(test.htim) != HAL_OK)
  {
    return -12;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(test.htim, &sMasterConfig) != HAL_OK)
  {
    return -13;
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = test.pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(test.htim, &sConfigOC, test.channel) != HAL_OK)
  {
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
  if (HAL_TIMEx_ConfigBreakDeadTime(test.htim, &sBreakDeadTimeConfig) != HAL_OK)
  {
    return -15;
  }
  Universal_HAL_TIM_MspPostInit(test.htim, GPIOA, GPIO_PIN_8, GPIO_AF2_TIM1);
  return test.pulse;
}


int Universal_TIM_Init(TIM_HandleTypeDef *htim, uint64_t channel, uint32_t prescaler, uint32_t pulse, uint32_t period, TIM_TypeDef* instance, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
  
	htim->Instance = instance;
    htim->Init.Prescaler = prescaler;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = period;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(htim) != HAL_OK)
    {
        return -10;
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK)
    {
        return -11;
    }

    if (HAL_TIM_PWM_Init(htim) != HAL_OK)
    {
        return -12;
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
    {
        return -13;
    }

    Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF1_TIM3);
    
	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = pulse;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel) != HAL_OK)
	{
		return -14;
    }
    return 0;
}

int MX_TIM14_Init(TIM_HandleTypeDef *htim, uint64_t channel, uint32_t prescaler, uint32_t pulse, uint32_t period, TIM_TypeDef* instance, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{

  TIM_OC_InitTypeDef sConfigOC = {0};

  htim->Instance = instance;
  htim->Init.Prescaler = prescaler;
  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  htim->Init.Period = period;
  htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(htim) != HAL_OK)
  {
    return -10;
  }
  if (HAL_TIM_PWM_Init(htim) != HAL_OK)
  {
    return -11;
  }
  Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF4_TIM14);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    return -11;
  }
  return 0;
}

int MX_TIM16_Init(TIM_HandleTypeDef *htim, uint64_t channel, uint32_t prescaler, uint32_t pulse, uint32_t period, TIM_TypeDef* instance, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim->Instance = instance;
  htim->Init.Prescaler = prescaler;
  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  htim->Init.Period = period;
  htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim->Init.RepetitionCounter = 0;
  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(htim) != HAL_OK)
  {
    return -10;
  }
  if (HAL_TIM_PWM_Init(htim) != HAL_OK)
  {
    return -11;
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel) != HAL_OK)
  {
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
  if (HAL_TIMEx_ConfigBreakDeadTime(htim, &sBreakDeadTimeConfig) != HAL_OK)
  {
    return -13;
  }
    Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF5_TIM16);
  return 0;
}

int MX_TIM17_Init(TIM_HandleTypeDef *htim, uint64_t channel, uint32_t prescaler, uint32_t pulse, 
                  uint32_t period, TIM_TypeDef* instance, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim->Instance = instance;
  htim->Init.Prescaler = prescaler;
  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  htim->Init.Period = period;
  htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim->Init.RepetitionCounter = 0;
  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(htim) != HAL_OK)
  {
    return -10;
  }
  if (HAL_TIM_PWM_Init(htim) != HAL_OK)
  {
    return -11;
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel) != HAL_OK)
  {
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
  if (HAL_TIMEx_ConfigBreakDeadTime(htim, &sBreakDeadTimeConfig) != HAL_OK)
  {
    return -13;
  }
  Universal_HAL_TIM_MspPostInit(htim, GPIOx, GPIO_Pin, GPIO_AF5_TIM17);
  return 0;
}



int PWM_Init(TIM_HandleTypeDef *htim, uint32_t channel)
{    
  int result = 0;
  result = HAL_TIM_PWM_Start(htim, channel);
  return result;
}

int MyGPIO_PWM_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_TypeDef *timerInstance, uint32_t new_channel, uint32_t frequency, uint32_t dutyCycle)
{
	
	TIM_HandleTypeDef htim;
		
    uint32_t mcuClockFrequency = HAL_RCC_GetSysClockFreq();
    uint32_t counterPeriod = 255; 
    uint32_t prescaler = mcuClockFrequency / (frequency * counterPeriod);
    
  //  if (Universal_TIM_Init(&htim, ((uint64_t) TIM_CHANNEL_1), prescaler, dutyCycle, counterPeriod, timerInstance, GPIOx, GPIO_Pin))
   //     return -1;
   // if (MX_TIM14_Init(&htim, ((uint32_t) TIM_CHANNEL_1), prescaler, dutyCycle, counterPeriod, timerInstance, GPIOx, GPIO_Pin))
  //      return -1;
  //  if (MX_TIM16_Init(&htim, 0, prescaler, dutyCycle, counterPeriod, timerInstance, GPIOx, GPIO_Pin))
   //     return -1;
   // if (MX_TIM17_Init(&htim, 0, prescaler, dutyCycle, counterPeriod, timerInstance, GPIOx, GPIO_Pin))
   //     return -1;
   MX_TIM1_Params new;
   new.htim = &htim;
   new.channel = 0;
   new.prescaler = prescaler;
   new.pulse = dutyCycle;
   new.period = counterPeriod;
   new.instance = timerInstance;
   
   int resylt11 = MX_TIM1_Init_test(new);
    if (PWM_Init(&htim, ((uint32_t) TIM_CHANNEL_1)) != 0)
        return -2;

   // if (Universal_PWM_Init(&htim, ((uint32_t) TIM_CHANNEL_1) , dutyCycle, GPIOx, GPIO_Pin, GPIO_AF1_TIM3) != 0)
   //     return -2;

    return resylt11;
}



