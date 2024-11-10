#include "board.h"
#include <stdlib.h>

static uint16_t speed_g = 2000;
uint16_t CCR1_Val       = 333;
uint16_t CCR2_Val       = 249;
uint16_t CCR3_Val       = 166;
uint16_t CCR4_Val       = 83;
static OCInitType TIM_OCInitStructure;

static const uint16_t step16[] = {
    0, 10, 20, 29, 38, 47, 56, 63, 71, 77, 83, 88, 92, 96, 98, 100
};
static uint16_t generated[32];

static void generate_table(void) {
    uint16_t k = speed_g/200;
    for (int i = 0; i<16; i++) {
        generated[i] = generated[31 - i] = step16[i] * k;
    }
}

void stepper_init(void) {
    GPIO_InitType GPIO_InitStructure;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = PHASE_A1_PIN | PHASE_A2_PIN;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2_TIM3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(PHASE_A_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin          = PHASE_B1_PIN | PHASE_B2_PIN;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2_TIM3;
    GPIO_InitPeripheral(PHASE_B_PORT, &GPIO_InitStructure);

    /* TIM3 configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);    
    TIM_TimeBaseStructure.Period    = speed_g;
    TIM_TimeBaseStructure.Prescaler = 1;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIM3, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TIM_InitOcStruct(&TIM_OCInitStructure);    
    TIM_OCInitStructure.OcMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR1_Val;
    TIM_OCInitStructure.OcPolarity  = TIM_OC_POLARITY_HIGH;
    TIM_InitOc1(TIM3, &TIM_OCInitStructure);
    TIM_ConfigOc1Preload(TIM3, TIM_OC_PRE_LOAD_ENABLE);

    /* PWM1 Mode configuration: Channel2 */
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR2_Val;
    TIM_InitOc2(TIM3, &TIM_OCInitStructure);
    TIM_ConfigOc2Preload(TIM3, TIM_OC_PRE_LOAD_ENABLE);

    /* PWM1 Mode configuration: Channel3 */
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR3_Val;
    TIM_InitOc3(TIM3, &TIM_OCInitStructure);
    TIM_ConfigOc3Preload(TIM3, TIM_OC_PRE_LOAD_ENABLE);

    /* PWM1 Mode configuration: Channel4 */
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR4_Val;
    TIM_InitOc4(TIM3, &TIM_OCInitStructure);
    TIM_ConfigOc4Preload(TIM3, TIM_OC_PRE_LOAD_ENABLE);

    TIM_ConfigArPreload(TIM3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ConfigInt(TIM3, TIM_INT_UPDATE, ENABLE);

    /* TIM3 enable counter */
    TIM_Enable(TIM3, ENABLE);

    generate_table();
}

void TIM3_IRQHandler(void) {
    static uint8_t idx = 0;
    TIM_ClrIntPendingBit(TIM3, TIM_INT_UPDATE);
    switch (idx&0x30) {
    case 0x00:
        TIM3->CCDAT1 = generated[idx & 0x1F];
        TIM3->CCDAT2 = 0;
        TIM3->CCDAT3 = generated[(idx+16) & 0x1F];
        TIM3->CCDAT4 = 0;
        break;
    case 0x10:
        TIM3->CCDAT1 = generated[idx & 0x1F];
        TIM3->CCDAT2 = 0;
        TIM3->CCDAT3 = 0;
        TIM3->CCDAT4 = generated[(idx+16) & 0x1F];
        break;
    case 0x20:
        TIM3->CCDAT1 = 0;
        TIM3->CCDAT2 = generated[idx & 0x1F];
        TIM3->CCDAT3 = 0;
        TIM3->CCDAT4 = generated[(idx+16) & 0x1F];
        break;
    case 0x30:
        TIM3->CCDAT1 = 0;
        TIM3->CCDAT2 = generated[idx & 0x1F];
        TIM3->CCDAT3 = generated[(idx+16) & 0x1F];
        TIM3->CCDAT4 = 0;
        break;
    default:
    }
    if (++idx>64) idx = 0;
}

void stepper_set_speed(int8_t speed) {
    speed_g = speed;
}