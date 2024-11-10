
#include "board.h"

void __attribute__ ((weak)) _init(void)  {}

void board_init(void) {
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    I2C_InitType i2c1_master;

    /* PCLK1 = HCLK/4 */
    RCC_ConfigPclk2(RCC_HCLK_DIV4);

    // --- RCC Init
    RCC_EnableAHBPeriphClk(
            RCC_AHB_PERIPH_DMA    | 
            RCC_AHB_PERIPH_ADC, ENABLE);
    RCC_EnableAPB1PeriphClk(
            RCC_APB1_PERIPH_I2C1  | 
            RCC_APB1_PERIPH_TIM2  |
            RCC_APB1_PERIPH_TIM3, ENABLE);
    RCC_EnableAPB2PeriphClk(
            RCC_APB2_PERIPH_SPI1  |
            RCC_APB2_PERIPH_TIM1  |
            RCC_APB2_PERIPH_AFIO  |
            RCC_APB2_PERIPH_GPIOA |  // I2C1
            RCC_APB2_PERIPH_GPIOB |  // SPI1
            RCC_APB2_PERIPH_GPIOD, ENABLE);

    // --- Timers
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // --- i2c (OLED ssd1306)
    /*PA4 -- SCL; PA5 -- SDA*/
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin               = SSD1306_SCL | SSD1306_SDA;
    GPIO_InitStructure.GPIO_Slew_Rate    = GPIO_Slew_Rate_High;
    GPIO_InitStructure.GPIO_Mode         = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Alternate    = GPIO_AF7_I2C1; 
    GPIO_InitStructure.GPIO_Pull         = GPIO_Pull_Up;	  
    GPIO_InitPeripheral(SSD1306_PORT, &GPIO_InitStructure);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = SSD1306_RESET;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
    GPIO_InitStructure.GPIO_Pull    = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(SSD1306_RPORT, &GPIO_InitStructure);

    I2C_DeInit(I2C1);
    i2c1_master.BusMode     = I2C_BUSMODE_I2C;
    i2c1_master.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    i2c1_master.OwnAddr1    = I2C_MASTER_ADDR;
    i2c1_master.AckEnable   = I2C_ACKEN;
    i2c1_master.AddrMode    = I2C_ADDR_MODE_7BIT;
    i2c1_master.ClkSpeed    = 100000; // 100K

    I2C_Init(I2C1, &i2c1_master);
    I2C_Enable(I2C1, ENABLE);
 
    led_init();
    mt6816_init();
    stepper_init();
    adc_init();
    buttons_init();
    external_init();
    can_init();

    /* Setup SysTick Timer for 1 usec interrupts  */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
       /* Capture error */
       while (1);
    }
}

void SysTick_Handler(void) {
    buttons_tick();
}

void SVC_Handler(void) {
}

void DebugMon_Handler(void) {
}

void DMA_IRQ_HANDLER(void) {
}

void HardFault_Handler(void) {
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 */
void MemManage_Handler(void) {
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 */
void BusFault_Handler(void) {
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 */
void UsageFault_Handler(void) {
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

void NMI_Handler(void) {
}