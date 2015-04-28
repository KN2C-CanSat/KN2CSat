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
#include "NRF24l01.h"



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
	//LED
	PORTE_DIRSET = LED_Blue_PIN_bm;  
	PORTE_OUTCLR = LED_Blue_PIN_bm;  
	
	//TX
	PORTD_DIRSET = PIN3_bm; 
	PORTD_OUTSET = PIN3_bm;
	
	//SHT11 init
	PORTA.DIR=0x30; 
	PORTA.OUT=0x00;
	
	// wireless module & programmer data
	PORTD_DIRSET = NRF24L01_L_CS_LINE | NRF24L01_L_MOSI_LINE | NRF24L01_L_SCK_LINE; 
	PORTE_DIRSET = NRF24L01_L_CE_LINE;
	
	//IRQ interrupt (INT0)	
	PORTE_PIN0CTRL |= PORT_ISC_FALLING_gc; 
	PORTE_INTCTRL |= PORT_INT0LVL_LO_gc;
	PORTE_INT0MASK |= PIN0_bm;
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

#define TIMERD0_PER 0x9C3F  //10 ms
void TimerD0_init(void)
{
	tc_write_clock_source(&TCD0,TC_CLKSEL_DIV8_gc);
	tc_set_wgm(&TCD0,TC_WG_NORMAL);
	tc_set_overflow_interrupt_level(&TCD0,TC_INT_LVL_MED);
	tc_write_period(&TCD0,TIMERD0_PER);
	tc_set_direction(&TCD0,TC_UP);
	tc_enable(&TCD0);
}

void set_micro(void)
{

	En_RC32M();
	PMIC_CTRL |= PMIC_HILVLEN_bm | PMIC_LOLVLEN_bm |PMIC_MEDLVLEN_bm; // fa'al kardane interrupt ha?
	PORT_init();
	USARTD0_init();
	TimerD0_init();
	SPI_Init();
	TWI_MasterInit(&twiMaster,&TWIC,TWI_MASTER_INTLVL_LO_gc,TWI_BAUDSETTING);
	TWIC.SLAVE.CTRLA=0;  //slave disabled
	
	sei();


}


void SPI_Init(void)
{
	spi_xmega_set_baud_div(&NRF24L01_L_SPI,8000000UL,F_CPU);
	spi_enable_master_mode(&NRF24L01_L_SPI);
	spi_enable(&NRF24L01_L_SPI);
}


void variable_init(void)
{
	MS5611.count=0;
	MS5611.read_d=0x48;
	MS5611.ADC_read=0x00;
	//baghie ro biar inja
}