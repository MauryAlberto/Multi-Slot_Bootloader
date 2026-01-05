#include <stdio.h>
#include "stm32f4xx.h"
#include "bsp.h"
#include "fpu.h"
#include "timebase.h"

#define VEC_TAB_BASE_ADDRESS		FLASH_BASE
#define VEC_TAB_OFFSET				0xC000

struct bl_common_apis
{
	void(*led_init)(void);
	void(*led_toggle)(uint32_t dly);
	void(*led_on)(void);
	void(*led_off)(void);
	void(*button_init)(void);
	bool(*get_btn_state)(void);
	bool (*get_enter_btn_state)(void);
	uint8_t (*count_btn_presses)(void);
	void(*fpu_enable)(void);
	void(*timebase_init)(void);
};

int main()
{

	struct bl_common_apis *common_apis = (struct bl_common_apis*)0x08010000;

	// Enable FPU
	common_apis->fpu_enable();

	// Initialize Timebase
	common_apis->timebase_init();

	// Initialize LED
	common_apis->led_init();

	// Initialize Push Button
	common_apis->button_init();

	while(1)
	{
		printf("Factory App is running...\n\r");
		delay(2000);
	}
}

void SystemInit(void)
{
	SCB->VTOR = VEC_TAB_BASE_ADDRESS | VEC_TAB_OFFSET;
}
