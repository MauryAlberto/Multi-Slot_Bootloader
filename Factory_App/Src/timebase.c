#include "timebase.h"
#include "stm32f4xx.h"

#define TICK_FREQ		1
#define MAX_DELAY		0xFFFFFFFF

volatile uint32_t g_curr_tick;
volatile uint32_t g_curr_tick_p;
volatile uint32_t tick_freq = 1;

// delay in milliseconds
void delay(uint32_t delay)
{
	uint32_t tickstart = get_tick();
	uint32_t wait = delay;

	if(wait < MAX_DELAY)
	{
		wait += (uint32_t)TICK_FREQ;
	}

	while((get_tick() - tickstart) < wait) {}
}

uint32_t get_tick(void)
{
	__disable_irq();
	g_curr_tick_p = g_curr_tick;
	__enable_irq();

	return g_curr_tick_p;
}

void tick_increment(void)
{
	g_curr_tick += TICK_FREQ;
}

void SysTick_Handler(void)
{
	tick_increment();
}
