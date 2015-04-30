/*
 * SHT11.c
 *
 * Created: 4/20/2015 4:41:54 PM
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
#include "SHT11.h"
#include "MS5611.h"

long int H,T; 


value humi_val, temp_val;

uint8_t SHT11_count=0;
uint8_t SHT11_dataflag=0;

enum {TEMP,HUMI};  //mishe bejash define konam? chon yeja dg am ina ro mikham
	
const float C1=-4.0;              // for 12 Bit
const float C2=+0.0405;           // for 12 Bit
const float C3=-0.0000028;        // for 12 Bit
const float T1=+0.01;             // for 14 Bit @ 5V
const float T2=+0.00008;          // for 14 Bit @ 5V	


void SHT11_measure (void)
{

    unsigned char error, checksum;

    error=0;
//     humi_val.i=0;  
//     temp_val.i=0;   comment kardam ke har bar sefr nakone! lazemesh daram dar bare bad baraye mohasebe humi             
    
  
	switch(SHT11_count)
	{
 		case	0	:	{	error+=s_measure(TEMP);
							SHT11_count++;	
							break;}
 		case	80	:	{	error+=SHT11_GetResult((unsigned char*) &temp_val.i,&checksum);
							if (error==0)
							{
								temp_val.f=(float)temp_val.i;                     //converts integer to float
								calc_sth11(&humi_val.f,&temp_val.f);              //calculate humidity, temperature
								T=(int)(temp_val.f*(float)100);
								printf2pc("Temp: %ld \r",T);
							}
							error+=s_measure(HUMI);
							SHT11_count++;
							break;}
 		case	100	:	{	error+=SHT11_GetResult((unsigned char*) &humi_val.i,&checksum);
							if (error==0)
							{	
								humi_val.f=(float)humi_val.i;                   //converts integer to float
								calc_sth11(&humi_val.f,&temp_val.f);              //calculate humidity, temperature
								H=(int)(humi_val.f*(float)100);
								printf2pc("Humi: %ld \r\r",H);
								SHT11_dataflag=1;
							}
							error+=s_measure(TEMP);
							SHT11_count=1;
							break;}																							
 		default		:	{
							SHT11_count++;
							break;}
	}
    
     if (error!=0) 
 	{
 		s_connectionreset();
 		printf2pcs("error\r");
 	}
             	
}


//----------------------------------------------------------------------------------
// tulis byte ke SHT
//----------------------------------------------------------------------------------
char SHT_WriteByte(unsigned char valu) 
{ 
   
  unsigned char i,error=0; 
  
  //puts("write");
  //putchar('\r');
   
  for (i=0x80;i>0;i/=2)   //i avalesh 10000000             
     { 
     if (i & valu)    SHT_DATA_OUT(0);  //bit be bit mifresti 
    else SHT_DATA_OUT(1);                        
    SHT_SCK(1);                          
    _delay_us(5);         //pulswith approx. 5 us     
    SHT_SCK(0);
     }
  SHT_DATA_OUT(0);                       //release DATA-line
  SHT_SCK(1);                            //clk #9 for ack 
  error=SHT_DATA_IN;                    //check ack (DATA will be pulled down by SHT11)
  SHT_SCK(0);        
  return error;                        //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
// baca byte dari SHT, dan berikan ACK
//----------------------------------------------------------------------------------
char SHT_ReadByte(unsigned char ack) 
{ 
  unsigned char i,val=0;
  //puts("read");
  //putchar('\r');
  
  SHT_DATA_OUT(0);                   //release DATA-line
  for (i=0x80;i>0;i/=2)   // i avalesh: 1 0 0 0 0 0 0 0 ine             
    { 
    SHT_SCK(1);                
    if (SHT_DATA_IN) val=(val | i);      //read bit  
    SHT_SCK(0);                  
     }
  SHT_DATA_OUT(ack);                  //in case of "ack==1" pull down DATA-Line
  SHT_SCK(1);                            //clk #9 for ack
  _delay_us(5);                  //pulswith approx. 5 us 
  SHT_SCK(0);                      
  SHT_DATA_OUT(0);                    //release DATA-line
  
  //printf2pc("val: %c\r",val);
  copy_val=val;
  return val;
}

//----------------------------------------------------------------------------------
// kondisi start
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
//----------------------------------------------------------------------------------
void s_transstart(void)
{                                
   //puts("transstart");
   //putchar('\r');
   
   SHT_DATA_OUT(0); 
   SHT_SCK(0);                   //Initial state
   _delay_us(1);
   SHT_SCK(1);
   _delay_us(1);
   SHT_DATA_OUT(1);
   _delay_us(1);
   SHT_SCK(0);  
   _delay_us(5);
   SHT_SCK(1);
   _delay_us(1);
   SHT_DATA_OUT(0);         
   _delay_us(1);
   SHT_SCK(0);         
}

//----------------------------------------------------------------------------------
// reset koneksi, 9 clock diikuti dengan kondisi start
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
//----------------------------------------------------------------------------------

void s_connectionreset(void)
{  
  
  unsigned char i;
  
  //puts("reset");
  //putchar('\r');
   
  SHT_DATA_OUT(0); SHT_SCK(0);            //Initial state
  for(i=0;i<9;i++)                      //9 SCK cycles
     { 
     SHT_SCK(1);
     _delay_us(1);
     SHT_SCK(0);
     }
  s_transstart();                       //transmission start
}

//----------------------------------------------------------------------------------
// reset SHT-nya !!
//----------------------------------------------------------------------------------
 
char SHT11_softreset(void)
{ 
  unsigned char error=0;

  //puts("softreset"); 
  //putchar('\r');
   
  s_connectionreset();              //reset communication
  error+=SHT_WriteByte(RESET);      //send RESET-command to sensor
  

  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
// pengukuran data
//----------------------------------------------------------------------------------
char s_measure(unsigned char mode)
{ 
  unsigned error=0;                                  
  //unsigned long int i,j; 
  
  //puts("s_measure");
  //putchar('\r');
  
  s_transstart();                   //transmission start
  switch(mode){                     //send command to sensor
    case TEMP   : error+=SHT_WriteByte(MEASURE_TEMP); break;
    case HUMI   : error+=SHT_WriteByte(MEASURE_HUMI); break;
    default     : break;    
  }
  
  //*********************************************************************************
//    switch(mode){                     //MS5611 measure
// 	   case HUMI   : MS5611_measure(); break;
// 	   default     : break;
//    }
  
  //*********************************************************************************
  
  //*** inja ro zamanesho kam kon (test kon), chon kheili tulanie age bekhad gir kone barname be har dalili sensor natune kar kone
 
//    for (i=0;i<65535;i++)     for (j=0;j<9344000;j++)   
//    if(SHT_DATA_IN==0) break; //wait until sensor has finished the measurement
//    if(SHT_DATA_IN) error+=1;                // or timeout (~2 sec.) is reached
//    *(p_value+1)  =SHT_ReadByte(ACK);    //read the first byte (MSB)    dar micro harchi adrese bishtr, arzesh bishtar. baraks zakhire mikone
//    *(p_value)  =SHT_ReadByte(ACK);    //read the second byte (LSB)
//    *p_checksum =SHT_ReadByte(ACK);  //read checksum 
//    check_sum= *p_checksum;
  
  return error;
}   

//----------------------------------------------------------------------------------------
// hitung temperature [°C] and humidity [%RH] 
// input :  humi [Ticks] (12 bit) 
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [°C] 
//----------------------------------------------------------------------------------------

void calc_sth11(float *p_humidity ,float *p_temperature)
{ 
  
  //float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit
  //float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [°C]
  
  //puts("calculate");
  //putchar('\r');
  
  t_C=*p_temperature*0.01 - 39.7;                  //calc. temperature from ticks to [°C] //man: dar 3.5(nazdik be 3.3)!
  rh_lin=C3*(*p_humidity)*(*p_humidity) + C2*(*p_humidity) + C1;     //calc. humidity from ticks to [%RH]
  rh_true=(t_C-25)*(T1+T2*(*p_humidity))+rh_lin;   //calc. temperature compensated humidity [%RH]
  if(rh_true>100)rh_true=100;       //cut if the value is outside of
  if(rh_true<0.1)rh_true=0.1;       //the physical possible range

  *p_temperature=t_C;               //return temperature [°C]
  *p_humidity=rh_true;              //return humidity[%RH]
}

//--------------------------------------------------------------------
// calculates dew point
// input:   humidity [%RH], temperature [°C]
// output:  dew point [°C]
//--------------------------------------------------------------------

/*float calc_dewpoint(float h,float t)
{ 
  float logEx,dew_point;
  logEx=0.66077+7.5*t/(237.3+t)+(log10(h)-2);
  dew_point = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
  return dew_point;
} */


char SHT11_GetResult(unsigned char *p_value, unsigned char *p_checksum)
{
	   unsigned error=0;
	   unsigned char crc;
	   
	   if(SHT_DATA_IN) error+=1;                 // or timeout (~2 sec.) is reached
	   *(p_value+1)=SHT_ReadByte(ACK);			 //read the first byte (MSB)    dar micro harchi adrese bishtr, arzesh bishtar. baraks zakhire mikone
	   *(p_value)  =SHT_ReadByte(ACK);			 //read the second byte (LSB)
	   *p_checksum =SHT_ReadByte(ACK);			 //read checksum
	   check_sum= *p_checksum;
	   
	   crc= copy_val/256 + copy_val%256;
	   if (crc != check_sum)
	    {
		    s_connectionreset();
		    //PORTD_OUTSET=LED_Red_PIN_bm;
	    }
	  
	   return error;
}
