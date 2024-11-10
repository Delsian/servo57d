#include "board.h"
#include <stdlib.h>

static uint8_t button_state = 0x7;

void buttons_init(void) {
    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = BTN1_PIN | BTN2_PIN;
    GPIO_InitStructure.GPIO_Pull    = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Input;
    GPIO_InitPeripheral(BTN1_PORT, &GPIO_InitStructure);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin          = BTN3_PIN;
    GPIO_InitStructure.GPIO_Pull    = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Input;
    GPIO_InitPeripheral(BTN3_PORT, &GPIO_InitStructure);   
}

void buttons_tick(void) {
    uint8_t pins = (BTN1_PORT->PID >> 8) & 0x3;
    pins |= (BTN3_PORT->PID >> 13) & 4;
    if(button_state != pins) {
        if ((button_state^pins) & 1) {
            stepper_set_speed(0);
            print_log("button Enter\n");
        }
        if ((button_state^pins) & 2) {
            stepper_set_speed(1);
            print_log("button Menu\n");
        }
        if ((button_state^pins) & 4) {
            stepper_set_speed(-1);
            print_log("button Next\n");
        }
        button_state = pins;
    }
}