#ifndef	_BOARD_H_
#define	_BOARD_H_
#include "mydef.h"
#include <iom16v.h>											// Target :	ATmega16 CPU�Ĵ�������
#include <macros.h>											// ���ú궨��

/*----------------------------------------------------------*/
/* ȫ�ֳ�������	*/											//
/*----------------------------------------------------------*/
/* ϵͳ����													*/
#define	c_FC_OSC				11059200					// ����Ƶ��11.0592MHz
#define	c_USART_BAUD0			115200						// ���ڲ����� �Զ���,�����ʼӱ�ģʽ
#define	c_USART_UBRR0			c_FC_OSC/(8*c_USART_BAUD0)-1// �����ʼĴ���	�첽����ģʽ (U2X0 = 1)
#define c_ADC_NUM				8							// ADC������·��
/*__________________________________________________________*/
/*	�ⲿ�������� */											//
extern	u08	g_bTimer_10mS;									// ��ʱ��10m��ʱ�䵽��־λ
extern  u08 g_bTimer_50mS;
extern  u08 g_pu8CanSendBuffer[4];
extern  u16	g_Adc_Data[c_ADC_NUM];						    // ADC���ݻ���	
extern  u08 g_Adc_Buf[c_ADC_NUM];
extern	u08	b_Rxd_Data;										// �������ݽ�����ɱ�־λ true���������
extern	u08	g_Rxd_Buf[];									// ���ڴ�������
extern u08 ADC_SampHole_Flag;
extern volatile unsigned int  sys1msClock;



/*__________________________________________________________*/
//	IO��ʼ��  //

//  ��������  //
#define M8_Hole0_Io()			cbi(DDRC,bit5)				// ��������1��IO��ʼ��
#define M8_Hole0_Up()			sbi(PORTC,bit5)				// ��������1������
#define M8_Hole0_Down()			cbi(PORTC,bit5)
#define Read_Hole0				cki(PINC,bit5)	  			// ����������1�ĵ�ƽ�仯
#define M8_Hole0_Sleep()		sbi(DDRC,bit5)				// ����Ϊ���
#define M8_Hole0_Low()			cbi(PORTC,bit5)

#define M8_Hole8_Io()			cbi(DDRC,bit6)				// ��������1��IO��ʼ��
#define M8_Hole8_Up()			sbi(PORTC,bit6)				// ��������1������
#define Read_Hole8				cki(PINC,bit6)	  			// ����������1�ĵ�ƽ�仯
#define M8_Hole8_Sleep()		sbi(DDRC,bit6)				// ����������͹���
#define M8_Hole8_Low()			cbi(PORTC,bit6)				// ����͵�ƽ

//	ָʾ��	//
#define	Led_Test_Io()			sbi(DDRC,bit7)				// ָʾ��IO��ʼ��
#define	Led_Test_On()			sbi(PORTC,bit7)				// ָʾ�� 1������
#define	Led_Test_Off()			cbi(PORTC,bit7)				// ָʾ�� 0����
#define	Led_Test_Toggle()		kbi(PORTC,bit7)				// ָʾ�Ʒ�ת

//	������	//
#define	Led_Red_Io()			sbi(DDRC,bit0)				// ������IO��ʼ��
#define	Led_Red_On()			sbi(PORTC,bit0)				// ������ 1������
#define	Led_Red_Off()			cbi(PORTC,bit0)				// ������ 0����
#define	Led_Red_Toggle()		kbi(PORTC,bit0)				// �����Ʒ�ת

#define	Led_Green_Io()			sbi(DDRC,bit1)				// ������IO��ʼ��
#define	Led_Green_On()			sbi(PORTC,bit1)				// ������ 1������
#define	Led_Green_Off()			cbi(PORTC,bit1)				// ������ 0����
#define	Led_Green_Toggle()		kbi(PORTC,bit1)				// �����Ʒ�ת

#define	Led_Blue_Io()			sbi(DDRC,bit2)				// ������IO��ʼ��
#define	Led_Blue_On()			sbi(PORTC,bit2)				// ������ 1������
#define	Led_Blue_Off()			cbi(PORTC,bit2)				// ������ 0����
#define	Led_Blue_Toggle()		kbi(PORTC,bit2)				// �����Ʒ�ת
//	KEY����ָʾ�� //
#define	Led_Touch_Io()			sbi(DDRB,bit0)				// ָʾ��IO��ʼ��
#define	Led_Touch_On()			cbi(PORTB,bit0)				// ָʾ�� 0������
#define	Led_Touch_Off()			sbi(PORTB,bit0)				// ָʾ�� 1����

//	���	*/
#define	Moto_Sleep_Io()			sbi(DDRD,bit6)				// ���MOTO_nSLEEP����I/O��ʼ��
#define	Moto_Sleep_On()			cbi(PORTD,bit6)				// �������IC���ߣ�0
#define	Moto_Sleep_Off()		sbi(PORTD,bit6)				// �������IC����������1
//	�������� //
#define	Read_Touch_Button		cki(PINB,bit1)				// �а����͵�ƽ I/O��ʼ�����ж�0��ʼ��
#define Touch_Io()				cbi(PORTB,bit1)
#define Touch_Up()				sbi(PORTB,bit1)
#define Touch_Down()			cbi(PORTB,bit1)
/*	CAN	MCP2515	*/											//
#define	Read_MCP2515_IRQ		cki(PINB,bit2)				// MCP2515 IRQ ����
#define MCP2515_IRQ_Io()		cbi(DDRB,bit2)
#define MCP2515_IRQ_Up()		sbi(PORTB,bit2)
#define MCP2515_IRQ_Down()		cbi(PORTB,bit2)
#define MCP2515_CS_Io()			sbi(PORTB,bit4)
#define	MCP2515_CS_Low()		cbi(PORTB,bit4)				//
#define	MCP2515_CS_High()		sbi(PORTB,bit4)				//
//	CAN	IO	//
#define	MCP2561_STBY_Io()		sbi(DDRB,bit3)				//
#define	MCP2561_STBY_On()		sbi(PORTB,bit3)				// 1���ߵ�ƽ
#define	MCP2561_STBY_Off()		cbi(PORTB,bit3)				// 0���͵�ƽ

/************************************************************/
/* ����˵��	*/												//
/************************************************************/
extern void Init_Board(void);
extern void Delay_Us( u08 g_Us	);									// us ��ʱ
extern void Delay_ms( u16 g_ms	);									// ���뼶��ʱ
extern void Bell( u08 g_Bell );									// ������������ķ�ʽ
extern void Putdata( u08 g_Dat	);									// ����д���� s_Dat:������д������
extern void Putdatas( u08 *g_Dat, u08 g_DatSize );
extern void Printf( c08 *g_String );								// ����д���� s_String:������д���ַ�������ָ��
extern void Power_Down(void);
extern void Power_Up(void);
extern void Lamp_On( u08 g_Lamp_Num );								//
extern void Lamp_Init(void);
extern void CAN_SoftVersionSend(void);								 // �汾����
extern u08	SPI_RW(u08 dat);
extern u08	SPI_Read(u08 dat);
/************************************************************/

#endif //_BOARD_H_
/*
=================================================================================
------------------------------------End	of FILE----------------------------------
=================================================================================
*/
