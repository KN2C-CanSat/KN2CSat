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

 uint8_t data_flag=1;  //nmishe tu ye tabe dg bashe?
  
 int main (void)
 {
	set_micro();
	NRF_init();
	MS5611_reset();
	SHT11_softreset();
    
	
 while (1)
 {
  	while(data_flag==0); 
	MS5611_measure();
  	SHT11_measure();
	NRF_Transmit();
	  
 }
 
 }
 
 
  ISR(TWIC_TWIM_vect) //twi interrupt
  {
  	TWI_MasterInterruptHandler(&twiMaster);
  }
 
 
  ISR(TCD0_OVF_vect) //timer interrupt
  {
	data_flag=1;  
  }
  
  
   ISR(PORTE_INT0_vect)////////////////////////////////////////PTX   IRQ Interrupt Pin
   {
	 PTX_IRQ();
	   
   }
