#include "bsp.h"
#include "stm32f4xx.h"
#include "timebase.h"
#include <stdbool.h>
#include <stdio.h>

#define GPIOAEN		(1 << 0)
#define GPIODEN		(1 << 3)

#define PIN15		(1 << 15)
#define LED_PIN		PIN15
#define PIN0		(1 << 0)
#define PIN8		(1 << 8)
#define BTN_PIN		PIN0
#define ENTER_BTN	PIN8

void led_init(void)
{
	// Enable clock access to GPIOD
	RCC->AHB1ENR |= GPIODEN;

	// Set PD15 to output mode
	GPIOD->MODER |=  (1 << 30);
	GPIOD->MODER &= ~(1 << 31);
}

void led_toggle(uint32_t dly)
{
	GPIOD->ODR ^= LED_PIN;
	delay(dly);
}

void led_on(void)
{
	// Set PD15 high
	GPIOD->ODR |= LED_PIN;
}

void led_off(void)
{
	// Set PD15 low
	GPIOD->ODR &= ~(LED_PIN);
}

void button_init(void)
{
	// Enable clock access to port A
	RCC->AHB1ENR |= GPIOAEN;

	// Set PA0 as input pin
	GPIOA->MODER &= ~(1 << 0);
	GPIOA->MODER &= ~(1 << 1);

	// Set PA8 as input pin
	GPIOA->MODER &= ~(1 << 16);
	GPIOA->MODER &= ~(1 << 17);

}

bool get_btn_state(void)
{
	// Check if button is pressed
	if(GPIOA->IDR & BTN_PIN)
	{
		return true;
	}else
	{
		return false;
	}
}

bool get_enter_btn_state(void)
{
	// Check if enter button is pressed
	if((GPIOA->IDR >> 8) & 1)
	{
		return true;
		printf("enter pressed...\n\r");
	}else
	{
		return false;
	}
}

uint8_t count_btn_presses(void)
{
	static int btn_count = 0;

	if(btn_count == 3)
	{
		return 3;
	}

	if(GPIOA->IDR & BTN_PIN)
	{
		btn_count++;
		printf("Button Count = %d\n\r", btn_count);
	}

	return btn_count;
}




