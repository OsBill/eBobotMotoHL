/********************************************************************************************
* 文件名: board.c																			*
* 创建人: 林振坤  创建时间 : 2018年4月13日22:59:19												*
* 修改人:    	  修改时间 : 											*
* 功能	: eBobotMotoHR 贝芽机器人右手臂操作													*
* 版权	: 贝芽																		*
********************************************************************************************/
#include "includes.h"


/************************************************************/
/* 变量、常量定义	*/										//
/************************************************************/
u08 g_pu8CanSendBuffer[4] = {0};

/* 串口	*/													//
#define	c_RXD_BUF_NUM			8							// 串口数据通讯协议字节数 小等于c_Databuff_num 用于接收到的数据个数
#define	c_DATA_BUFF_NUM			8							// data	buf的字节数量定义，用于定义串口数据暂存数组的空间
	u08	g_Rxd_Buf[c_DATA_BUFF_NUM];							// 串口处理缓冲区
	u08	g_Rxd_Buf_Num;
	u08	b_Rxd_Data;											// 串口数据接收完成标志位 true：接收完毕
/* ADC */
#define	c_ADC_NUM				8							// ADC采样的路数
#define	c_ADC_SUMS				4							// 平均数 即采样多少次取平均 次数要求是2的倍数 ｛预留｝
#define	c_ADC_SUMBIT			2							// 除法运算，采用移位来做。和上面的c_ADC_num对应｛预留｝
	u08	g_Adc_Sum				= 0;											// ADC采样次数计数器
	u08	g_Adc_Num				= 0;											// ADC通道选择
	u16	g_Adc_Data[c_ADC_NUM]	={0};						// ADC数据处理:算数和
    u08 g_Adc_Buf [c_ADC_NUM] = {0};
	u08 ADC_SampHole_Flag           = 0;        				//采样完成标志

/* 系统时间字节变量	*/
// Timer0
#define	c_Timer_100mS			10							// 100M秒定时参数，单位20mS	用于超声波测距
	u08	g_bTimer_10mS = false;								// 定时器20m秒时间到标志位
	u08 g_bTimer_50mS = false;
	u08	g_Timer_50mS  = 0;									// 100ms定时器
// Timer1
// Timer2

volatile unsigned char pollintack;    //定时tick
volatile unsigned int  sys1msClock;

/************************************************************/
void Factory_FunctionTest_SendState(void)
{
    g_pu8CanSendBuffer[0] = 0xFF;
	g_pu8CanSendBuffer[1] = 0xAA;
	g_pu8CanSendBuffer[2] = 0xBB;
    g_pu8CanSendBuffer[3] = g_Head_State;
	CAN_Send_Buffer0(g_pu8CanSendBuffer);

}

/************************************************************/
// 说明：I/O端口初始化，默认全部输入且上拉电阻不使能，降低功耗
void Port_Init(void)
{
	PORTA =	0x00;
	DDRA  =	0x00;
	PORTB =	0x00;
	DDRB  =	0x00;
	PORTC =	0x00;
	DDRC  =	0x00;
	PORTD =	0x00;
	DDRD  =	0x00;

	M8_Hole0_Io();                                           // 线性霍尔开关1初始化输入
	M8_Hole0_Up();											 // 线性霍尔开关1输入上拉(先不上拉，降低功耗)
	M8_Hole8_Io();                                           // 线性霍尔开关8初始化输入
	M8_Hole8_Up();											 // 线性霍尔开关8输入上拉(先不上拉，降低功耗)

	Led_Test_Io();											 // 指示灯初始化输出
	Led_Test_Off();											 // 指示灯输出关闭
	Led_Red_Io();											 // 三色灯红色初始化输出
	Led_Red_Off();											 // 三色灯红色输出关闭
	Led_Green_Io();											 // 三色灯绿色初始化输出
	Led_Green_Off();										 // 三色灯绿色输出关闭
	Led_Blue_Io();											 // 三色灯蓝色初始化输出
	Led_Blue_Off();											 // 三色灯蓝色输出关闭
	Led_Touch_Io();											 // 触摸指示灯初始化输出
	Led_Touch_Off();										 // 触摸指示灯输出关闭

	Moto_Sleep_Io();										 // 电机控制IC休眠初始化输出
	Moto_Sleep_Off();										 // 电机休眠模式

	Touch_Io();												 // 触摸初始化输出
	Touch_Up();											 	 // 触摸输出关闭

	MCP2515_IRQ_Io();										 // MCP2515_IRQ初始化输入
	MCP2515_IRQ_Up();										 // MCP2515_IRQ输入口上拉(先不上拉，降低功耗)

	MCP2515_CS_Io();										 // MCP2515_CS初始化输出
	MCP2515_CS_High();										 // MCP2515_CS输出高电平，正常模式

	MCP2561_STBY_Io();										 // MCP2561_STBY初始化输出
	MCP2561_STBY_Off();				 						 // MCP2561_STBY输出高电平，STBY模式

}

