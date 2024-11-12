#include "board.h"
#include <stdlib.h>
#include <string.h>  // For memset

#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

static __IO uint32_t I2CTimeout;
typedef enum
{
    C_READY = 0,
    C_START_BIT,
    C_STOP_BIT
} CommCtrl_t;
static CommCtrl_t Comm_Flag = C_READY;
static uint8_t err_code;
static uint8_t buffer[(SSD1306_WIDTH/8) * SSD1306_HEIGHT];

void oled_send(uint8_t addr, uint8_t *bytes, uint8_t len) {
    err_code = 0;
    I2CTimeout             = I2CT_LONG_TIMEOUT;
    while (I2C_GetFlag(I2C1, I2C_FLAG_BUSY)) {
        if ((I2CTimeout--) == 0) {
            err_code = 1;
            return;
        }
    }

    if (Comm_Flag == C_READY) {
        Comm_Flag = C_START_BIT;
        I2C_GenerateStart(I2C1, ENABLE);
    }

    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_MODE_FLAG)) { // EV5
        if ((I2CTimeout--) == 0) {
            err_code = 2;
            return;
        }
    }

    I2C_SendAddr7bit(I2C1, SSD1306_I2C_ADDR, I2C_DIRECTION_SEND);
    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_TXMODE_FLAG)) { // EV6
        if ((I2CTimeout--) == 0) {
            err_code = 3;
            return;
        }
    }
    Comm_Flag = C_READY;

    // Addr
    I2C_SendData(I2C1, addr);
    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_SENDING)) {// EV8
        if ((I2CTimeout--) == 0) {
            err_code = 4;
            return;
        }
    }

    // Data
    while (len-- > 0) {
        I2C_SendData(I2C1, *bytes++);
        I2CTimeout = I2CT_LONG_TIMEOUT;
        while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_SENDING)) {// EV8
            if ((I2CTimeout--) == 0) {
                err_code = 5;
                return;
            }
        }
    }

    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_SENDED)) {// EV8-2
        if ((I2CTimeout--) == 0) {
            err_code = 6;
            return;
        }
    }

    if (Comm_Flag == C_READY) {
        Comm_Flag = C_STOP_BIT;
        I2C_GenerateStop(I2C1, ENABLE);
    }
    
    while (I2C_GetFlag(I2C1, I2C_FLAG_BUSY)) {
        if ((I2CTimeout--) == 0) {
            err_code = 7;
            return;
        }
    }
    Comm_Flag = C_READY;
}

// Send a byte to the command register
static void ssd1306_WriteCommand(uint8_t byte) {
    oled_send(0, &byte, 1);
}

static void ssd1306_WriteCommandSeq(const uint8_t *bytes, size_t len) {
    for(int i=0; i<len; i++) {
        oled_send(0, (uint8_t*)&bytes[i], 1);
    }
}

// Send data
static void ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
    oled_send(0x40, buffer, buff_size);
}

void ssd1306_SetContrast(const uint8_t value) {
    const uint8_t kSetContrastControlRegister = 0x81;
    ssd1306_WriteCommand(kSetContrastControlRegister);
    ssd1306_WriteCommand(value);
}

const uint8_t init_seq[] = {
    0xae,//--turn off oled panel
	
    0xd5,//--set display clock divide ratio/oscillator frequency
    0x80,//--set divide ratio

    0xa8,//--set multiplex ratio
    0x3F, //0x27,//--1/40 duty

    0xd3,//-set display offset
    0x00,//-not offset

    0x20,//Addressing Setting Command Table
    0x00,//horizontal addressing mode

    0xad,//--Internal IREF Setting	
    0x30,//--

    0x8d,//--set Charge Pump enable/disable
    0x14,//--set(0x10) disable

    0x68,//0x40,//--set start line address

    0xa6,//--set normal display

    0xa4,//Disable Entire Display On

    0xa1,//--set segment re-map 128 to 0

    0xC8,//--Set COM Output Scan Direction 64 to 0

    0xda,//--set com pins hardware configuration
    0x12,

    0x81,//--set contrast control register
    0x7f,
    0xd9,//--set pre-charge period
    0x22,

    0xdb,//--set vcomh
    0x40,

    0xaf//--turn on oled panel
};

/* Initialize the oled screen */
void oled_init(void) {
    ssd1306_WriteCommandSeq(init_seq, sizeof(init_seq));

    oled_write(101);
}

// update screen
void oled_update(void)
{
    const uint8_t clr_seq[] = {
        0x21, 28, 99, 0x22, 0, 4 
    };
    ssd1306_WriteCommandSeq(clr_seq, sizeof(clr_seq));
    for (int i=0; i<72; i++)
        ssd1306_WriteData(&buffer[5*i], 5);
}

void oled_set_pos(uint8_t x, uint8_t y) {
    uint8_t pos_cmd[] = {0x21, x, x+1, 0x22, y, y+25};
    ssd1306_WriteCommandSeq(pos_cmd, sizeof(pos_cmd));
}

