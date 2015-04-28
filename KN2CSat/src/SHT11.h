/*
 * SHT11.h
 *
 * Created: 4/20/2015 4:42:11 PM
 *  Author: Dell
 */ 


#ifndef SHT11_H_
#define SHT11_H_

#define      SHT_DATA_OUT(X)         PORTA.DIR=(X==1)?(PORTA.DIR | 0x20):(PORTA.DIR & 0xDF)
#define      SHT_DATA_IN             (PORTA.IN&0x20)
#define      SHT_SCK(X)              PORTA.OUT=(X==1)?(PORTA.OUT | 0x10):(PORTA.OUT & 0xEF)
#define      noACK                    0
#define      ACK                      1

//adr  command  r/w
#define     STATUS_REG_W    0x06   //000   0011    0
#define     STATUS_REG_R    0x07   //000   0011    1
#define     MEASURE_TEMP    0x03   //000   0001    1
#define     MEASURE_HUMI    0x05   //000   0010    1
#define     RESET           0x1e   //000   1111    0


extern long int H,T; 

//SHT Functions
char SHT_WriteByte(unsigned char valu);
char SHT_ReadByte(unsigned char ack);
void s_transstart(void);
void s_connectionreset(void);
char SHT11_softreset(void);
char s_measure(unsigned char mode);
void calc_sth11(float *p_humidity ,float *p_temperature);
void SHT11_measure (void);
char SHT11_GetResult(unsigned char *p_value, unsigned char *p_checksum);
void givemethat (void);


#endif /* SHT11_H_ */