/************************************************************/
// Power_Init
// 掉电模式，低功耗处理
void Power_Init(void)										// 待机模式初始化
{
//  MCUCR |= BIT(SE);
	MCUCR &=~BIT(SM0);										// 待机模式控制，SM2;SM1;SM0 共6种
//	MCUCR |= BIT(SM0);										// 000:空闲模式；0.35mA
//	MCUCR &=~BIT(SM1);										// 001:ADC噪声抑制模式；
	MCUCR |= BIT(SM1);										// 010:掉电模式  <1uA
	MCUCR &=~BIT(SM2);										// 011:省电模式；
//	MCUCR |= BIT(SM2);										// 110:Standby模式；111:扩展Standby模式

//	SFIOR |= BIT(PUD);										// 禁用上拉电阻
	ACSR  |= BIT(ACD);										// 关闭模拟比较器电源 可以进一步省电
}
/************************************************************/
// Power_Down
// 掉电模式，低功耗处理
void Power_Down(void)
{
	CLI();
	Lamp_Off();
	MCP2561_STBY_On();
	Moto_Sleep_On();
    SFIOR |= BIT(PUD);										// 禁用上拉电阻
	ADCSRA &=~BIT(ADEN);									// ADC禁用 降低功耗
//	GIFR |=	BIT(INTF0);										// 清除中断标志位
	GICR |=	BIT(INT0);										// 开外部中断0 确保唤醒中断开启
	GIFR |=	BIT(INTF1);										// 清除中断标志位
//	GICR |=	BIT(INT1);										// 开外部中断1 确保唤醒中断开启
//	GIFR |=	BIT(INTF2);										// 清除中断标志位
//	GICR |=	BIT(INT2);										// 开外部中断2 确保唤醒中断开启
	SEI();													// 确保中断使能
}
/************************************************************/
// Power_Up
// 唤醒模式，复原处理
void Power_Up(void)
{
    MCP2561_STBY_Off();
	Moto_Sleep_Off();
	SFIOR &= ~BIT(PUD);										// 禁用上拉电阻
    Port_Init();
	GIFR |=	BIT(INTF0);										// 清除中断标志位
	GICR &=~BIT(INT0);										// 关外部中断1 按键采用查询模式
//	GICR |=	BIT(INT0);										// 开外部中断0 确保唤醒中断开启
//	GICR &=~BIT(INT0);										// 关外部中断1 按键采用查询模式
//	GIFR |=	BIT(INTF0);										// 清除中断标志位
//	GICR &=~BIT(INT1);										// 超声波中断要开启
//	GIFR |=	BIT(INTF1);										// 清除中断标志位
//	GICR &=~BIT(INT2);										// 关外部中断2 按键采用查询模式
//	GIFR |=	BIT(INTF2);										// 清除中断标志位
//  Port_Init();
	ADCSRA |= BIT(ADEN);									// ADC使能 首次ADC采样需要 25个ADC时钟
	ADCSRA |= BIT(ADSC);									// 启动ADC 从休眠唤醒之后，因之前有关闭ADC电源，因此需要重新开启ADC
}
/************************************************************/
// 说明：外部中断初始化
// int0_init - 低电平触发，CAN_RXD接入，用于低功耗唤醒
void Int0_Init(	void )										// INT0中断初始化
{
	PORTD |= BIT(PD2);										// 初始化端口 置1上拉
	DDRD  &=~BIT(PD2);										// 初始化端口 置0输入

	GICR &=~BIT(INT0);										// 先屏蔽外部中断0

	MCUCR &=~BIT(ISC00);									// 中断1 触发方式控制  四种模式	ISC01和ISC00控制
//	MCUCR |= BIT(ISC00);									// 00：INT0	为低电平时产生中断请求 01：INT1	引脚上任意的逻辑电平变化都将引发中断
	MCUCR &=~BIT(ISC01);									// 10：INT0	的下降沿产生异步中断请求
//	MCUCR |= BIT(ISC01);									// 11：INT0	的上升沿产生异步中断请求

	GIFR |=	BIT(INTF0);										// 清除中断标志位
	GICR |=	BIT(INT0);										// 开外部中断0
}

