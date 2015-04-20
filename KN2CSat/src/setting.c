/*
 * setting.c
 *
 * Created: 1/26/2015 6:29:02 PM
 *  Author: Dell
 */ 

#include <asf.h>
#include "twi_master_driver.h"
#include <util/delay.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "setting.h"
#include "MS5611.h"

char buffer[100];
void UART_SEND(int l)
{
	int i;
	for(i=0;i<l;i++)
	usart_putchar(&USARTD0,buffer[i]);
	
}


void En_RC32M(void)
{
	// Start RC32M OSC
	OSC_CTRL |= OSC_RC32MEN_bm;
	while(!(OSC_STATUS & OSC_RC32MRDY_bm));

	// Select the system clock source: 32 MHz Internal RC Osc.
	CCP = CCP_IOREG_gc;
	CLK_CTRL = CLK_SCLKSEL_RC32M_gc;

	// Disable the unused oscillators: 2 MHz, internal 32 kHz, external clock/crystal oscillator, PLL
	OSC_CTRL &= ~(OSC_RC2MEN_bm | OSC_RC32KEN_bm | OSC_XOSCEN_bm | OSC_PLLEN_bm);
};

void PORT_init(void)
{

	PORTE_DIRSET = LED_Blue_PIN_bm;  //LED
	PORTE_OUTCLR = LED_Blue_PIN_bm;  //LED
	
	PORTD_DIRSET = PIN3_bm; //TX
	PORTD_OUTSET = PIN3_bm;
	
	//SHT11 init
	PORTA.DIR=0x30; 
	PORTA.OUT=0x00;
};

#define USARTD0_conf USARTD0
#define USARTD0_BUADRATE 115200
void USARTD0_init(void)
{
	usart_set_mode(&USARTD0_conf,USART_CMODE_ASYNCHRONOUS_gc);
	usart_format_set(&USARTD0_conf,USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc,false);
	//usart_set_rx_interrupt_level(&USARTE0_conf,USART_INT_LVL_MED);
	//usart_set_dre_interrupt_level(&USARTE0_conf,USART_INT_LVL_LO);
	usart_set_baudrate(&USARTD0_conf,USARTD0_BUADRATE,F_CPU);
	usart_tx_enable(&USARTD0_conf);
	usart_rx_enable(&USARTD0_conf);
}

#define TIMERD0_PER 0xF423
void TimerD0_init(void)
{
	tc_write_clock_source(&TCD0,TC_CLKSEL_DIV256_gc);
	tc_set_wgm(&TCD0,TC_WG_NORMAL);
	tc_set_overflow_interrupt_level(&TCD0,TC_INT_LVL_MED);
	tc_write_period(&TCD0,TIMERD0_PER);
	tc_set_direction(&TCD0,TC_UP);
	tc_enable(&TCD0);
}

void set_micro(void)
{

	En_RC32M();
	PMIC_CTRL |= PMIC_HILVLEN_bm | PMIC_LOLVLEN_bm |PMIC_MEDLVLEN_bm;
	PORT_init();
	USARTD0_init();
	TimerD0_init();

	sei();

	TWI_MasterInit(&twiMaster,&TWIC,TWI_MASTER_INTLVL_LO_gc,TWI_BAUDSETTING);	
	TWIC.SLAVE.CTRLA=0;  //slave disabled
	

}