 /**
  * \file
  *
  * \brief Empty user application template
  *
  */
 
 /**
  * \mainpage User Application template doxygen documentation
  *
  * \par Empty user application template
  *
  * Bare minimum empty user application template
  *
  * \par Content
  *
  * -# Include the ASF header files (through asf.h)
  * -# "Insert system clock initialization code here" comment
  * -# Minimal main function that starts with a call to board_init()
  * -# "Insert application code here" comment
  *
  */
 
 /*
  * Include header files for all drivers that have been imported from
  * Atmel Software Framework (ASF).
  */
 #include <asf.h>
 #include <avr/io.h>
 #include <math.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <util/delay.h>
 #include "twi_master_driver.h"
 #include "setting.h"
 #include "MS5611.h"
 #include "SHT11.h"
 #include "NRF24l01.h"
 #define F_CPU 32000000UL

 //uint8_t data_flag=1;  //nmishe tu ye tabe dg bashe?
//  long int countt;
//  long int testt=0;
 uint8_t nrf_flag=0;
 int main (void)
 {
	set_micro();
	NRF_init();
	MS5611_reset();
	MS5611_read_PROM();
	SHT11_softreset();  
	variable_init();
	
	TCD0_CNT=0;
 	
 while (1)
 {

	
	//if(data_flag)
	//{
		//LED_Blue_PORT.OUTTGL = LED_Blue_PIN_bm;
		while (TCD0_CNT!=0xF9FF); //10 ms timer lock. kamesh kon, baraye nrf ziade! (?)
		SHT11_measure();
 		MS5611_measure();
 		
		if (MS5611_dataflag) 
		{
			NRF_Transmit(); //bbin chera ba moteghayer moshkel dare
			MS5611_dataflag=0;	
		}
		
		if (nrf_flag)  //doroste inja?
		{
			PTX_IRQ();
			nrf_flag=0;
		}
// 		if (twi_flag)    nashod
// 		{
// 		TWI_MasterInterruptHandler(&twiMaster);
// 		twi_flag=0;	
// 		}
		
	//}

 	// data_flag=0;

 //   _delay_us(3);  //dar girande?	 
  
 	 

 }
 
 }
 
 
    ISR(TWIC_TWIM_vect) //twi interrupt  //key mikhore ke biaram tu while?
    {
    	TWI_MasterInterruptHandler(&twiMaster);  //ino chi?
		//twi_flag=1;
    }
 
 
  ISR(TCD0_OVF_vect) //timer interrupt
  {
	//LED_Blue_PORT.OUTTGL = LED_Blue_PIN_bm;
	 
  }
  
  
      ISR(PORTE_INT0_vect)////////////////////////////////////////PTX   IRQ Interrupt Pin
      {
   		//PTX_IRQ();  //alan ino az interrupt bar daram flag bezaram?
   	    nrf_flag=1;
      }