/************************************************************/
// 说明：外部中断初始化
// int1_init - 低电平触发
void Int1_Init(	void )										// INT1中断初始化
{
	PORTD |= BIT(PD3);										// 初始化端口 置1上拉
	DDRD  &=~BIT(PD3);										// 初始化端口 置0输入

	GICR &=~BIT(INT1);										// 先屏蔽外部中断1

//	MCUCR &=~BIT(ISC10);									// 中断1 触发方式控制  四种模式	ISC01和ISC00控制
	MCUCR |= BIT(ISC10);									// 00：INT1	为低电平时产生中断请求 01：INT1	引脚上任意的逻辑电平变化都将引发中断
	MCUCR &=~BIT(ISC11);									// 10：INT1	的下降沿产生异步中断请求
//	MCUCR |= BIT(ISC11);									// 11：INT1	的上升沿产生异步中断请求

	GIFR |=	BIT(INTF1);										// 清除中断标志位
//	GICR |=	BIT(INT1);										// 开外部中断1
}

/************************************************************/
// 说明：外部中断初始化
// int2_init	- 下降沿触发，CAN总线唤醒
void Int2_Init(	void )										// INT2中断初始化
{
//	PORTB |= BIT(PB2);										// 初始化端口 置1上拉
//	DDRB  &=~BIT(PB2);										// 初始化端口 置0输入

	GICR  &=~BIT(INT2);										// 先屏蔽外部中断2

//	MCUCSR |= BIT(ISC2);									// 中断2 触发方式控制 由ISC2控制 ISC2写1，INT2 的上升沿激活中断
	MCUCSR &=~BIT(ISC2);									// ISC2写0，INT2 的下降沿激活中断

	GIFR  |= BIT(INTF2);									// 写“1”清除中断标志位
	GICR  |= BIT(INT2);										// 使能外部中断2
}

/************************************************************/
// TIMER0 initialize - prescale:1024
// WGM:	Normal
// desired value: 10mSec 定时器
// actual value: 10.000mSec	(0.0%)
void Timer0_Init(void)
{
//	PORTD |= BIT(PD4);										// 初始化端口 置1上拉 T0端口
//	DDRD  &=~BIT(PD4);										// 初始化端口 置0输入
/*
	TCCR0 =	0x00;											// stop
	TCNT0 =	0x94;											// set count
	OCR0  =	0x6C;											// set compare
	TCCR0 =	0x05;											// start timer
*/


//1ms

	TCCR0 = 0x00; //stop
	TCNT0 = 0xD5; //set count
	OCR0  = 0x2B;  //set compare
	TCCR0 = 0x04; //start timer


	TIFR  |= BIT(TOV0);										// 清除T/C0	溢出中断标志位
	TIMSK |= BIT(TOIE0);									// timer interrupt sources



}

