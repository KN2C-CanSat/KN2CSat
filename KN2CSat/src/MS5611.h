/*
 * MS5611.h
 *
 * Created: 1/26/2015 6:17:23 PM
 *  Author: Dell
 */ 

#ifndef MS5611_H_
#define MS5611_H_

#include "twi_master_driver.h"
//MS5611 - Air Pressure Sensor
#define MS5607_ADD 0x77
#define CPU_SPEED       32000000
#define BAUDRATE	    100000
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)  

extern uint8_t MS5611_flag;

 typedef struct {
 	
 	unsigned char c[2];
 	unsigned char d[3];
 	unsigned char ms_reset;
 	unsigned char PROM_read;
 	unsigned char read_d;
 	unsigned char ADC_read;
 	unsigned int n[6];
 	unsigned long int m[2];
 	float pres[3];
	uint8_t	count; 
 	
 } MS5611_Struct;

extern MS5611_Struct MS5611;
extern TWI_Master_t twiMaster;

extern long int pressure;
long int temperature;

void MS5611_reset(void);
void MS5611_read_PROM(void);
void MS5611_D_read(void);
void MS5611_calculate(unsigned int C1,unsigned int C2,unsigned int C3,unsigned int C4,unsigned int C5,unsigned int C6,long int D1,long int D2);

void MS5611_measure(void);
void read_d_command (unsigned char d_reg);
void ADC_read_command (unsigned char adc_reg, uint8_t i);
#endif /* MS5611_H_ */

