#ifndef __BSP_H__
#define __BSP_H__

#include <stdint.h>
#include <stdbool.h>

void led_init(void);
void led_toggle(uint32_t dly);
void led_on(void);
void led_off(void);
void button_init(void);
bool get_btn_state(void);
bool get_enter_btn_state(void);
uint8_t count_btn_presses(void);

#endif