/************************************************************/
// 说明：timer1_init
// WGM:	0) Normal, TOP=0xFFFF
void Timer1_Init(void)										// 定时器1 左电机PWMB控制＋右电机PWMA/B控制
{
//	PORTB |= BIT(PB0);										// ICP1	输入上拉
//	DDRB  &=~BIT(PB0);										// ICP1	置0输入
	PORTD |= BIT(PD4);										// OC1A	输出1
	DDRD  |= BIT(PD4);										// OC1A	置1输出
	PORTD |= BIT(PD5);										// OC1B	输出1
	DDRD  |= BIT(PD5);										// OC1B	置1输出
//	PORTD |= BIT(PD5);										// T0	置1上拉
//	DDRD  &=~BIT(PD5);										// T0	置0输入

	TCCR1B = 0x00;											// stop
	TCNT1H = 0x00;											// setup
	TCNT1L = 0x00;											// 计数器清零

	OCR1AH = 0x00;											// 比较匹配寄存器，PWM模式下如果是0xFF就占空比100(一条直线)	如果是0x00，占空比0％，输出是窄脉冲
	OCR1AL = 0x00;
	OCR1BH = 0x00;
	OCR1BL = 0x00;
	ICR1H  = 0x00;											// 输入捕捉寄存器
	ICR1L  = 0x00;

//	TCCR1A &=~BIT(COM1A0);									// 通道A的比较输出模式COM1A1和COM1A0控制OC1A管脚输出电平
	TCCR1A |= BIT(COM1A0);									// 00：正常的端口操作，OC0 未连接  01：比较匹配时电平取反
//	TCCR1A &=~BIT(COM1A1);									// 10：比较匹配发生时OC0 清零，计数到TOP 时OC0 置位
	TCCR1A |= BIT(COM1A1);									// 11：比较匹配发生时OC0 置位，计数到TOP 时OC0 清零

//	TCCR1A &=~BIT(COM1B0);									// 通道B的比较输出模式COM1B1和COM1B0控制OC1B管脚输出电平
	TCCR1A |= BIT(COM1B0);									// 00：正常的端口操作，OC0 未连接  01：比较匹配时电平取反
//	TCCR1A &=~BIT(COM1B1);									// 10：比较匹配发生时OC0 清零，计数到TOP 时OC0 置位
	TCCR1A |= BIT(COM1B1);									// 11：比较匹配发生时OC0 置位，计数到TOP 时OC0 清零

//	TCCR1A &=~BIT(WGM10);									// WGM13:0 控制波形发生模式
	TCCR1A |= BIT(WGM10);									// 0000：正常的端口操作，OC0 未连接
	TCCR1A &=~BIT(WGM11);									// 0001: 8位相位修正PWM	TOP=0x00FF
//	TCCR1A |= BIT(WGM11);									// 0011: 10位相位修正PWM TOP=0x03FF
	TCCR1B &=~BIT(WGM12);									// 0101：8位快速PWM	TOP=0x00FF
//	TCCR1B |= BIT(WGM12);									// 0111: 10位快速PWM TOP=0x03FF
	TCCR1B &=~BIT(WGM13);									// 1110: 快速PWM TOP=ICRn
//	TCCR1B |= BIT(WGM13);									// 1111: 快速PWM  TOP=OCRnA

//	TCCR1B |= BIT(ICNC1);									// 输入捕捉噪声抑制器
//	TCCR1B |= BIT(ICES1);									// 输入捕捉触发沿选择 1:选择的是逻辑电平的上升沿触发输入捕捉
//	TCCR1B |= 0x00;											// 无时钟 T/C 不工作 以下分频系数在相位修正模式下减半
	TCCR1B |= 0x01;											// 定时器启动，分频比：clkT0S/1	   (没有预分频)
//	TCCR1B |= 0x02;											// 定时器启动，分频比：clkT0S/8	   (来自预分频器) 2710.588K	@11.0592Mhz
//	TCCR1B |= 0x03;											// 定时器启动，分频比：clkT0S/64   (来自预分频器)  @11.0592Mhz
//	TCCR1B |= 0x04;											// 定时器启动，分频比：clkT0S/256  (来自预分频器)  @11.0592Mhz
//	TCCR1B |= 0x05;											// 定时器启动，分频比：clkT0S/1024 (来自预分频器)
//	TCCR1B |= 0x06;											// 定时器启动，从T1	引脚的外部时钟源。时钟为下降沿
//	TCCR1B |= 0x07;											// 定时器启动，从T1	引脚的外部时钟源。时钟为上升沿
//	TCCR1B |= 0x0F;											// 定时器启动，从T1	引脚的外部时钟源。时钟为上升沿 比较匹配触发清零

//	TCCR1C |= BIT(FOC1A);									// 强制比较输出配置A
//	TCCR1C |= BIT(FOC1B);									// 强制比较输出配置B

//	TIFR  |= BIT(ICF1);										// 清除T/C1输入捕捉标志位
//	TIFR  |= BIT(OCF1A);									// 清除输出比较中断标志位
//	TIFR  |= BIT(OCF1B);									// 清除输出比较中断标志位
//	TIFR  |= BIT(TOV1);										// 清除T/C0	溢出中断标志位

//	TIMSK  |= BIT(TICIE1);									// T/C1	输入捕捉中断使能
//	TIMSK  |= BIT(OCIE1A);									// T/C1	输出比较 A 匹配中断使能
//	TIMSK  |= BIT(OCIE1B);									// T/C1	输出比较 B 匹配中断使能
//	TIMSK  |= BIT(TOIE1);									// T/C1	溢出中断使能

//	b_Timer1 = false;										// 时间到标识初始化
}

