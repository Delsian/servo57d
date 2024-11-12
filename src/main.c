
#include "board.h"
#include <stdio.h>
#include <stdint.h>
#include <SEGGER_RTT.h>
#include "fw_version.h"

/**
 * @brief  Inserts a delay time.
 * @param count specifies the delay time length.
 */
void Delay(uint32_t count)
{
    volatile uint32_t c = count;
    for (; c > 0; c--)
        ;
}

/**
 * @brief Assert failed function by user.
 * @param file The name of the call that failed.
 * @param line The source line number of the call that failed.
 */
#ifdef USE_FULL_ASSERT
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    while (1)
    {
    }
}
#endif // USE_FULL_ASSERT

int main(void)

{
    SEGGER_RTT_Init();
    print_log("MKS Servo57d v.%s\n", FW_VERSION);
    print_log("Core clock: %d\n", SystemCoreClock);
    board_init();
    LedOn(); /*Turn on Led1*/
    oled_init();

    while (1)
    {       
        /* Insert delay */
        Delay(0x28FFFF);
        //ADC_EnableSoftwareStartConv(ADC, ENABLE);
        uint16_t sample = mt6816_read();
        float angle = (360.0 / 16384.0) * sample;
        oled_write((uint16_t)angle);
        //print_log("adc %d\n", get_adc());
    }
}

void print_log(const char * sFormat, ...)
{	
	va_list ParamList;
	va_start(ParamList, sFormat);
	SEGGER_RTT_vprintf(0, sFormat, &ParamList);
	va_end(ParamList);
}