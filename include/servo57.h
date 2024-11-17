#ifndef __SERVO57_H__
#define __SERVO57_H__

#include <stdint.h>
#include "board.h"

/* Parameters */

#define STEPPER_MIN_SPEED 8000
#define STEPPER_MAX_SPEED 1200

/* Prototypes */

/* Stepper */
void stepper_set_speed(int16_t speed);
int16_t stepper_get_speed(void);

// CAN
void can_tx(uint8_t *data, uint8_t len) ;

/* ADC */
uint16_t get_adc(void);
uint16_t adc_get_temperature(void);
uint16_t adc_get_vbus(void);

/* Angle sensor */
uint16_t mt6816_read(void);
void mt6816_request(void);

/* RTT print */
void print_log(const char * sFormat, ...);

/* LED */
void LedOn(void);
void LedOff(void);
void LedBlink(void);

#endif /* __SERVO57_H__ */