/************************************************************/
// TIMER2 initialize - prescale:1
// WGM:	PWM	Phase correct 呼吸灯PWM输出
// desired value: 21KHz
// actual value: 21.685KHz (3.2%)
void Timer2_Init(void)
{
//	PORTB |= BIT(PB3);										// OC2 输出1
//	DDRB  |= BIT(PB3);										// OC2 置1输出
	PORTD |= BIT(PD7);										// OC2 输出1
	DDRD  |= BIT(PD7);										// OC2 置1输出

//	TCCR2 = 0x00; 											//stop
//  ASSR  = 0x00;											//set async mode
//  TCNT2 = 0xF8; 											//setup
//  OCR2  = 0x08;
//  TCCR2 = 0x24; 											//start

	TCCR2 =	0x00; 											//stop
	ASSR  =	0x00; 											//set	async mode
	TCNT2 =	0x01; 											//setup
	OCR2  =	0xFF;
	TCCR2 =	0x71; 											//start

	//TIFR  |= BIT(TOV2);										// 清除T/C2	溢出中断标志位
	//TIMSK |= BIT(TOIE2);									// timer interrupt sources
}

/************************************************************/
// 说明：Adc	Initialize
// Conversion time:	104uS
void Adc_Init(void)
{
//	PORTC &=~0x00;											// 上拉电阻不使能
//	DDRC   = 0x00;											// ADC通道输入I/O配置为输入
	ADCSRA = 0x00;											// disable adc

	ADMUX  = 0x00;											// 选择通道时先关闭ADC和关闭比较器 默认选择0通道。数据右对齐
//	ADMUX &=~BIT(REFS0);									// 参考电压选择	 REFS1 REFS0
	ADMUX |= BIT(REFS0);									// 00：外部AREF，内部Vref 关闭	10：保留
	ADMUX &=~BIT(REFS1);									// 01：AVCC，AREF引脚外加滤波电容
//	ADMUX |= BIT(REFS1);									// 11：2.56V 的片内基准电压源，AREF引脚外加滤波电容

//	ADMUX &=~BIT(ADLAR);									// 选择右对齐
	ADMUX |= BIT(ADLAR);									// 选择左对齐

//	ADCSRA = 0x01;											// 分频比：2 即：ADC=fc/2 高采样率，低精度
//	ADCSRA = 0x02;											// 分频比：4 即：ADC=fc/4 分频比越高,精度越高,采样率越低。
//	ADCSRA = 0x03;											// 分频比：8 即：ADC=fc/8
//	ADCSRA = 0x04;											// 分频比：16 即：ADC=fc/16
//	ADCSRA = 0x05;											// 分频比：32 即：ADC=fc/32
	ADCSRA = 0x06;											// 分频比：64 即：ADC=fc/64	  采样时间75uS	@11.0592Mhz
	//ADCSRA = 0x07;											// 分频比：128 即：ADC=fc/128 采样时间150uS	@11.0592Mhz
	ADCSRA |= BIT(ADEN);									// ADC使能 ADC采样需要 13个ADC时钟
//	ADCSRA |= BIT(ADFR);									// 连续采样
	ADCSRA |= BIT(ADIF);									// 清除ADC中断
	ADCSRA |= BIT(ADIE);									// 开ADC中断

	ADCSRA |= BIT(ADSC);									// 启动ADC 启动后一个ADC时钟内不要切换ADC采样通道。

	g_Adc_Num =	0;											// 清ADC通道计数器
	g_Adc_Sum	= 0;
}

