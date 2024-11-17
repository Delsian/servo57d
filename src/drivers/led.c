#include "servo57.h"
#include <stdlib.h>

typedef enum {
    LED_OK_OFF,
    LED_OK_ON,
    LED_ERR
} led_state;

static led_state state = LED_OK_OFF;
#define DELAY_LONG   50000
#define DELAY_SHORT  1000

void led_init(void) {
    GPIO_InitType GPIO_InitStructure;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;

    /* Enable TIM2 clocks */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = LED1_PIN;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
    GPIO_InitStructure.GPIO_Pull    = GPIO_No_Pull;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(LED1_PORT, &GPIO_InitStructure);

    /* Compute the prescaler value */
    uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 120000) - 1;

    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);    
    TIM_TimeBaseStructure.Period    = DELAY_LONG;
    TIM_TimeBaseStructure.Prescaler = 0;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIM1, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_ConfigPrescaler(TIM1, PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);

    /* TIM1 enable update irq */
    TIM_ConfigInt(TIM1, TIM_INT_UPDATE, ENABLE);

    /* TIM1 enable counter */
    TIM_Enable(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetIntStatus(TIM1, TIM_INT_UPDATE) != RESET)
    {
        TIM_ClrIntPendingBit(TIM1, TIM_INT_UPDATE);

        switch (state) {
        case LED_OK_OFF:
            TIM1->AR = DELAY_LONG;
            state = LED_OK_ON;
            LedOff();
            break;
        case LED_OK_ON:
            TIM1->AR = DELAY_SHORT;
            state = LED_OK_OFF;
            LedOn();
            break;
        case LED_ERR:
            TIM1->AR = DELAY_SHORT;
            LedBlink();
            break;
        }
    }
}

void LedOn(void) {
    LED1_PORT->PBSC = LED1_PIN;
}

void LedOff(void) {
    LED1_PORT->PBC = LED1_PIN;
}

void LedBlink(void) {
    LED1_PORT->POD ^= LED1_PIN;
}