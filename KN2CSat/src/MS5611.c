/*
 * MS5611.c
 *
 * Created: 1/26/2015 6:17:02 PM
 *  Author: Dell
 */ 
#include <asf.h>
#include <util/delay.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "twi_master_driver.h"
#include "setting.h"
#include "MS5611.h"
#include "SHT11.h"

MS5611_Struct MS5611;
TWI_Master_t twiMaster;    /*!< TWI master module. */

uint8_t MS5611_flag=1;
uint8_t MS5611_dataflag=0;
uint8_t twi_flag=0;

lngint2int finalpres; 

 long int pressure;
// long int temperature;
//int pressure;
int temperature;

void MS5611_measure(void)
{

  	switch (MS5611.count)
  	{
  		case	0	:	{
  							//MS5611_read_PROM();  bordamesh avale main
  							read_d_command(MS5611.read_d);  
  							MS5611.count++;
							break;}
  		case	3	:	{
							ADC_read_command(MS5611.ADC_read,0);
							MS5611.read_d=MS5611.read_d+16;
							read_d_command(MS5611.read_d); 
							MS5611.count++;
							break;}
 		case	6	:	{	
							MS5611.read_d=0x48;
							ADC_read_command(MS5611.ADC_read,1);
							if (SHT11_dataflag)
							{
								MS5611_calculate(MS5611.n[0],MS5611.n[1],MS5611.n[2],MS5611.n[3],MS5611.n[4],MS5611.n[5],MS5611.m[0],MS5611.m[1]);
								SHT11_dataflag=0;
								MS5611_dataflag=1;
							}
							read_d_command(MS5611.read_d);
							MS5611.count=1;
							break;}
							
// 		case	100	:	{
// 							//baraye chap
// 							printf2pc("Pressure: %ld\r",pressure);					
//  							printf2pc("Temperature: %ld\r\r",temperature);
// 							break;}
// 							 
 		default		:	{	
 							MS5611.count++;
 							break;}					
  							
  	}
	
// 	MS5611_read_PROM();
// 	MS5611_D_read();
// 	MS5611_calculate(MS5611.n[0],MS5611.n[1],MS5611.n[2],MS5611.n[3],MS5611.n[4],MS5611.n[5],MS5611.m[0],MS5611.m[1]);
}