/************************************************************/
// UART0 initialize
// desired baud	rate: 115200
// actual: baud	rate:115200	(3.7%)
void Uart0_Init(void)
{
	PORTD |= BIT(PD0);										// 置1上拉	(否则会出现异常)
	DDRD  &=~BIT(PD0);										// RXD0	置0输入
	PORTD |= BIT(PD1);										// 输出1
	DDRD  |= BIT(PD1);										// TXD0	置1输出

	UCSRB =	0x00; 											//disable	while setting baud rate
	UCSRA =	0x02;
	UCSRC =	BIT(URSEL) | 0x06;
	UBRRL =	0x0B; 	   	 	  								//set	baud rate lo
	UBRRH =	0x00; 											//set	baud rate hi
	UCSRB =	0x98;

	UCSRB |= BIT(RXEN);										// 接收器使能
	UCSRB |= BIT(TXEN);										// 发送器使能
	UCSRB |= BIT(RXCIE);									// 接收中断使能
//	UCSRB |= BIT(TXCIE);									// 发送中断使能
//	UCSRB |= BIT(UDRIE);									// USART 数据寄存器空中断使能
//	UCSRB |= BIT(UCSZ2);									// USART 数据9位

	g_Rxd_Buf_Num =	0;
	b_Rxd_Data = false;										// 串口数据接收完成标志位
}

/************************************************************/
// SPI initialize
// clock rate: 5000000hz
void SPI_Initial( void )
{
	PORTB |= BIT(PB5);										// RF_MOSI初始化端口 如果输入置1上拉
	DDRB  |= BIT(PB5);										// RF_MOSI初始化端口 置1输出

	PORTB |= BIT(PB6);										// RF_MISO初始化端口 如果输入置1上拉
	DDRB  &=~BIT(PB6);										// RF_MISO初始化端口 置0输入

	PORTB |= BIT(PB7);										// RF_SCK初始化端口	如果输入置1上拉
	DDRB  |= BIT(PB7);										// RF_SCK初始化端口	置1输出

	PORTB |= BIT(PB4);										// CAN_CSN初始化端口 如果输入置1上拉
	DDRB  |= BIT(PB4);										// CAN_CSN初始化端口 置1输出
															// IRQ中断IO初始化在中断初始化函数
	SPCR = 0x50;											// setup SPI 模式0
	SPSR = 0x01;											// setup SPI bit clock rate	5529600Hz 最快速率
}

