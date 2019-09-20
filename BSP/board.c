/********************************************************************************************
* �ļ���: board.c																			*
* ������: ������  ����ʱ�� : 2018��4��13��22:59:19												*
* �޸���:    	  �޸�ʱ�� : 											*
* ����	: eBobotMotoHR ��ѿ���������ֱ۲���													*
* ��Ȩ	: ��ѿ																		*
********************************************************************************************/
#include "includes.h"


/************************************************************/
/* ��������������	*/										//
/************************************************************/
u08 g_pu8CanSendBuffer[4] = {0};

/* ����	*/													//
#define	c_RXD_BUF_NUM			8							// ��������ͨѶЭ���ֽ��� С����c_Databuff_num ���ڽ��յ������ݸ���
#define	c_DATA_BUFF_NUM			8							// data	buf���ֽ��������壬���ڶ��崮�������ݴ�����Ŀռ�
	u08	g_Rxd_Buf[c_DATA_BUFF_NUM];							// ���ڴ�������
	u08	g_Rxd_Buf_Num;
	u08	b_Rxd_Data;											// �������ݽ�����ɱ�־λ true���������
/* ADC */
#define	c_ADC_NUM				8							// ADC������·��
#define	c_ADC_SUMS				4							// ƽ���� ���������ٴ�ȡƽ�� ����Ҫ����2�ı��� ��Ԥ����
#define	c_ADC_SUMBIT			2							// �������㣬������λ�������������c_ADC_num��Ӧ��Ԥ����
	u08	g_Adc_Sum				= 0;											// ADC��������������
	u08	g_Adc_Num				= 0;											// ADCͨ��ѡ��
	u16	g_Adc_Data[c_ADC_NUM]	={0};						// ADC���ݴ���:������
    u08 g_Adc_Buf [c_ADC_NUM] = {0};
	u08 ADC_SampHole_Flag           = 0;        				//������ɱ�־

/* ϵͳʱ���ֽڱ���	*/
// Timer0
#define	c_Timer_100mS			10							// 100M�붨ʱ��������λ20mS	���ڳ��������
	u08	g_bTimer_10mS = false;								// ��ʱ��20m��ʱ�䵽��־λ
	u08 g_bTimer_50mS = false;
	u08	g_Timer_50mS  = 0;									// 100ms��ʱ��
// Timer1
// Timer2

volatile unsigned char pollintack;    //��ʱtick
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
// ˵����I/O�˿ڳ�ʼ����Ĭ��ȫ���������������費ʹ�ܣ����͹���
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

	M8_Hole0_Io();                                           // ���Ի�������1��ʼ������
	M8_Hole0_Up();											 // ���Ի�������1��������(�Ȳ����������͹���)
	M8_Hole8_Io();                                           // ���Ի�������8��ʼ������
	M8_Hole8_Up();											 // ���Ի�������8��������(�Ȳ����������͹���)

	Led_Test_Io();											 // ָʾ�Ƴ�ʼ�����
	Led_Test_Off();											 // ָʾ������ر�
	Led_Red_Io();											 // ��ɫ�ƺ�ɫ��ʼ�����
	Led_Red_Off();											 // ��ɫ�ƺ�ɫ����ر�
	Led_Green_Io();											 // ��ɫ����ɫ��ʼ�����
	Led_Green_Off();										 // ��ɫ����ɫ����ر�
	Led_Blue_Io();											 // ��ɫ����ɫ��ʼ�����
	Led_Blue_Off();											 // ��ɫ����ɫ����ر�
	Led_Touch_Io();											 // ����ָʾ�Ƴ�ʼ�����
	Led_Touch_Off();										 // ����ָʾ������ر�

	Moto_Sleep_Io();										 // �������IC���߳�ʼ�����
	Moto_Sleep_Off();										 // �������ģʽ

	Touch_Io();												 // ������ʼ�����
	Touch_Up();											 	 // ��������ر�

	MCP2515_IRQ_Io();										 // MCP2515_IRQ��ʼ������
	MCP2515_IRQ_Up();										 // MCP2515_IRQ���������(�Ȳ����������͹���)

	MCP2515_CS_Io();										 // MCP2515_CS��ʼ�����
	MCP2515_CS_High();										 // MCP2515_CS����ߵ�ƽ������ģʽ

	MCP2561_STBY_Io();										 // MCP2561_STBY��ʼ�����
	MCP2561_STBY_Off();				 						 // MCP2561_STBY����ߵ�ƽ��STBYģʽ

}