void MS5611_calculate(unsigned int C1,unsigned int C2,unsigned int C3,unsigned int C4,unsigned int C5,unsigned int C6,long int D1,long int D2)
{
		
	float q;
	long int SENS;
	long int SENS2=0;
	long int OFF;
	long int OFF2=0;
	long int dT;
	float TEMP;
	//float T2=0;
	
			//long int pressure; //baraye chaap
	//long int temperature;
	
	dT=(long int)D2-(long int)(C5*pow(2,8));
	TEMP=(2000+(C6/(float)pow(2,23))*dT)/100;
	OFF=pow(2,16)/16*C2+dT/pow(2,7)/16*C4;
	SENS=pow(2,15)/8*C1+dT/pow(2,8)/8*C3;

	if (TEMP<20)
	{
		//T2=(dT/(float)pow(2,31))*dT;  //moshkel dare!! check beshe
		OFF2=(pow((TEMP-2000),2)/(float)2)*5;
		SENS2=(pow((TEMP-2000),2)/(float)4)*5;
		
		if (TEMP<-15)
		{
			OFF2=OFF2+7*pow((TEMP+1500),2);
			SENS2=SENS2+(pow((TEMP+1500),2)/(float)2)*11;
		}
		
		//TEMP=TEMP-T2;
		OFF=OFF-OFF2;
		SENS=SENS-SENS2;
	}
	
	
	MS5611.pres[1]=SENS/(float)pow(2,21)/(float)pow(2,15)*8*D1;
	q=OFF/(float)pow(2,15)*16;
	MS5611.pres[1]=(MS5611.pres[1]-q)/(float)100;
	
	MS5611.pres[1]=MS5611.pres[1]*100;
	
 	if (MS5611_flag==1)  //bare aval last_data ba data barabare
 	{
 		MS5611.pres[0]=MS5611.pres[1];
 		MS5611_flag=0;
 	}
 	
 	MS5611.pres[1]=MS5611.pres[0]+(0.028/(float)(0.028+1/(float)(2*(float)(3.14)*7)))*(MS5611.pres[1]-MS5611.pres[0]);    //low pass filter. 28 ms for ms5611 code.
 	MS5611.pres[0]=MS5611.pres[1];
	pressure=(long int)MS5611.pres[1]; //jash doroste?
	
	finalpres.lngint=pressure;
// 	printf2pc("Pressure:: %ld\r",finalpres.lngint);
// 	printf2pc("byte[0]: %d\r",finalpres.integer[0]);
// 	printf2pc("byte[1]: %d\r",finalpres.integer[1]);
	TEMP=TEMP*100;
	temperature=(long int)TEMP;
			//temperature=(long int)T2;
	//printf2pc("Temperature: %ld\r\r",temperature);

}



 void MS5611_reset(void)
 {
	 MS5611.ms_reset=0x1E;
	 TWI_MasterWriteRead(&twiMaster,MS5607_ADD,&MS5611.ms_reset,1,0);
 	 while (twiMaster.status != TWIM_STATUS_READY) {
      /* Wait until transaction is complete. */
      }
	 _delay_ms(3);
	
	  
 }


 
   void MS5611_read_PROM(void)
   {
   	 int i;
   	 MS5611.PROM_read=0xA2;
		
   	 for(i=0;i<6;i++)
   	 {
   		  TWI_MasterWriteRead(&twiMaster,MS5607_ADD,&MS5611.PROM_read,1,2);
    		  while (twiMaster.status != TWIM_STATUS_READY) {
    			  /* Wait until transaction is complete. */
    		  }
   		 MS5611.PROM_read=MS5611.PROM_read+2;
		 MS5611.c[0]=twiMaster.readData[0];
		 MS5611.c[1]=twiMaster.readData[1];	
   		 MS5611.n[i]=(unsigned int)MS5611.c[0]*256+(unsigned int)MS5611.c[1];   //avali MSB
			
   	 }

  	 
   }
   
   
   void MS5611_D_read(void)
   {
  	 
   	 int i;
   	 MS5611.read_d=0x48;
   	 MS5611.ADC_read=0x00;
		
   	 for(i=0;i<2;i++)
   	 {
   		  TWI_MasterWriteRead(&twiMaster,MS5607_ADD,&MS5611.read_d,1,0);
    		  while (twiMaster.status != TWIM_STATUS_READY) {   
    			  /* Wait until transaction is complete. */
    		  }
   		 _delay_ms(10);
   		 TWI_MasterWriteRead(&twiMaster,MS5607_ADD,&MS5611.ADC_read,1,3);
    		 while (twiMaster.status != TWIM_STATUS_READY) {    
    			 /* Wait until transaction is complete. */
    		 }
   		 MS5611.read_d=MS5611.read_d+16;
		 MS5611.d[0]=twiMaster.readData[0];
		 MS5611.d[1]=twiMaster.readData[1];
		 MS5611.d[2]=twiMaster.readData[2];
		 	
   		 MS5611.m[i]=(unsigned long int)MS5611.d[2]+(unsigned long int)MS5611.d[1]*256+(unsigned long int)MS5611.d[0]*65536;   
   	 }
  
  	 
  
   }
   
   
   void read_d_command (unsigned char d_reg)
   {
 		TWI_MasterWriteRead(&twiMaster,MS5607_ADD,&d_reg,1,0);
 		while (twiMaster.status != TWIM_STATUS_READY) {
 			/* Wait until transaction is complete. */
 		}   
   }
   
   
   void ADC_read_command (unsigned char adc_reg, uint8_t i)
   {
		TWI_MasterWriteRead(&twiMaster,MS5607_ADD,&adc_reg,1,3);
		while (twiMaster.status != TWIM_STATUS_READY) {
			/* Wait until transaction is complete. */
		}
		/*MS5611.read_d=MS5611.read_d+16;*/
		MS5611.d[0]=twiMaster.readData[0];
		MS5611.d[1]=twiMaster.readData[1];
		MS5611.d[2]=twiMaster.readData[2];
		   		 
		MS5611.m[i]=(unsigned long int)MS5611.d[2]+(unsigned long int)MS5611.d[1]*256+(unsigned long int)MS5611.d[0]*65536;   
   }