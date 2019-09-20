#ifndef	_BOARD_H_
#define	_BOARD_H_
#include "mydef.h"
#include <iom16v.h>											// Target :	ATmega16 CPU寄存器定义
#include <macros.h>											// 常用宏定义

/*----------------------------------------------------------*/
/* 全局常量定义	*/											//
/*----------------------------------------------------------*/
/* 系统常数													*/
#define	c_FC_OSC				11059200					// 晶振频率11.0592MHz
#define	c_USART_BAUD0			115200						// 串口波特率 自定义,波特率加倍模式
#define	c_USART_UBRR0			c_FC_OSC/(8*c_USART_BAUD0)-1// 波特率寄存器	异步倍速模式 (U2X0 = 1)
#define c_ADC_NUM				8							// ADC采样的路数
/*__________________________________________________________*/
/*	外部变量声明 */											//
extern	u08	g_bTimer_10mS;									// 定时器10m秒时间到标志位
extern  u08 g_bTimer_50mS;
extern  u08 g_pu8CanSendBuffer[4];
extern  u16	g_Adc_Data[c_ADC_NUM];						    // ADC数据缓存	
extern  u08 g_Adc_Buf[c_ADC_NUM];
extern	u08	b_Rxd_Data;										// 串口数据接收完成标志位 true：接收完毕
extern	u08	g_Rxd_Buf[];									// 串口处理缓冲区
extern u08 ADC_SampHole_Flag;
extern volatile unsigned int  sys1msClock;



/*__________________________________________________________*/
//	IO初始化  //

//  霍尔开关  //
#define M8_Hole0_Io()			cbi(DDRC,bit5)				// 霍尔开关1，IO初始化
#define M8_Hole0_Up()			sbi(PORTC,bit5)				// 霍尔开关1，上拉
#define M8_Hole0_Down()			cbi(PORTC,bit5)
#define Read_Hole0				cki(PINC,bit5)	  			// 读霍尔开关1的电平变化
#define M8_Hole0_Sleep()		sbi(DDRC,bit5)				// 配置为输出
#define M8_Hole0_Low()			cbi(PORTC,bit5)

#define M8_Hole8_Io()			cbi(DDRC,bit6)				// 霍尔开关1，IO初始化
#define M8_Hole8_Up()			sbi(PORTC,bit6)				// 霍尔开关1，上拉
#define Read_Hole8				cki(PINC,bit6)	  			// 读霍尔开关1的电平变化
#define M8_Hole8_Sleep()		sbi(DDRC,bit6)				// 配置输出，低功耗
#define M8_Hole8_Low()			cbi(PORTC,bit6)				// 输出低电平

//	指示灯	//
#define	Led_Test_Io()			sbi(DDRC,bit7)				// 指示灯IO初始化
#define	Led_Test_On()			sbi(PORTC,bit7)				// 指示灯 1：点亮
#define	Led_Test_Off()			cbi(PORTC,bit7)				// 指示灯 0：灭
#define	Led_Test_Toggle()		kbi(PORTC,bit7)				// 指示灯反转

//	呼吸灯	//
#define	Led_Red_Io()			sbi(DDRC,bit0)				// 呼吸灯IO初始化
#define	Led_Red_On()			sbi(PORTC,bit0)				// 呼吸灯 1：点亮
#define	Led_Red_Off()			cbi(PORTC,bit0)				// 呼吸灯 0：灭
#define	Led_Red_Toggle()		kbi(PORTC,bit0)				// 呼吸灯反转

#define	Led_Green_Io()			sbi(DDRC,bit1)				// 呼吸灯IO初始化
#define	Led_Green_On()			sbi(PORTC,bit1)				// 呼吸灯 1：点亮
#define	Led_Green_Off()			cbi(PORTC,bit1)				// 呼吸灯 0：灭
#define	Led_Green_Toggle()		kbi(PORTC,bit1)				// 呼吸灯反转

#define	Led_Blue_Io()			sbi(DDRC,bit2)				// 呼吸灯IO初始化
#define	Led_Blue_On()			sbi(PORTC,bit2)				// 呼吸灯 1：点亮
#define	Led_Blue_Off()			cbi(PORTC,bit2)				// 呼吸灯 0：灭
#define	Led_Blue_Toggle()		kbi(PORTC,bit2)				// 呼吸灯反转
//	KEY按键指示灯 //
#define	Led_Touch_Io()			sbi(DDRB,bit0)				// 指示灯IO初始化
#define	Led_Touch_On()			cbi(PORTB,bit0)				// 指示灯 0：点亮
#define	Led_Touch_Off()			sbi(PORTB,bit0)				// 指示灯 1：灭

//	电机	*/
#define	Moto_Sleep_Io()			sbi(DDRD,bit6)				// 电机MOTO_nSLEEP控制I/O初始化
#define	Moto_Sleep_On()			cbi(PORTD,bit6)				// 电机控制IC休眠：0
#define	Moto_Sleep_Off()		sbi(PORTD,bit6)				// 电机控制IC正常工作：1
//	按键输入 //
#define	Read_Touch_Button		cki(PINB,bit1)				// 有按键低电平 I/O初始化在中断0初始化
#define Touch_Io()				cbi(PORTB,bit1)
#define Touch_Up()				sbi(PORTB,bit1)
#define Touch_Down()			cbi(PORTB,bit1)
/*	CAN	MCP2515	*/											//
#define	Read_MCP2515_IRQ		cki(PINB,bit2)				// MCP2515 IRQ 输入
#define MCP2515_IRQ_Io()		cbi(DDRB,bit2)
#define MCP2515_IRQ_Up()		sbi(PORTB,bit2)
#define MCP2515_IRQ_Down()		cbi(PORTB,bit2)
#define MCP2515_CS_Io()			sbi(PORTB,bit4)
#define	MCP2515_CS_Low()		cbi(PORTB,bit4)				//
#define	MCP2515_CS_High()		sbi(PORTB,bit4)				//
//	CAN	IO	//
#define	MCP2561_STBY_Io()		sbi(DDRB,bit3)				//
#define	MCP2561_STBY_On()		sbi(PORTB,bit3)				// 1：高电平
#define	MCP2561_STBY_Off()		cbi(PORTB,bit3)				// 0：低电平

/************************************************************/
/* 函数说明	*/												//
/************************************************************/
extern void Init_Board(void);
extern void Delay_Us( u08 g_Us	);									// us 延时
extern void Delay_ms( u16 g_ms	);									// 毫秒级延时
extern void Bell( u08 g_Bell );									// 蜂鸣器控制响的方式
extern void Putdata( u08 g_Dat	);									// 串口写函数 s_Dat:往串口写的数据
extern void Putdatas( u08 *g_Dat, u08 g_DatSize );
extern void Printf( c08 *g_String );								// 串口写函数 s_String:往串口写的字符串常量指针
extern void Power_Down(void);
extern void Power_Up(void);
extern void Lamp_On( u08 g_Lamp_Num );								//
extern void Lamp_Init(void);
extern void CAN_SoftVersionSend(void);								 // 版本发送
extern u08	SPI_RW(u08 dat);
extern u08	SPI_Read(u08 dat);
/************************************************************/

#endif //_BOARD_H_
/*
=================================================================================
------------------------------------End	of FILE----------------------------------
=================================================================================
*/
