

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

typedef struct
{
    uint32_t MODER;    /* GPIO port mode register,               Address offset: 0x00      */
    uint32_t OTYPER;   /* GPIO port output type register,        Address offset: 0x04      */
    uint32_t OSPEEDR;  /* GPIO port output speed register,       Address offset: 0x08      */
    uint32_t PUPDR;    /* GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    uint32_t IDR;      /* GPIO port input data register,         Address offset: 0x10      */
    uint32_t ODR;      /* GPIO port output data register,        Address offset: 0x14      */
    uint32_t BSRR;     /* GPIO port bit set/reset register,      Address offset: 0x18      */
    uint32_t LCKR;     /* GPIO port configuration lock register, Address offset: 0x1C      */
    uint32_t AFR[2];   /* GPIO alternate function registers,     Address offset: 0x20-0x24 */
    uint32_t BRR;      /* GPIO Bit Reset register,               Address offset: 0x28 */
} GPIO_TypeDef;

// Mock TIM_TypeDef structure
typedef struct
{
    uint32_t CR1;      /* TIM control register 1,              Address offset: 0x00 */
    uint32_t CR2;      /* TIM control register 2,              Address offset: 0x04 */
    uint32_t SMCR;     /* TIM slave Mode Control register,     Address offset: 0x08 */
    uint32_t DIER;     /* TIM DMA/interrupt enable register,   Address offset: 0x0C */
    uint32_t SR;       /* TIM status register,                 Address offset: 0x10 */
    uint32_t EGR;      /* TIM event generation register,       Address offset: 0x14 */
    uint32_t CCMR1;    /* TIM  capture/compare mode register 1,Address offset: 0x18 */
    uint32_t CCMR2;    /* TIM  capture/compare mode register 2,Address offset: 0x1C */
    uint32_t CCER;     /* TIM capture/compare enable register, Address offset: 0x20 */
    uint32_t CNT;      /* TIM counter register,                Address offset: 0x24 */
    uint32_t PSC;      /* TIM prescaler,                       Address offset: 0x28 */
    uint32_t ARR;      /* TIM auto-reload register,            Address offset: 0x2C */
    uint32_t RCR;      /* TIM  repetition counter register,    Address offset: 0x30 */
    uint32_t CCR1;     /* TIM capture/compare register 1,      Address offset: 0x34 */
    uint32_t CCR2;     /* TIM capture/compare register 2,      Address offset: 0x38 */
    uint32_t CCR3;     /* TIM capture/compare register 3,      Address offset: 0x3C */
    uint32_t CCR4;     /* TIM capture/compare register 4,      Address offset: 0x40 */
    uint32_t BDTR;     /* TIM break and dead-time register,    Address offset: 0x44 */
    uint32_t DCR;      /* TIM DMA control register,            Address offset: 0x48 */
    uint32_t DMAR;     /* TIM DMA address for full transfer,   Address offset: 0x4C */
} TIM_TypeDef;




typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
} GPIO_list;

typedef enum {
    TIM1 = 1,
    TIM3,
    TIM14,
    TIM16,
    TIM17,
} Timers_list;

typedef enum {
    TIM_CHANNEL_1 = 1,
    TIM_CHANNEL_2,
    TIM_CHANNEL_3,
    TIM_CHANNEL_4,
} TIM_Channels_list;

typedef enum {
    GPIOA = 0,
    GPIOB,
    GPIOC,
} GPIO_type_list;

