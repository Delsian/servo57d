#include "board.h"
#include <stdlib.h>

static __IO uint16_t ADCConvertedValue[4];
static uint16_t  V30 = 0;
/*xx mv per degree Celsius  by datasheet define*/
#define AVG_SLOPE  4
/*Tc1 is the temprature compensation value*/
#define Tc1        1.5f

void adc_init(void) {
    ADC_InitType ADC_InitStructure;
    DMA_InitType DMA_InitStructure;

    /* RCC_ADCHCLK_DIV16*/
    ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB, RCC_ADCHCLK_DIV16);
    RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8);  //select HSE as RCC ADC1M CLK Source	

    DMA_DeInit(ADC_DMA_Channel);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&ADC->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)&ADCConvertedValue;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = 4;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_HALFWORD;
    DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(ADC_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(DMA_REMAP_ADC1, DMA, ADC_DMA_Channel, ENABLE);

    /* Enable DMA channel1 */
    DMA_EnableChannel(ADC_DMA_Channel, ENABLE);
    ADC_DeInit(ADC);
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.MultiChEn      = ENABLE;
    ADC_InitStructure.ContinueConvEn = ENABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIGCONV_NONE;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 4;
    ADC_Init(ADC, &ADC_InitStructure);
    /* ADC1 regular channel configuration */
    ADC_ConfigRegularChannel(ADC, ADC_CURRENT_A, 1, ADC_SAMP_TIME_28CYCLES5);
    ADC_ConfigRegularChannel(ADC, ADC_CURRENT_B, 2, ADC_SAMP_TIME_28CYCLES5);
    ADC_ConfigRegularChannel(ADC, ADC_VBUS, 3, ADC_SAMP_TIME_55CYCLES5);
    ADC_ConfigRegularChannel(ADC, ADC_CH_TEMP_SENSOR, 4, ADC_SAMP_TIME_239CYCLES5);
    /* Enable ADC DMA */
    ADC_EnableDMA(ADC, ENABLE);

    /* Enable ADC */
    ADC_Enable(ADC, ENABLE);
    /* ADC1 enable temperature  */
    ADC_EnableTempSensorVrefint( ENABLE);
    V30 = *(__IO uint32_t*)((uint32_t)0x1FFFF7D0);

    /* Check ADC Ready */
    while(ADC_GetFlagStatusNew(ADC,ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADC1 calibration */
    ADC_StartCalibration(ADC);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC))
        ;

    ADC_EnableSoftwareStartConv(ADC, ENABLE);
}

uint16_t get_adc(void) {
    /* Input voltage Vadc = (R29/(R28+R29)) * Vin */
    uint16_t Vin = ADCConvertedValue[2] / 74; 
    int16_t Temper=(int16_t)((((V30 - ADCConvertedValue[3])*33000)/40950)/4 + 28);
     print_log("adc %u %u %u %d\n", ADCConvertedValue[0], ADCConvertedValue[1], Vin, Temper);
    return ADCConvertedValue[0]; 
}