static const uint8_t font16x24 [] = {
	0x00, 0x00, 0xC0, 0xF8, 0xFE, 0xFE, 0x3F, 0x1F, 0x1F, 0x3F, 
    0xFE, 0xFC, 0xF8, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 
    0x00, 0x00, 0x00, 0x00, 0x03, 0x1F, 0x3F, 0x7F, 0xFC, 0xF8, 
    0xF8, 0xFC, 0x7F, 0x7F, 0x1F, 0x03, 0x00, 0x00, // 0
	
	
    0x00, 0x00, 0x00, 0xC0, 0xE0, 0xE0, 0xF0, 0x78, 0xFE, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x07, 0x03, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, // 1


    0x00, 0x00, 0xE0, 0xF8, 0xFE, 0xFE, 0x3F, 0x1F, 0x1F, 0x3F, 
    0xFF, 0xFE, 0xFC, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
    0x01, 0x01, 0x80, 0xE0, 0xF0, 0xFC, 0x7F, 0x1F, 0x0F, 0x03, 
    0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFE, 0xFF, 0xFF, 0xFB, 
    0xF9, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0x00, 0x00, // 2


    0x00, 0x00, 0x60, 0xFC, 0xFE, 0xFF, 0x3F, 0x1F, 0x1F, 0xFF, 
    0xFE, 0xFC, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
    0x80, 0x80, 0x00, 0x1C, 0x1E, 0x3F, 0xF7, 0xF3, 0xE1, 0xC0, 
    0x00, 0x00, 0x00, 0x00, 0x07, 0x1F, 0x3F, 0x7F, 0xFC, 0xF8, 
    0xF8, 0xFC, 0x7F, 0x7F, 0x3F, 0x0F, 0x00, 0x00, // 3


    0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xFC, 0xFF, 0xFF, 
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xFC, 
    0xBF, 0x8F, 0x83, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x80, 
    0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 
    0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x07, 0x00, 0x00, // 4


    0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0x1F, 0x8F, 0x8F, 0x8F, 
    0x8F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x1F, 
    0x1F, 0x1F, 0x0F, 0x07, 0x07, 0x0F, 0xFF, 0xFF, 0xFE, 0xF8, 
    0x00, 0x00, 0x00, 0x00, 0x0E, 0x3E, 0x7E, 0x7E, 0xF8, 0xF0, 
    0xF0, 0xF8, 0x7F, 0x7F, 0x1F, 0x07, 0x00, 0x00, // 5


    0x00, 0x00, 0xC0, 0xF8, 0xFC, 0xFE, 0x3F, 0x1F, 0x1F, 0x3F, 
    0xFF, 0xFE, 0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xF8, 0x7C, 0x7C, 0xFC, 0xFC, 0xF8, 0xF0, 0xC0, 
    0x00, 0x00, 0x00, 0x00, 0x03, 0x1F, 0x3F, 0x7F, 0xFC, 0xF8, 
    0xF8, 0xF8, 0xFF, 0x7F, 0x3F, 0x0F, 0x00, 0x00, // 6


    0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xDF, 
    0xFF, 0xFF, 0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x80, 0xF0, 0xFC, 0xFF, 0x1F, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 7


    0x00, 0x00, 0xF0, 0xFC, 0xFE, 0xFF, 0x1F, 0x0F, 0x0F, 0x1F, 
    0xFF, 0xFE, 0xFC, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x81, 0xE7, 
    0xF7, 0xFF, 0x7E, 0x3C, 0x3C, 0x7E, 0xFF, 0xF7, 0xE3, 0x81, 
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x7F, 0x7F, 0xF8, 0xF0, 
    0xF0, 0xF8, 0xFF, 0x7F, 0x3F, 0x1F, 0x00, 0x00, // 8


    0x00, 0x00, 0xF0, 0xFC, 0xFE, 0xFF, 0x1F, 0x1F, 0x1F, 0x3F, 0xFE, 0xFC, 0xF8, 0xC0, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x0F, 0x1F, 0x3F, 0x3F, 0x3E, 0x3E, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
    0x00, 0x00, 0x07, 0x3F, 0x7F, 0xFF, 0xFC, 0xF8, 0xF8, 0xFC, 0x7F, 0x3F, 0x1F, 0x03, 0x00, 0x00, // 9
   
};

static void copy_buf(const uint8_t* ptr, uint8_t pos) {
    for(int i=0; i<16; i++) {
        buffer[i + pos] |= ptr[i];
        buffer[i + pos + SSD1306_HEIGHT] |= ptr[i + 16];
        buffer[i + pos + SSD1306_HEIGHT * 2] |= ptr[i + 32];
    }
}

void oled_write(uint16_t n) {
    bool show = false;
    memset(buffer, 0, sizeof(buffer));
    if (n > 999) return;
    if (n>99) {
        copy_buf(&font16x24[(n/100) * 48], 0);
        n -= (n/100) * 100;
        show = true;
    }
    if (n>9 || show) {
        copy_buf(&font16x24[(n/10) * 48], 14);
        n -= (n/10) * 10;
    }
    copy_buf(&font16x24[n * 48], 28);
    oled_update();
}