/************************************************************/
// Power_Init
// ����ģʽ���͹��Ĵ���
void Power_Init(void)										// ����ģʽ��ʼ��
{
//  MCUCR |= BIT(SE);
	MCUCR &=~BIT(SM0);										// ����ģʽ���ƣ�SM2;SM1;SM0 ��6��
//	MCUCR |= BIT(SM0);										// 000:����ģʽ��0.35mA
//	MCUCR &=~BIT(SM1);										// 001:ADC��������ģʽ��
	MCUCR |= BIT(SM1);										// 010:����ģʽ  <1uA
	MCUCR &=~BIT(SM2);										// 011:ʡ��ģʽ��
//	MCUCR |= BIT(SM2);										// 110:Standbyģʽ��111:��չStandbyģʽ

//	SFIOR |= BIT(PUD);										// ������������
	ACSR  |= BIT(ACD);										// �ر�ģ��Ƚ�����Դ ���Խ�һ��ʡ��
}
/************************************************************/
// Power_Down
// ����ģʽ���͹��Ĵ���
void Power_Down(void)
{
	CLI();
	Lamp_Off();
	MCP2561_STBY_On();
	Moto_Sleep_On();
    SFIOR |= BIT(PUD);										// ������������
	ADCSRA &=~BIT(ADEN);									// ADC���� ���͹���
//	GIFR |=	BIT(INTF0);										// ����жϱ�־λ
	GICR |=	BIT(INT0);										// ���ⲿ�ж�0 ȷ�������жϿ���
	GIFR |=	BIT(INTF1);										// ����жϱ�־λ
//	GICR |=	BIT(INT1);										// ���ⲿ�ж�1 ȷ�������жϿ���
//	GIFR |=	BIT(INTF2);										// ����жϱ�־λ
//	GICR |=	BIT(INT2);										// ���ⲿ�ж�2 ȷ�������жϿ���
	SEI();													// ȷ���ж�ʹ��
}
/************************************************************/
// Power_Up
// ����ģʽ����ԭ����
void Power_Up(void)
{
    MCP2561_STBY_Off();
	Moto_Sleep_Off();
	SFIOR &= ~BIT(PUD);										// ������������
    Port_Init();
	GIFR |=	BIT(INTF0);										// ����жϱ�־λ
	GICR &=~BIT(INT0);										// ���ⲿ�ж�1 �������ò�ѯģʽ
//	GICR |=	BIT(INT0);										// ���ⲿ�ж�0 ȷ�������жϿ���
//	GICR &=~BIT(INT0);										// ���ⲿ�ж�1 �������ò�ѯģʽ
//	GIFR |=	BIT(INTF0);										// ����жϱ�־λ
//	GICR &=~BIT(INT1);										// �������ж�Ҫ����
//	GIFR |=	BIT(INTF1);										// ����жϱ�־λ
//	GICR &=~BIT(INT2);										// ���ⲿ�ж�2 �������ò�ѯģʽ
//	GIFR |=	BIT(INTF2);										// ����жϱ�־λ
//  Port_Init();
	ADCSRA |= BIT(ADEN);									// ADCʹ�� �״�ADC������Ҫ 25��ADCʱ��
	ADCSRA |= BIT(ADSC);									// ����ADC �����߻���֮����֮ǰ�йر�ADC��Դ�������Ҫ���¿���ADC
}
/************************************************************/
// ˵�����ⲿ�жϳ�ʼ��
// int0_init - �͵�ƽ������CAN_RXD���룬���ڵ͹��Ļ���
void Int0_Init(	void )										// INT0�жϳ�ʼ��
{
	PORTD |= BIT(PD2);										// ��ʼ���˿� ��1����
	DDRD  &=~BIT(PD2);										// ��ʼ���˿� ��0����

	GICR &=~BIT(INT0);										// �������ⲿ�ж�0

	MCUCR &=~BIT(ISC00);									// �ж�1 ������ʽ����  ����ģʽ	ISC01��ISC00����
//	MCUCR |= BIT(ISC00);									// 00��INT0	Ϊ�͵�ƽʱ�����ж����� 01��INT1	������������߼���ƽ�仯���������ж�
	MCUCR &=~BIT(ISC01);									// 10��INT0	���½��ز����첽�ж�����
//	MCUCR |= BIT(ISC01);									// 11��INT0	�������ز����첽�ж�����

	GIFR |=	BIT(INTF0);										// ����жϱ�־λ
	GICR |=	BIT(INT0);										// ���ⲿ�ж�0
}