/************************************************************/
// SPI接口读写函数
/************************************************************/
u08	SPI_RW(	u08	dat	)
{
	SPDR = dat;

	while(!(SPSR&(1<<SPIF)));

	return SPDR;									// return read u08
}

u08	SPI_Read(	u08	dat	)
{
	SPDR = dat;

	while(!(SPSR&(1<<SPIF)));

	return SPDR;									// return read u08
}

/************************************************************/
// 说明：微秒延时
// 输入：g_Us:延时的时间
// 输出：无
void Delay_Us( u08 g_Us	)									// us 延时
{
	for( ; g_Us	> 0; g_Us--	)                             	// 1.36uS
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
//		NOP();
//		NOP();
//		NOP();
//		NOP();
	}
}

/************************************************************/
//说明：毫秒延时
//输入：g_Ms:延时的时间
//输出：无
void Delay_ms( u16 g_ms	)									// ms 延时
{
#define	c_Delay_Ms		(u16)(c_FC_OSC/8000)-4				// 1个循环大约8个周期计算方法：时钟/8000，再减去计算延时计数器g_Ms的周期

	u16	j;													// 特别要注意 用static声明的变量，编译后的代码是不一样的。特别是这类延时程序。

	for( ; g_ms	> 0; g_ms--	)
	{
		for( j = 0;	j <	c_Delay_Ms;	j++	);					// 根据仿真一个双字节变量的for循环大约8个周期
	}
}

/************************************************************/
//说明：串口写单字节数据函数 默认串口1
//输入：g_Dat:往串口写的数据
//输出：无
void Putdata( u08 g_Dat	)									// 串口写函数 s_Dat:往串口写的数据
{
	while( !( UCSRA	& (BIT(UDRE))) );						// 等待传送结束	等待发送缓冲区空
	UDR	= g_Dat;											// 送数据放在判断传送结束的之后可以提高效率
}

/************************************************************/
//说明：串口写n单字节数据函数 默认串口1
//输入：g_Dat:往串口写的数据指针，g_DatSize：字节数
//输出：无
void Putdatas( u08 *g_Dat, u08 g_DatSize )
{
	while( g_DatSize )
	{
		while( !( UCSRA	& (BIT(UDRE))) );					// 等待传送结束	等待发送缓冲区空
		UDR	= *g_Dat;										// 送数据放在判断传送结束的之后可以提高效率
		g_Dat++;
		g_DatSize--;
	}
}

/************************************************************/
//说明：串口写字符串函数 默认串口1
//输入：g_String:往串口写的字符串常量指针
//输出：无
void Printf( c08 *g_String )								// 串口写函数 s_String:往串口写的字符串常量指针
{
	while( *g_String )
	{
		while( !( UCSRA	& (BIT(UDRE))) );					// 等待传送结束	等待发送缓冲区空
		UDR	= *g_String;									// 送数据放在判断传送结束的之后可以提高效率
		g_String++;
	}
//	Putdata( '\r' );										// 增加一个回车符
}


/************************************************************/
//说明：call this routine to initialize	all	peripherals
//输入：无
//输出：外设初始化
/************************************************************/
void Init_Board(void)
{
	CLI();													// disable all interrupts：stop	errant interrupts until	set	up
	Port_Init();
//	Int0_Init();
//	Int1_Init();
	Int2_Init();
	Timer0_Init();
	Timer1_Init();
	Timer2_Init();
	Power_Init();
	Adc_Init();
//	Uart0_Init();
	SPI_Initial();
	Lamp_Init();											// 呼吸灯参数初始化
  //  Moto_Init();                                            // 电机参数初始化
	SEI();													// re-enable	interrupts
}

