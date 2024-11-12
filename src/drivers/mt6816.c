#include "board.h"
#include <stdlib.h>

#define CS_LOW   MT6816_PORT->PBC = MT6816_CS
#define CS_HIGH  MT6816_PORT->PBSC = MT6816_CS
static uint8_t mt6816_tx_buf[4] = {
    0x83, 0, 0, 0
};

static uint8_t mt6816_buf[4];

void mt6816_init(void) {
    SPI_InitType SPI_InitStructure;
    GPIO_InitType GPIO_InitStructure;
    DMA_InitType DMA_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = MT6816_MISO;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1_SPI1;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Input;
    GPIO_InitPeripheral(MT6816_PORT, &GPIO_InitStructure);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = MT6816_MOSI;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF0_SPI1;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(MT6816_PORT, &GPIO_InitStructure);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = MT6816_CLK;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1_SPI1;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(MT6816_PORT, &GPIO_InitStructure);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = MT6816_CS;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
    GPIO_InitStructure.GPIO_Pull    = GPIO_No_Pull;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(MT6816_PORT, &GPIO_InitStructure);
    CS_HIGH;

    DMA_DeInit(MT6816_DMA_Channel);
    DMA_InitStructure.PeriphAddr     = (uint32_t)MT6816_DR_Base;
    DMA_InitStructure.MemAddr        = (uint32_t)mt6816_tx_buf;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize        = 4;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_MEDIUM;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(MT6816_DMA_Channel, &DMA_InitStructure);
    DMA_DeInit(RX_DMA_Channel);
    DMA_InitStructure.MemAddr        = (uint32_t)mt6816_buf;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_Init(RX_DMA_Channel, &DMA_InitStructure);

    /* SPI_MASTER configuration ------------------------------------------------------*/
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_4;
    SPI_InitStructure.FirstBit      = SPI_FB_MSB;
    SPI_InitStructure.CRCPoly       = 7;
    SPI_Init(MT6816_SPI, &SPI_InitStructure);
    SPI_I2S_EnableDma(MT6816_SPI, SPI_I2S_DMA_TX, ENABLE);
    SPI_I2S_EnableDma(MT6816_SPI, SPI_I2S_DMA_RX, ENABLE);

    SPI_Enable(MT6816_SPI, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ConfigInt(RX_DMA_Channel, DMA_INT_TXC | DMA_INT_ERR, ENABLE);

    /* Enable DMA Channel4 */
    DMA_RequestRemap(DMA_REMAP_SPI1_TX, DMA, MT6816_DMA_Channel, ENABLE);
    DMA_RequestRemap(DMA_REMAP_SPI1_RX, DMA, RX_DMA_Channel, ENABLE);
    RX_DMA_Channel->CHCFG |= DMA_CHCFG5_CHEN;
}

uint16_t mt6816_read(void) {
    uint16_t sample = ((uint16_t)mt6816_buf[1] < 8) | mt6816_buf[2];
    //print_log("RX %04x\n", sample);
    return sample>>2;
}

void mt6816_request(void) {
    CS_LOW;
    MT6816_DMA_Channel->TXNUM = 4;
    MT6816_DMA_Channel->CHCFG |= DMA_CHCFG4_CHEN;
}

void DMA_Channel5_IRQHandler(void) {
    if (DMA->INTSTS & DMA_INT_TXC5) {
        DMA->INTCLR |= DMA_INT_TXC5;
        CS_HIGH;
    }
}