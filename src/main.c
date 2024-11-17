
#include <stdio.h>
#include <stdint.h>
#include "servo57.h"
#include <SEGGER_RTT.h>
#include "fw_version.h"
#include "sched.h"

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
    uint8_t msg[5] = {1,5,0x77,0x88,0x23};
    SEGGER_RTT_Init();
    print_log("MKS Servo57d v.%s\n", FW_VERSION);
    print_log("Core clock: %d\n", SystemCoreClock);
    board_init();
    LedOn(); /*Turn on Led1*/
    oled_init();

    sched_main();
    /*while (1)
    {       
        Delay(0x28FFFF);
        uint16_t sample = mt6816_read();
        float angle = (360.0 / 16384.0) * sample;
        oled_write((uint16_t)angle);
        get_adc();
        //can_tx(msg, 5);
    }*/
}

void print_log(const char * sFormat, ...)
{	
	va_list ParamList;
	va_start(ParamList, sFormat);
	SEGGER_RTT_vprintf(0, sFormat, &ParamList);
	va_end(ParamList);
}

// shutdown() compatibility code
uint8_t ctr_lookup_static_string(const char *str) {
    return 0;
}