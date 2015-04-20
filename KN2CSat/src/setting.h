/*
 * setting.h
 *
 * Created: 1/26/2015 6:29:19 PM
 *  Author: Dell
 */ 


#ifndef SETTING_H_
#define SETTING_H_

//LED
#define LED_Blue_PIN_bm		PIN3_bm
#define LED_Blue_PIN_bp		PIN3_bp

#define printf2pcs(T)      UART_SEND(sprintf(buffer,T));
#define printf2pc(T,...)   UART_SEND(sprintf(buffer,T,__VA_ARGS__));

extern char buffer[100];

void UART_SEND(int l);

void En_RC32M(void);
void PORT_init(void);


void TimerD0_init(void);

void SPI_Init(void);

void USARTD0_init(void);
void set_micro(void);



#endif /* SETTING_H_ */