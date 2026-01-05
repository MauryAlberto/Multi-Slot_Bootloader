#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "bsp.h"
#include "fpu.h"
#include "timebase.h"

#define SECTOR0_BASE_ADDRESS		0x08000000 // Bootloader sector
#define SECTOR1_BASE_ADDRESS		0x08004000 // Default App sector
#define SECTOR2_BASE_ADDRESS		0x08008000 // App1 sector
#define SECTOR3_BASE_ADDRESS		0x0800C000 // Factory App sector

#define DEFAULT_APP_ADDRESS			SECTOR1_BASE_ADDRESS
#define APP1_ADDRESS				SECTOR2_BASE_ADDRESS
#define FACTORY_APP_ADDRESS			SECTOR3_BASE_ADDRESS

#define VEC_TAB_BASE_ADDRESS		FLASH_BASE
#define VEC_TAB_OFFSET				0x0000

#define APPLICATION_ADDRESS		0x08000000
#define MSP_VERIFY_MASK			0x2FFE0000

typedef void(*func_ptr)(void);

typedef enum
{
	APP1 = 1,
	FACTORY_APP,
}SYS_APPS;

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

struct bl_common_apis __attribute__((section(".COMMON_APIS"))) common_apis_table =
{
    led_init,
    led_toggle,
    led_on,
    led_off,
    button_init,
    get_btn_state,
	get_enter_btn_state,
	count_btn_presses,
    fpu_enable,
    timebase_init,
};

static void jump_to_app(uint32_t addr_value);
static void process_bl_cmds(void);

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

	delay(1000);

	if(get_btn_state())
	{
		// Button pressed
		printf("=====================\n\r");
		printf("---Bootloader Menu---\n\r");
		printf("=====================\n\r");
		printf("Press Blue Button 1 2 or 3 times\n\r");
		printf("1 -> Run App 1\n\r");
		printf("2 -> Factory App 1\n\r");
		printf("3 -> Run Default App\n\r");

		delay(1000);

		process_bl_cmds();
	}
	else
	{
		// Button not pressed
		jump_to_app(DEFAULT_APP_ADDRESS);
	}
}

static void jump_to_app(uint32_t app_address)
{
	uint32_t app_start_address;
	func_ptr jump_to_application;

	if((*(uint32_t*)(app_address) & MSP_VERIFY_MASK) == 0x20020000)
	{
		app_start_address = *(uint32_t*)(app_address + 4);

		jump_to_application = (func_ptr)app_start_address;

		// Initialize main stack pointer
		__set_MSP(*(uint32_t*)app_address);

		jump_to_application(); // jump to application address and start executing
	}
	else
	{
		printf("No application found at location...\n\r");
	}
}

static void process_bl_cmds(void)
{
	while(1)
	{
		count_btn_presses();
		delay(500);
		if(get_enter_btn_state())
		{
			break;
		}
	}

	switch(count_btn_presses())
	{
	case APP1: // 1 btn press == APP1
		printf("App 1 Selected...\n\r\n\r");
		jump_to_app(APP1_ADDRESS);
		break;
	case FACTORY_APP: // 2 btn presses == FACTORY_APP
		printf("Factory App Selected...\n\r\n\r");
		jump_to_app(FACTORY_APP_ADDRESS);
		break;
	default:
		printf("Default App Selected...\n\r\n\r");
		jump_to_app(DEFAULT_APP_ADDRESS);
		break;
	}
}



void SystemInit(void)
{
	SCB->VTOR = VEC_TAB_BASE_ADDRESS | VEC_TAB_OFFSET;
}