/************************************************************/
// ˵�����ⲿ�жϳ�ʼ��
// int1_init - �͵�ƽ����
void Int1_Init(	void )										// INT1�жϳ�ʼ��
{
	PORTD |= BIT(PD3);										// ��ʼ���˿� ��1����
	DDRD  &=~BIT(PD3);										// ��ʼ���˿� ��0����

	GICR &=~BIT(INT1);										// �������ⲿ�ж�1

//	MCUCR &=~BIT(ISC10);									// �ж�1 ������ʽ����  ����ģʽ	ISC01��ISC00����
	MCUCR |= BIT(ISC10);									// 00��INT1	Ϊ�͵�ƽʱ�����ж����� 01��INT1	������������߼���ƽ�仯���������ж�
	MCUCR &=~BIT(ISC11);									// 10��INT1	���½��ز����첽�ж�����
//	MCUCR |= BIT(ISC11);									// 11��INT1	�������ز����첽�ж�����

	GIFR |=	BIT(INTF1);										// ����жϱ�־λ
//	GICR |=	BIT(INT1);										// ���ⲿ�ж�1
}

/************************************************************/
// ˵�����ⲿ�жϳ�ʼ��
// int2_init	- �½��ش�����CAN���߻���
void Int2_Init(	void )										// INT2�жϳ�ʼ��
{
//	PORTB |= BIT(PB2);										// ��ʼ���˿� ��1����
//	DDRB  &=~BIT(PB2);										// ��ʼ���˿� ��0����

	GICR  &=~BIT(INT2);										// �������ⲿ�ж�2

//	MCUCSR |= BIT(ISC2);									// �ж�2 ������ʽ���� ��ISC2���� ISC2д1��INT2 �������ؼ����ж�
	MCUCSR &=~BIT(ISC2);									// ISC2д0��INT2 ���½��ؼ����ж�

	GIFR  |= BIT(INTF2);									// д��1������жϱ�־λ
	GICR  |= BIT(INT2);										// ʹ���ⲿ�ж�2
}

/************************************************************/
// TIMER0 initialize - prescale:1024
// WGM:	Normal
// desired value: 10mSec ��ʱ��
// actual value: 10.000mSec	(0.0%)
void Timer0_Init(void)
{
//	PORTD |= BIT(PD4);										// ��ʼ���˿� ��1���� T0�˿�
//	DDRD  &=~BIT(PD4);										// ��ʼ���˿� ��0����
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


	TIFR  |= BIT(TOV0);										// ���T/C0	����жϱ�־λ
	TIMSK |= BIT(TOIE0);									// timer interrupt sources



}

/************************************************************/
// ˵����timer1_init
// WGM:	0) Normal, TOP=0xFFFF
void Timer1_Init(void)										// ��ʱ��1 ����PWMB���ƣ��ҵ��PWMA/B����
{
//	PORTB |= BIT(PB0);										// ICP1	��������
//	DDRB  &=~BIT(PB0);										// ICP1	��0����
	PORTD |= BIT(PD4);										// OC1A	���1
	DDRD  |= BIT(PD4);										// OC1A	��1���
	PORTD |= BIT(PD5);										// OC1B	���1
	DDRD  |= BIT(PD5);										// OC1B	��1���
//	PORTD |= BIT(PD5);										// T0	��1����
//	DDRD  &=~BIT(PD5);										// T0	��0����

	TCCR1B = 0x00;											// stop
	TCNT1H = 0x00;											// setup
	TCNT1L = 0x00;											// ����������

	OCR1AH = 0x00;											// �Ƚ�ƥ��Ĵ�����PWMģʽ�������0xFF��ռ�ձ�100(һ��ֱ��)	�����0x00��ռ�ձ�0���������խ����
	OCR1AL = 0x00;
	OCR1BH = 0x00;
	OCR1BL = 0x00;
	ICR1H  = 0x00;											// ���벶׽�Ĵ���
	ICR1L  = 0x00;

//	TCCR1A &=~BIT(COM1A0);									// ͨ��A�ıȽ����ģʽCOM1A1��COM1A0����OC1A�ܽ������ƽ
	TCCR1A |= BIT(COM1A0);									// 00�������Ķ˿ڲ�����OC0 δ����  01���Ƚ�ƥ��ʱ��ƽȡ��
//	TCCR1A &=~BIT(COM1A1);									// 10���Ƚ�ƥ�䷢��ʱOC0 ���㣬������TOP ʱOC0 ��λ
	TCCR1A |= BIT(COM1A1);									// 11���Ƚ�ƥ�䷢��ʱOC0 ��λ��������TOP ʱOC0 ����

//	TCCR1A &=~BIT(COM1B0);									// ͨ��B�ıȽ����ģʽCOM1B1��COM1B0����OC1B�ܽ������ƽ
	TCCR1A |= BIT(COM1B0);									// 00�������Ķ˿ڲ�����OC0 δ����  01���Ƚ�ƥ��ʱ��ƽȡ��
//	TCCR1A &=~BIT(COM1B1);									// 10���Ƚ�ƥ�䷢��ʱOC0 ���㣬������TOP ʱOC0 ��λ
	TCCR1A |= BIT(COM1B1);									// 11���Ƚ�ƥ�䷢��ʱOC0 ��λ��������TOP ʱOC0 ����

//	TCCR1A &=~BIT(WGM10);									// WGM13:0 ���Ʋ��η���ģʽ
	TCCR1A |= BIT(WGM10);									// 0000�������Ķ˿ڲ�����OC0 δ����
	TCCR1A &=~BIT(WGM11);									// 0001: 8λ��λ����PWM	TOP=0x00FF
//	TCCR1A |= BIT(WGM11);									// 0011: 10λ��λ����PWM TOP=0x03FF
	TCCR1B &=~BIT(WGM12);									// 0101��8λ����PWM	TOP=0x00FF
//	TCCR1B |= BIT(WGM12);									// 0111: 10λ����PWM TOP=0x03FF
	TCCR1B &=~BIT(WGM13);									// 1110: ����PWM TOP=ICRn
//	TCCR1B |= BIT(WGM13);									// 1111: ����PWM  TOP=OCRnA

//	TCCR1B |= BIT(ICNC1);									// ���벶׽����������
//	TCCR1B |= BIT(ICES1);									// ���벶׽������ѡ�� 1:ѡ������߼���ƽ�������ش������벶׽
//	TCCR1B |= 0x00;											// ��ʱ�� T/C ������ ���·�Ƶϵ������λ����ģʽ�¼���
	TCCR1B |= 0x01;											// ��ʱ����������Ƶ�ȣ�clkT0S/1	   (û��Ԥ��Ƶ)
//	TCCR1B |= 0x02;											// ��ʱ����������Ƶ�ȣ�clkT0S/8	   (����Ԥ��Ƶ��) 2710.588K	@11.0592Mhz
//	TCCR1B |= 0x03;											// ��ʱ����������Ƶ�ȣ�clkT0S/64   (����Ԥ��Ƶ��)  @11.0592Mhz
//	TCCR1B |= 0x04;											// ��ʱ����������Ƶ�ȣ�clkT0S/256  (����Ԥ��Ƶ��)  @11.0592Mhz
//	TCCR1B |= 0x05;											// ��ʱ����������Ƶ�ȣ�clkT0S/1024 (����Ԥ��Ƶ��)
//	TCCR1B |= 0x06;											// ��ʱ����������T1	���ŵ��ⲿʱ��Դ��ʱ��Ϊ�½���
//	TCCR1B |= 0x07;											// ��ʱ����������T1	���ŵ��ⲿʱ��Դ��ʱ��Ϊ������
//	TCCR1B |= 0x0F;											// ��ʱ����������T1	���ŵ��ⲿʱ��Դ��ʱ��Ϊ������ �Ƚ�ƥ�䴥������

//	TCCR1C |= BIT(FOC1A);									// ǿ�ƱȽ��������A
//	TCCR1C |= BIT(FOC1B);									// ǿ�ƱȽ��������B

//	TIFR  |= BIT(ICF1);										// ���T/C1���벶׽��־λ
//	TIFR  |= BIT(OCF1A);									// �������Ƚ��жϱ�־λ
//	TIFR  |= BIT(OCF1B);									// �������Ƚ��жϱ�־λ
//	TIFR  |= BIT(TOV1);										// ���T/C0	����жϱ�־λ

//	TIMSK  |= BIT(TICIE1);									// T/C1	���벶׽�ж�ʹ��
//	TIMSK  |= BIT(OCIE1A);									// T/C1	����Ƚ� A ƥ���ж�ʹ��
//	TIMSK  |= BIT(OCIE1B);									// T/C1	����Ƚ� B ƥ���ж�ʹ��
//	TIMSK  |= BIT(TOIE1);									// T/C1	����ж�ʹ��

//	b_Timer1 = false;										// ʱ�䵽��ʶ��ʼ��
}

/************************************************************/
// TIMER2 initialize - prescale:1
// WGM:	PWM	Phase correct ������PWM���
// desired value: 21KHz
// actual value: 21.685KHz (3.2%)
void Timer2_Init(void)
{
//	PORTB |= BIT(PB3);										// OC2 ���1
//	DDRB  |= BIT(PB3);										// OC2 ��1���
	PORTD |= BIT(PD7);										// OC2 ���1
	DDRD  |= BIT(PD7);										// OC2 ��1���

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

	//TIFR  |= BIT(TOV2);										// ���T/C2	����жϱ�־λ
	//TIMSK |= BIT(TOIE2);									// timer interrupt sources
}

/************************************************************/
// ˵����Adc	Initialize
// Conversion time:	104uS
void Adc_Init(void)
{
//	PORTC &=~0x00;											// �������費ʹ��
//	DDRC   = 0x00;											// ADCͨ������I/O����Ϊ����
	ADCSRA = 0x00;											// disable adc

	ADMUX  = 0x00;											// ѡ��ͨ��ʱ�ȹر�ADC�͹رձȽ��� Ĭ��ѡ��0ͨ���������Ҷ���
//	ADMUX &=~BIT(REFS0);									// �ο���ѹѡ��	 REFS1 REFS0
	ADMUX |= BIT(REFS0);									// 00���ⲿAREF���ڲ�Vref �ر�	10������
	ADMUX &=~BIT(REFS1);									// 01��AVCC��AREF��������˲�����
//	ADMUX |= BIT(REFS1);									// 11��2.56V ��Ƭ�ڻ�׼��ѹԴ��AREF��������˲�����

//	ADMUX &=~BIT(ADLAR);									// ѡ���Ҷ���
	ADMUX |= BIT(ADLAR);									// ѡ�������

//	ADCSRA = 0x01;											// ��Ƶ�ȣ�2 ����ADC=fc/2 �߲����ʣ��;���
//	ADCSRA = 0x02;											// ��Ƶ�ȣ�4 ����ADC=fc/4 ��Ƶ��Խ��,����Խ��,������Խ�͡�
//	ADCSRA = 0x03;											// ��Ƶ�ȣ�8 ����ADC=fc/8
//	ADCSRA = 0x04;											// ��Ƶ�ȣ�16 ����ADC=fc/16
//	ADCSRA = 0x05;											// ��Ƶ�ȣ�32 ����ADC=fc/32
	ADCSRA = 0x06;											// ��Ƶ�ȣ�64 ����ADC=fc/64	  ����ʱ��75uS	@11.0592Mhz
	//ADCSRA = 0x07;											// ��Ƶ�ȣ�128 ����ADC=fc/128 ����ʱ��150uS	@11.0592Mhz
	ADCSRA |= BIT(ADEN);									// ADCʹ�� ADC������Ҫ 13��ADCʱ��
//	ADCSRA |= BIT(ADFR);									// ��������
	ADCSRA |= BIT(ADIF);									// ���ADC�ж�
	ADCSRA |= BIT(ADIE);									// ��ADC�ж�

	ADCSRA |= BIT(ADSC);									// ����ADC ������һ��ADCʱ���ڲ�Ҫ�л�ADC����ͨ����

	g_Adc_Num =	0;											// ��ADCͨ��������
	g_Adc_Sum	= 0;
}

/************************************************************/
// UART0 initialize
// desired baud	rate: 115200
// actual: baud	rate:115200	(3.7%)
void Uart0_Init(void)
{
	PORTD |= BIT(PD0);										// ��1����	(���������쳣)
	DDRD  &=~BIT(PD0);										// RXD0	��0����
	PORTD |= BIT(PD1);										// ���1
	DDRD  |= BIT(PD1);										// TXD0	��1���

	UCSRB =	0x00; 											//disable	while setting baud rate
	UCSRA =	0x02;
	UCSRC =	BIT(URSEL) | 0x06;
	UBRRL =	0x0B; 	   	 	  								//set	baud rate lo
	UBRRH =	0x00; 											//set	baud rate hi
	UCSRB =	0x98;

	UCSRB |= BIT(RXEN);										// ������ʹ��
	UCSRB |= BIT(TXEN);										// ������ʹ��
	UCSRB |= BIT(RXCIE);									// �����ж�ʹ��
//	UCSRB |= BIT(TXCIE);									// �����ж�ʹ��
//	UCSRB |= BIT(UDRIE);									// USART ���ݼĴ������ж�ʹ��
//	UCSRB |= BIT(UCSZ2);									// USART ����9λ

	g_Rxd_Buf_Num =	0;
	b_Rxd_Data = false;										// �������ݽ�����ɱ�־λ
}

/************************************************************/
// SPI initialize
// clock rate: 5000000hz
void SPI_Initial( void )
{
	PORTB |= BIT(PB5);										// RF_MOSI��ʼ���˿� ���������1����
	DDRB  |= BIT(PB5);										// RF_MOSI��ʼ���˿� ��1���

	PORTB |= BIT(PB6);										// RF_MISO��ʼ���˿� ���������1����
	DDRB  &=~BIT(PB6);										// RF_MISO��ʼ���˿� ��0����

	PORTB |= BIT(PB7);										// RF_SCK��ʼ���˿�	���������1����
	DDRB  |= BIT(PB7);										// RF_SCK��ʼ���˿�	��1���

	PORTB |= BIT(PB4);										// CAN_CSN��ʼ���˿� ���������1����
	DDRB  |= BIT(PB4);										// CAN_CSN��ʼ���˿� ��1���
															// IRQ�ж�IO��ʼ�����жϳ�ʼ������
	SPCR = 0x50;											// setup SPI ģʽ0
	SPSR = 0x01;											// setup SPI bit clock rate	5529600Hz �������
}

/************************************************************/
// SPI�ӿڶ�д����
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
// ˵����΢����ʱ
// ���룺g_Us:��ʱ��ʱ��
// �������
void Delay_Us( u08 g_Us	)									// us ��ʱ
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
//˵����������ʱ
//���룺g_Ms:��ʱ��ʱ��
//�������
void Delay_ms( u16 g_ms	)									// ms ��ʱ
{
#define	c_Delay_Ms		(u16)(c_FC_OSC/8000)-4				// 1��ѭ����Լ8�����ڼ��㷽����ʱ��/8000���ټ�ȥ������ʱ������g_Ms������

	u16	j;													// �ر�Ҫע�� ��static�����ı����������Ĵ����ǲ�һ���ġ��ر���������ʱ����

	for( ; g_ms	> 0; g_ms--	)
	{
		for( j = 0;	j <	c_Delay_Ms;	j++	);					// ���ݷ���һ��˫�ֽڱ�����forѭ����Լ8������
	}
}

/************************************************************/
//˵��������д���ֽ����ݺ��� Ĭ�ϴ���1
//���룺g_Dat:������д������
//�������
void Putdata( u08 g_Dat	)									// ����д���� s_Dat:������д������
{
	while( !( UCSRA	& (BIT(UDRE))) );						// �ȴ����ͽ���	�ȴ����ͻ�������
	UDR	= g_Dat;											// �����ݷ����жϴ��ͽ�����֮��������Ч��
}