#pragma	interrupt_handler timer0_ovf_isr:iv_TIM0_OVF
// desired value: 10mSec
void timer0_ovf_isr(void)
{
	//TCNT0 =	0x94;											// reload counter value
	TCNT0 = 0xD5;


	pollintack = 1;
	sys1msClock++;





}
/*__________________________________________________________*/
#pragma	interrupt_handler timer1_ovf_isr:iv_TIM1_OVF
// desired value: PWM
void timer1_ovf_isr(void)
{
//	Debug_Toggle();											// 测试时间用
}

#pragma	interrupt_handler timer2_ovf_isr:iv_TIM2_OVF
// desired value: 1mS
void timer2_ovf_isr(void)
{
 	TCNT2 = 0x54; //reload counter value

}

/************************************************************/
//说明：ADC	conversion complete
//输入：无
//输出：无
/************************************************************/
#pragma	interrupt_handler Adc_Isr:			iv_ADC
/************************************************************/
void Adc_Isr(void)
{
	static u08	j = 0;
//	static u08  g_State_Temp = 10;
	g_Adc_Data[g_Adc_Num++] +=	ADCH;							// 当次采样值。只用了8位不大准
//	g_Adc_Num ++;											// 完成ADC采样后，要调整ADC通道

	if(	g_Adc_Num == c_ADC_NUM )							// 判断所需的采样的ADC路数是否全部完成
	{
		/*-------电流采集------------*/

	//  g_Adc_Sum ++;
	  if(++g_Adc_Sum>=4)
	  {
	  	for(j=0;j<c_ADC_NUM;j++)
	  	{
	  		g_Adc_Buf[j] =(u08)(g_Adc_Data[j]>>2);         //8次采样求平均值，速度极慢，AD中断处理些什么玩意

	  		g_Adc_Data[j] = 0;
	  	}

	  	g_Adc_Sum = 0;
		ADC_SampHole_Flag = 1;            					//采样完成

	  }


		g_Adc_Num =	0x00;									// ADC采样通道计数器清零
		ADMUX &= 0xE0 ;										// 重新选择ADC采样通道	清除低位5bit
		ADCSRA |= BIT(ADSC);								// 启动ADC，继续采样
		return;												// 退出中断
	}
	ADMUX++;												// 选择采样通道	低3位0-7为通道数，可以用加法切换通道
	ADCSRA |= BIT(ADSC);									// 启动ADC 模拟连续采样的次数
}

/************************************************************/
//说明：uart has received a	character
//输入：无
//输出：无
/************************************************************/
#pragma	interrupt_handler Uart0_Rx_Isr:iv_USART0_RXC
/************************************************************/
void Uart0_Rx_Isr(void)
{
	g_Rxd_Buf[g_Rxd_Buf_Num] = UDR;							// 收入临时缓冲区

	if(	g_Rxd_Buf[0]==0xAA )
	{
		g_Rxd_Buf_Num ++;
		if(	g_Rxd_Buf_Num == c_RXD_BUF_NUM )				// 缓冲区是否满了，如果溢出，计数器清零
		{
			g_Rxd_Buf_Num =	0x00;
			b_Rxd_Data = true;								// true：串口数据接收完毕
		}
	}
	else
	{
		g_Rxd_Buf_Num =	0x00;
	}
//	b_Led_Flash	= true;
}

/************************************************************/
#pragma	interrupt_handler int0_isr:iv_INT0
void int0_isr(void)
{

}

/************************************************************/
#pragma	interrupt_handler int1_isr:iv_INT1
void int1_isr(void)
{
}

/************************************************************/
#pragma	interrupt_handler int2_isr:iv_INT2
void int2_isr(void)
{
 //external	interupt on	INT2
 	CAN_ReceiveIRQHandler();
}

/*
=================================================================================
------------------------------------End	of FILE----------------------------------
=================================================================================
*/
