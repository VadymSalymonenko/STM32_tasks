#ifdef TEST

#include "unity.h"
#include "MyGPIO.h"


void setUp(void)
{
}

void tearDown(void)
{
}

void test_Valid_Configurations(void)
{
    TEST_ASSERT_EQUAL(0, validate_timer_config((GPIO_TypeDef*)GPIOA, GPIO_PIN_6, (TIM_TypeDef*)TIM3, TIM_CHANNEL_1));
    TEST_ASSERT_EQUAL(0, validate_timer_config((GPIO_TypeDef*)GPIOA, GPIO_PIN_7, (TIM_TypeDef*)TIM3, TIM_CHANNEL_2));
    TEST_ASSERT_EQUAL(0, validate_timer_config((GPIO_TypeDef*)GPIOB, GPIO_PIN_0, (TIM_TypeDef*)TIM3, TIM_CHANNEL_3));
    TEST_ASSERT_EQUAL(0, validate_timer_config((GPIO_TypeDef*)GPIOB, GPIO_PIN_1, (TIM_TypeDef*)TIM14, TIM_CHANNEL_1));
    TEST_ASSERT_EQUAL(0, validate_timer_config((GPIO_TypeDef*)GPIOB, GPIO_PIN_1, (TIM_TypeDef*)TIM3, TIM_CHANNEL_4));
    TEST_ASSERT_EQUAL(0, validate_timer_config((GPIO_TypeDef*)GPIOB, GPIO_PIN_3, (TIM_TypeDef*)TIM1, TIM_CHANNEL_2));
    TEST_ASSERT_EQUAL(0, validate_timer_config((GPIO_TypeDef*)GPIOC, GPIO_PIN_6, (TIM_TypeDef*)TIM3, TIM_CHANNEL_1));
}

void test_Invalid_Configurations(void)
{
    TEST_ASSERT_EQUAL(-1, validate_timer_config((GPIO_TypeDef*)GPIOA, GPIO_PIN_6, (TIM_TypeDef*)TIM3, TIM_CHANNEL_3));
    TEST_ASSERT_EQUAL(-1, validate_timer_config((GPIO_TypeDef*)GPIOA, GPIO_PIN_7, (TIM_TypeDef*)TIM17, TIM_CHANNEL_2));
    TEST_ASSERT_EQUAL(-1, validate_timer_config((GPIO_TypeDef*)GPIOB, GPIO_PIN_0, (TIM_TypeDef*)TIM3, TIM_CHANNEL_4));
    TEST_ASSERT_EQUAL(-1, validate_timer_config((GPIO_TypeDef*)GPIOB, GPIO_PIN_1, (TIM_TypeDef*)TIM14, TIM_CHANNEL_2));
    TEST_ASSERT_EQUAL(-1, validate_timer_config((GPIO_TypeDef*)GPIOB, GPIO_PIN_1, (TIM_TypeDef*)TIM16, TIM_CHANNEL_4));
    TEST_ASSERT_EQUAL(-1, validate_timer_config((GPIO_TypeDef*)GPIOC, GPIO_PIN_6, (TIM_TypeDef*)TIM17, TIM_CHANNEL_1));
}

#endif 