/************************************************************/
//˵��������дn���ֽ����ݺ��� Ĭ�ϴ���1
//���룺g_Dat:������д������ָ�룬g_DatSize���ֽ���
//�������
void Putdatas( u08 *g_Dat, u08 g_DatSize )
{
	while( g_DatSize )
	{
		while( !( UCSRA	& (BIT(UDRE))) );					// �ȴ����ͽ���	�ȴ����ͻ�������
		UDR	= *g_Dat;										// �����ݷ����жϴ��ͽ�����֮��������Ч��
		g_Dat++;
		g_DatSize--;
	}
}

/************************************************************/
//˵��������д�ַ������� Ĭ�ϴ���1
//���룺g_String:������д���ַ�������ָ��
//�������
void Printf( c08 *g_String )								// ����д���� s_String:������д���ַ�������ָ��
{
	while( *g_String )
	{
		while( !( UCSRA	& (BIT(UDRE))) );					// �ȴ����ͽ���	�ȴ����ͻ�������
		UDR	= *g_String;									// �����ݷ����жϴ��ͽ�����֮��������Ч��
		g_String++;
	}
//	Putdata( '\r' );										// ����һ���س���
}


/************************************************************/
//˵����call this routine to initialize	all	peripherals
//���룺��
//����������ʼ��
/************************************************************/
void Init_Board(void)
{
	CLI();													// disable all interrupts��stop	errant interrupts until	set	up
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
	Lamp_Init();											// �����Ʋ�����ʼ��
  //  Moto_Init();                                            // ���������ʼ��
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
//	Debug_Toggle();											// ����ʱ����
}

#pragma	interrupt_handler timer2_ovf_isr:iv_TIM2_OVF
// desired value: 1mS
void timer2_ovf_isr(void)
{
 	TCNT2 = 0x54; //reload counter value

}

/************************************************************/
//˵����ADC	conversion complete
//���룺��
//�������
/************************************************************/
#pragma	interrupt_handler Adc_Isr:			iv_ADC
/************************************************************/
void Adc_Isr(void)
{
	static u08	j = 0;
//	static u08  g_State_Temp = 10;
	g_Adc_Data[g_Adc_Num++] +=	ADCH;							// ���β���ֵ��ֻ����8λ����׼
//	g_Adc_Num ++;											// ���ADC������Ҫ����ADCͨ��

	if(	g_Adc_Num == c_ADC_NUM )							// �ж�����Ĳ�����ADC·���Ƿ�ȫ�����
	{
		/*-------�����ɼ�------------*/

	//  g_Adc_Sum ++;
	  if(++g_Adc_Sum>=4)
	  {
	  	for(j=0;j<c_ADC_NUM;j++)
	  	{
	  		g_Adc_Buf[j] =(u08)(g_Adc_Data[j]>>2);         //8�β�����ƽ��ֵ���ٶȼ�����AD�жϴ���Щʲô����

	  		g_Adc_Data[j] = 0;
	  	}

	  	g_Adc_Sum = 0;
		ADC_SampHole_Flag = 1;            					//�������

	  }


		g_Adc_Num =	0x00;									// ADC����ͨ������������
		ADMUX &= 0xE0 ;										// ����ѡ��ADC����ͨ��	�����λ5bit
		ADCSRA |= BIT(ADSC);								// ����ADC����������
		return;												// �˳��ж�
	}
	ADMUX++;												// ѡ�����ͨ��	��3λ0-7Ϊͨ�����������üӷ��л�ͨ��
	ADCSRA |= BIT(ADSC);									// ����ADC ģ�����������Ĵ���
}

/************************************************************/
//˵����uart has received a	character
//���룺��
//�������
/************************************************************/
#pragma	interrupt_handler Uart0_Rx_Isr:iv_USART0_RXC
/************************************************************/
void Uart0_Rx_Isr(void)
{
	g_Rxd_Buf[g_Rxd_Buf_Num] = UDR;							// ������ʱ������

	if(	g_Rxd_Buf[0]==0xAA )
	{
		g_Rxd_Buf_Num ++;
		if(	g_Rxd_Buf_Num == c_RXD_BUF_NUM )				// �������Ƿ����ˣ�������������������
		{
			g_Rxd_Buf_Num =	0x00;
			b_Rxd_Data = true;								// true���������ݽ������
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
