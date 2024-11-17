#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32l40x_it.h"
#include "n32l40x.h"
#include "n32l40x_i2c.h"
#include "oled.h"

/* Board definitions */

/* Led1 - PD0 */
#define LED1_PORT   GPIOD
#define LED1_PIN    GPIO_PIN_0

/* Buttons */

#define BTN1_PORT   GPIOB
#define BTN1_PIN    GPIO_PIN_9 /* Menu */
#define BTN2_PIN    GPIO_PIN_8 /* Enter */
#define BTN3_PORT   GPIOA
#define BTN3_PIN    GPIO_PIN_3 /* Next */

/* Externals */

#define EXT1_PORT   GPIOA
#define EXT1_PIN    GPIO_PIN_8 /* nDIR */
#define EXT2_PIN    GPIO_PIN_0 /* nSTP */
#define EXT3_PORT   GPIOB
#define EXT3_PIN    GPIO_PIN_7 /* nEN */
#define EXT4_PIN    GPIO_PIN_12 /* M_IN2 */
#define EXT5_PIN    GPIO_PIN_13 /* M_IN1 */
#define EXT6_PIN    GPIO_PIN_14 /* M_OUT1 */
#define EXT7_PIN    GPIO_PIN_15 /* M_OUT2 */

/* OLED ssd1306 Configuration */

#define SSD1306_SCL         GPIO_PIN_4
#define SSD1306_SDA         GPIO_PIN_5
#define SSD1306_RESET       GPIO_PIN_2
#define SSD1306_PORT        GPIOA
#define SSD1306_RPORT       GPIOB
#define SSD1306_I2C_ADDR    (0x3C << 1)
#define I2C_MASTER_ADDR     0x0

/* Encoder MT6816 */

#define MT6816_PORT         GPIOB
#define MT6816_MOSI         GPIO_PIN_5
#define MT6816_MISO         GPIO_PIN_4
#define MT6816_CLK          GPIO_PIN_3
#define MT6816_CS           GPIO_PIN_6
#define MT6816_DMA_Channel  DMA_CH4
#define RX_DMA_Channel      DMA_CH5
#define MT6816_SPI          SPI1
#define MT6816_DR_Base      SPI1_BASE+0x0c //0x4001300C

/* CAN */

#define CAN_PORT           GPIOA
#define CAN_TX_PIN         GPIO_PIN_12
#define CAN_RX_PIN         GPIO_PIN_11

/* Current ADC */

#define ADC_CURRENT_A       ADC_CH_3_PA2
#define ADC_CURRENT_B       ADC_CH_2_PA1
#define ADC_VBUS            ADC_CH_4_PA3
#define ADC_DMA_Channel     DMA_CH1

/* Stepper drivers */

#define PHASE_A_PORT        GPIOA
#define PHASE_A1_PIN        GPIO_PIN_6
#define PHASE_A2_PIN        GPIO_PIN_7
#define PHASE_B_PORT        GPIOB
#define PHASE_B1_PIN        GPIO_PIN_0
#define PHASE_B2_PIN        GPIO_PIN_1


/* Prototypes */

void board_init(void);
void oled_init(void);
void adc_init(void);
void led_init(void);
void stepper_init(void);
void buttons_init(void);
void external_init(void);
void can_init(void);
void mt6816_init(void);

void buttons_tick(void);
void ext_set(uint16_t pin, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */