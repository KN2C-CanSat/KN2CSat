/*
 * NRF24l01.h
 *
 * Created: 4/20/2015 5:55:00 PM
 *  Author: Dell
 */ 


#ifndef NRF24L01_H_
#define NRF24L01_H_
#include <asf.h>
#include "stdarg.h"

#define	_CH_0 50			// Channel 0..125
#define	_CH_1 1
#define	_Address_Width	5	// 3..5
#define _Buffer_Size 32 // 1..32

/************************************************************************/
/* Board Initialize                                                     */
/************************************************************************/

#define NRF24L01_L_CE_HIGH		NRF24L01_L_PORT_CE.OUTSET = NRF24L01_L_CE_LINE
#define NRF24L01_L_CE_LOW		NRF24L01_L_PORT_CE.OUTCLR = NRF24L01_L_CE_LINE
#define NRF24L01_L_CS_HIGH		NRF24L01_L_PORT_CS.OUTSET = NRF24L01_L_CS_LINE
#define NRF24L01_L_CS_LOW		NRF24L01_L_PORT_CS.OUTCLR = NRF24L01_L_CS_LINE


// extern char Buf_Tx[_Buffer_Size]; //{'a','b','c','d','e'}; //{'a','b','c','d','e','f','g','a','b','c','d','e','f','g','a','b','c','d','e','f','g','a','b','c','d','a','b','c','d','e','f','g'}; //= "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
// extern char Address[_Address_Width];//pipe0 {0xE7,0xE7,0xE7,0xE7,0xE7};////
// extern NRF_BOARD Nrf;

char SPI_L(char TX_Data);



// Bits


#define TX_REUSE (1<<6)
#define TX_FULL  (1<<5)
#define TX_EMPTY (1<<4)
#define RX_FULL  (1<<1)
#define RX_EMPTY (1<<0)

/**
 * Data Ready RX FIFO interrupt
 */
#define _RX_DR (1<<6)

/**
 * Data Sent TX FIFO interrupt
 */
#define _TX_DS (1<<5)

/**
 * Maximum number of TX retransmits interrupt
 */
#define _MAX_RT (1<<4)

/** Power Down mode
 *
 * Minimal current consumption, SPI can be activated
 *
 * @see NRF24L01_L_Set_Power(char Mode)
 */
#define _POWER_DOWN	0

/** Power Up mode
 *
 * Standby-I mode
 *
 * @see NRF24L01_L_Set_Power(char Mode)
 */
#define _POWER_UP	(1<<1)

/** Mode radio transmitter
 *
 * @see NRF24L01_L_Set_Device_Mode(char Device_Mode)
 * @see NRF24L01_L_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define _TX_MODE	0

/** Mode radio receiver
 *
 * @see NRF24L01_L_Set_Device_Mode(char Device_Mode)
 * @see NRF24L01_L_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define _RX_MODE	1

/**  Air data rate = 1 Mbps
 *
 *
 * @see NRF24L01_L_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define _250k   (1<<5)  //khodam ezafe kardam

#define	_1Mbps	0

/** Air data rate = 2 Mbps
 *
 * @see NRF24L01_L_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define	_2Mbps	(1<<3)

/** Enable ShockBurst

 Automatic Retransmission (Up to 1 Re-Transmit on fail of AA)

 Auto Acknowledgment (data pipe 0)

 @see NRF24L01_L_Set_ShockBurst(char Mode)
 */
#define _ShockBurst_ON 1

/** Disable ShockBurst
 *
 @see NRF24L01_L_Set_ShockBurst(char Mode)
 */
#define _ShockBurst_OFF 0

/************************************************************************/
/* Transmit Power                                                                     */
/************************************************************************/

#define RF_PWR_LOW 0x00<<1 //-18dBm
#define RF_PWR_MED 0x01<<1 //-12dBm
#define RF_PWR_HI  0x02<<1 //-6dBm
#define RF_PWR_MAX 0x03<<1 //0dBm

// REGISTERS
#define CONFIG		0x00
#define EN_AA		0x01
#define EN_RXADDR	0x02
#define SETUP_AW	0x03
#define SETUP_RETR	0x04
#define RF_CH		0x05
#define RF_SETUP	0x06
#define STATUSe		0x07
#define OBSERVE_TX	0x08
#define CD			0x09
#define RX_ADDR_P0	0x0A
#define RX_ADDR_P1	0x0B
#define RX_ADDR_P2	0x0C
#define RX_ADDR_P3	0x0D
#define RX_ADDR_P4	0x0E
#define RX_ADDR_P5	0x0F
#define TX_ADDR		0x10
#define RX_PW_P0	0x11
#define RX_PW_P1	0x12
#define RX_PW_P2	0x13
#define RX_PW_P3	0x14
#define RX_PW_P4	0x15
#define RX_PW_P5	0x16
#define FIFO_STATUS	0x17
#define DYNPD		0x1C
#define FEATURE		0x1D

// COMMANDS
#define R_REGISTER			0x00
#define W_REGISTER			0x20
#define R_RX_PAYLOAD        0x61
#define W_TX_PAYLOAD        0xA0
#define FLUSH_TX      		0xE1
#define FLUSH_RX	 		0xE2
#define REUSE_TX_PL  		0xE3
#define ACTIVATE            0x50
#define R_RX_PL_WID         0x60
#define W_ACK_PAYLOAD		0xA8
#define W_TX_PAYLOAD_NOACK	0x58
#define NOP                 0xFF

/*
 * SPI functions for NRF24L01
 */
char NRF24L01_L_ReadReg(char Reg);
char NRF24L01_L_WriteReg(char Reg, char Value);
char NRF24L01_L_ReadRegBuf(char Reg, char *Buf, int Size);
char NRF24L01_L_WriteRegBuf(char Reg, char *Buf, int Size);

/*
 * NRF24L01 functions
 */
char NRF24L01_L_Get_Status(void);
char NRF24L01_L_Get_CD(void);
void NRF24L01_L_Set_Power(char Mode);
void NRF24L01_L_Set_CH(char CH);
void NRF24L01_L_Set_ShockBurst(char Mode);
void NRF24L01_L_Set_Address_Width(char Width);
void NRF24L01_L_Set_Device_Mode(char Device_Mode);
void NRF24L01_L_Set_RX_Pipe(char PipeNum, char *Address, int AddressSize, char PayloadSize);
void NRF24L01_L_Disable_All_Pipes(void);
char NRF24L01_L_Clear_Interrupts(void);
void NRF24L01_L_Set_TX_Address(char *Address, int Size);
void NRF24L01_L_Flush_TX(void);
void NRF24L01_L_Flush_RX(void);
void NRF24L01_L_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload, char Tx_Power);
void NRF24L01_L_Init_milad(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload, char Tx_Power);
void NRF24L01_L_RF_TX(void);
void NRF24L01_L_Write_TX_Buf(char *Buf, int Size);
void NRF24L01_L_Read_RX_Buf(char *Buf, int Size);


void NRF24L01_L_Receive(char Buf[_Buffer_Size]);
void NRF24L01_L_Send(char Buf[_Buffer_Size]);


//******************************************************************************************

void NRF_init (void);
void NRF_send(void);
void PTX_IRQ (void);



union _ch2int
{
	int real;
	char byte[2];
};

typedef union _ch2int  ch2int;




typedef struct {
	
	uint8_t Add;
	char    data[20];// age bekhad beshkane do bodish kon :p
	uint8_t State;
	uint8_t Num;
	uint8_t Check_Sum;
	uint8_t Len;
	uint8_t g_data;
	int counter;
	
}NRF_BOARD;

extern NRF_BOARD Nrf;

void Nrf_Empty_Data(void);
void NrF_Fill_Data( uint8_t num , ... );

void NRF_Transmit (void);

#endif /* NRF24L01_H_ */