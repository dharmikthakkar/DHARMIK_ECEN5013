#ifndef _PWMLED_H
#define _PWMLED_H
#include <MKL25Z4.H>

void UART0_init(void);

void delayMs(int n);

void LED_init(void);

void LED_set(char value);

int x;

uint16_t initial =100;

void led_uart (void);
void LEDON(char value, uint16_t intensity);

void LEDbright(char value);
#endif