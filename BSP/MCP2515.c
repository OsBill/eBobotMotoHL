/**********************************************************************************
 * �ļ���  ��MCP2515.c
 * ����	   ��MCP2515����������
**********************************************************************************/
#include "includes.h"


/*	�ⲿ�������� */											//
extern u08 SPI_RW(u08 dat);									// SPI��д����

// CAN����ID����
#define	c_TXB0SIDH	0x95									// ����ID TX Buffer0 ��׼֡ID SID10��SID3
#define	c_TXB0SIDL	0x55									// ����ID TX Buffer0 ��׼֡ID ��3bit��Ӧ SID2��SID0
#define	c_RXF0EID8	0x60									// ����ID ���ø�4λ����4λû���� RXBUFF0��2������ID
#define	c_RXF1EID8	0xF0									// ����ID ���ø�4λ����4λû���� RXBUFF0��2������ID
#define	c_RXF2EID8	0x61									// ����ID   RXBUFF1��4������ID
#define	c_RXF3EID8	0xF1									// ����ID   RXBUFF0��2������ID


// MCP2515ϵͳ���� 8Mhzʱ����MCP2515������Ԥ��Ƶ(ʱ��16Mhzʱ���²�����Ҫ��2)
#define	CAN_10Kbps	0x31
#define	CAN_25Kbps	0x13
#define	CAN_50Kbps	0x09
#define	CAN_100Kbps	0x04
#define	CAN_125Kbps	0x03
#define	CAN_250Kbps	0x01
#define	CAN_500Kbps	0x00

#define CAN_RECEIVE_BUFFER_SIZE												128

unsigned char g_pu08CANReceiveBuffer[CAN_RECEIVE_BUFFER_SIZE]				= {0};
unsigned char g_u08CANReceiveBufferWriteIndex	= 0;
unsigned char g_u08CANReceiveBufferReadIndex	= 0;

/*******************************************************************************
* ������  :	Delay_Nms
* ����	  :	ͨ�������ʱԼnms(��׼ȷ)
* ����	  :	��
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	�˷�ʽ��ʱʱ���ǲ�׼ȷ��,׼ȷ��ʱ�����ö�ʱ��
*******************************************************************************/
void Delay_Nms(unsigned	int	x)
{
	unsigned int y;

	for	(;x>0;x--)
		for	(y=0;y<100;y++);
}

/*******************************************************************************
* ������  :	MCP2515_WriteReg
* ����	  :	ͨ��SPI��MCP2515ָ����ַ�Ĵ���д1���ֽ�����
* ����	  :	addr:MCP2515�Ĵ�����ַ,dat:��д�������
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	��
*******************************************************************************/
void MCP2515_WriteReg(unsigned char	addr,unsigned char dat)
{
	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_WRITE);										// ����д����
	SPI_RW(addr);											// ���͵�ַ
	SPI_RW(dat);											// д������
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  :	MCP2515_WriteRegN
* ����	  :	ͨ��SPI��MCP2515ָ����ַ�Ĵ���дN���ֽ�����
* ����	  :	addr:MCP2515�Ĵ����׵�ַ,DataBuff:��д������ݻ�����,n��д�����ݸ���
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	��
*******************************************************************************/
void MCP2515_WriteRegN(unsigned	char addr,unsigned char	*DataBuff,unsigned char	n)
{
	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_WRITE);										// ����д����
	SPI_RW(addr);											// ���͵�ַ
	while(n)
	{
		SPI_RW(*DataBuff);									// ����д������
		DataBuff++;
		n--;
	}
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  :	MCP2515_ReadReg
* ����	  :	ͨ��SPI��MCP2515ָ����ַ������1���ֽ�����
* ����	  :	addr:MCP2515�Ĵ�����ַ
* ���	  :	��
* ����ֵ  :	rByte:��ȡ���Ĵ�����1���ֽ�����
* ˵��	  :	��
*******************************************************************************/
unsigned char MCP2515_ReadReg(unsigned char	addr)
{
	unsigned char rByte;

	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_READ);										// ���Ͷ�����
	SPI_RW(addr);											// ���͵�ַ
	rByte=SPI_RW(0);										// ��ȡ����
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
	return rByte;											// ���ض�����һ���ֽ�����
}

/*******************************************************************************
* ������  :	MCP2515_ReadRegN
* ����	  :	ͨ��SPI��MCP2515ָ����ַ������N���ֽ�����
* ����	  :	addr:MCP2515�Ĵ�����ַ,,DataBuff:n����ȡ���ݸ���
* ���	  :	��
* ����ֵ  :	rByte:��ȡ���Ĵ�����N���ֽ����ݻ�����
* ˵��	  :	��
*******************************************************************************/
void MCP2515_ReadRegN(unsigned char	addr,unsigned char *DataBuff,unsigned char n)
{
	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_READ);										// ���Ͷ�����
	SPI_RW(addr);											// ���͵�ַ
	while(n)
	{
		*DataBuff=SPI_RW(0);								// ������ȡ����
		DataBuff++;
		n--;
	}
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  :	MCP2515_Reset
* ����	  :	���͸�λָ�������λMCP2515
* ����	  :	��
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	���ڲ��Ĵ�����λΪȱʡ״̬,���������趨Ϊ����ģʽ
*******************************************************************************/
void MCP2515_Reset(void)
{
	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_RESET);										// ���ͼĴ�����λ����
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
}

void MCP2515_BitModify(unsigned char regAddr,unsigned char mask, unsigned char data)
{
	MCP2515_CS_Low();
	SPI_RW(CAN_BIT_MODIFY);
	SPI_RW(regAddr);
	SPI_RW(mask);
	SPI_RW(data);
	MCP2515_CS_High();
}

/*******************************************************************************
* ������  :	MCP2515_Init
* ����	  :	MCP2515��ʼ������
* ����	  :	��
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	��ʼ�������������λ���������������á���ʶ��������õȡ�
*******************************************************************************/
void MCP2515_Init(void)
{
	unsigned char temp=0;

	MCP2515_Reset();										// ���͸�λָ�������λMCP2515
	Delay_Nms(1);											// ͨ�������ʱԼnms(��׼ȷ)

	// TQ/Bit��5��25֮�� һ��16������ ȫ���ĸ�ʱ��ε�TQ�����������5��25֮�䡣���õĿ�����ѡ��ÿλ16��TQ
	// ���SJW�ή�Ͷ������ݲ��Ҫ��
	// TPROP = ����ʱ(ns) PRSEG��Сֵ =	TPROP/TQ��
	// ����ʹ��40m�������ߣ�TPROP =	870	ns��PRSEG��Сֵ	= TPROP/TQ = 6.96 TQ�����PRSEGѡ��7������ʹ��40m��������
	// ����ɲο�����MCP25625������23ҳ
	// ���ò�����Ϊ125Kbps
	MCP2515_WriteReg(CNF1,SJW_1TQ|CAN_500Kbps);				// set CNF1,SJW=3,ͬ����(SyncSeg)Ϊ1TQ,BRP=0,TQ=[2*(BRP+1)]/Fsoc=2/16M=0.125us
	MCP2515_WriteReg(CNF2,0x80|PHSEG1_3TQ|PRSEG_2TQ);		// set CNF2,SAM=0,�ڲ���������߽���һ�β�����PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
	MCP2515_WriteReg(CNF3,PHSEG2_2TQ);						// set CNF3,PHSEG2=(2+1)TQ=3TQ,ͬʱ��CANCTRL.CLKEN=1ʱ�趨CLKOUT����Ϊʱ�����ʹ��λ

	// ���÷��ͻ�����0
	MCP2515_WriteReg(TXB0CTRL,0x03);						// ���ͻ�����0��ߵı��ķ������ȼ�
	MCP2515_WriteReg(TXB0DLC,0x04);							// �����ݳ���д�뷢�ͻ�����0�ķ��ͳ��ȼĴ��� 4���ֽ�����
	MCP2515_WriteReg(TXB0SIDH,c_TXB0SIDH);					// ���ͻ�����0��׼��ʶ����λ ����ID	Ϊ�����bitͬ���ɿ��ԣ�����bit������������Ϊ1������Ϊ0
	MCP2515_WriteReg(TXB0SIDL,c_TXB0SIDL&(~EXIDE));			// ���ͻ�����0��׼��ʶ����λ ����ID	bit3����չ֡��־λ ��׼֡
//	MCP2515_WriteReg(TXB0SIDL,c_TXB0SIDL|(EXIDE));			// ���ͻ�����0��׼��ʶ����λ ����ID	bit3����չ֡��־λ ��չ֡

	// ���÷��ͻ�����1
	MCP2515_WriteReg(TXB1CTRL,0x03);						// ���ͻ�����1��ߵı��ķ������ȼ�
	MCP2515_WriteReg(TXB1DLC,0x04);							// �����ݳ���д�뷢�ͻ�����1�ķ��ͳ��ȼĴ��� 4���ֽ�����
	MCP2515_WriteReg(TXB1SIDH,c_TXB0SIDH);					// ���ͻ�����1��׼��ʶ����λ ����ID	Ϊ�����bitͬ���ɿ��ԣ�����bit������������Ϊ1������Ϊ0
	MCP2515_WriteReg(TXB1SIDL,c_TXB0SIDL&(~EXIDE));			// ���ͻ�����1��׼��ʶ����λ ����ID	bit3����չ֡��־λ ��׼֡
//	MCP2515_WriteReg(TXB1SIDL,c_TXB0SIDL|(EXIDE));			// ���ͻ�����1��׼��ʶ����λ ����ID	bit3����չ֡��־λ ��չ֡

	// ���÷��ͻ�����2
	MCP2515_WriteReg(TXB2CTRL,0x02);						// ���ͻ�����2�ϵ͵ı��ķ������ȼ�
	MCP2515_WriteReg(TXB2DLC,0x04);							// �����ݳ���д�뷢�ͻ�����1�ķ��ͳ��ȼĴ��� 4���ֽ�����
	MCP2515_WriteReg(TXB2SIDH,c_TXB0SIDH);					// ���ͻ�����1��׼��ʶ����λ ����ID	Ϊ�����bitͬ���ɿ��ԣ�����bit������������Ϊ1������Ϊ0
	MCP2515_WriteReg(TXB2SIDL,c_TXB0SIDL&(~EXIDE));			// ���ͻ�����1��׼��ʶ����λ ����ID	bit3����չ֡��־λ ��׼֡
//	MCP2515_WriteReg(TXB1SIDL,c_TXB0SIDL|(EXIDE));			// ���ͻ�����1��׼��ʶ����λ ����ID	bit3����չ֡��־λ ��չ֡

	// ���ý��ջ�����0
	MCP2515_WriteReg(RXB0SIDH,0x00);						// ��ս��ջ�����0�ı�׼��ʶ����λ
	MCP2515_WriteReg(RXB0SIDL,0x00);						// ��ս��ջ�����0�ı�׼��ʶ����λ
	MCP2515_WriteReg(RXB0CTRL,0x04);						// ���շ����˲������������д���չ��ʶ�����׼��ʶ������Ч���ġ�
// ��չ��ʶ���˲����Ĵ���RXFnEID8:RXFnEID0���ڴ��б�׼��ʶ���ı����е�ǰ���������ֽڡ�
//	MCP2515_WriteReg(RXB0DLC,DLC_4);						// ���ý������ݵĳ���Ϊ4���ֽ�
	MCP2515_WriteReg(RXF0SIDH,0x00);						// ���������˲��Ĵ���n��׼��ʶ����λ ����buffer0 û��ʹ��
	MCP2515_WriteReg(RXF0SIDL,0x00);						// ���������˲��Ĵ���n��׼��ʶ����λ ����buffer0 û��ʹ��
	MCP2515_WriteReg(RXF0EID8,c_RXF1EID8);					// ���������˲��Ĵ���n��չ��ʶ����λ ��׼֡Ӧ���������ֽ�0 ���ø�4λ����4λû����
	MCP2515_WriteReg(RXF0EID0,0x00);						// ���������˲��Ĵ���n��չ��ʶ����λ ��׼֡Ӧ���������ֽ�1 û��ʹ��

	MCP2515_WriteReg(RXF1SIDH,0x00);						// ���������˲��Ĵ���n��׼��ʶ����λ ����buffer0 û��ʹ��
	MCP2515_WriteReg(RXF1SIDL,0x00);						// ���������˲��Ĵ���n��׼��ʶ����λ ����buffer0 û��ʹ��
	MCP2515_WriteReg(RXF1EID8,c_RXF0EID8);					// ���������˲��Ĵ���n��չ��ʶ����λ ��׼֡Ӧ���������ֽ�0 ���ø�4λ����4λû����
	MCP2515_WriteReg(RXF1EID0,0x00);						// ���������˲��Ĵ���n��չ��ʶ����λ ��׼֡Ӧ���������ֽ�1 û��ʹ��

	MCP2515_WriteReg(RXM0SIDH,0x00);						// �����������μĴ���n��׼��ʶ����λ ����buffer0 û��ʹ��
	MCP2515_WriteReg(RXM0SIDL,0x00);						// �����������μĴ���n��׼��ʶ����λ ����buffer0 û��ʹ��
	MCP2515_WriteReg(RXM0EID8,0xFF);						// �����������μĴ���n��չ��ʶ����λ ���ø�4λ����4λû����
	MCP2515_WriteReg(RXM0EID0,0x00);						// �����������μĴ���n��չ��ʶ����λ û��ʹ��


	// ���ý��ջ�����1,����ͨ��2��3
	MCP2515_WriteReg(RXB1SIDH,0x00);						// ��ս��ջ�����1�ı�׼��ʶ����λ
	MCP2515_WriteReg(RXB1SIDL,0x00);						// ��ս��ջ�����1�ı�׼��ʶ����λ
	MCP2515_WriteReg(RXB1CTRL,0x00);						// ���շ����˲������������д���չ��ʶ�����׼��ʶ������Ч���ġ�

	MCP2515_WriteReg(RXF2SIDH,0x00);						// ���������˲��Ĵ���n��׼��ʶ����λ ����buffer1 û��ʹ��
	MCP2515_WriteReg(RXF2SIDL,0x00);						// ���������˲��Ĵ���n��׼��ʶ����λ ����buffer1 û��ʹ��
	MCP2515_WriteReg(RXF2EID8,c_RXF2EID8);					// ���������˲��Ĵ���n��չ��ʶ����λ ��׼֡Ӧ���������ֽ�0 ���ø�4λ����4λû����
	MCP2515_WriteReg(RXF2EID0,0x00);						// ���������˲��Ĵ���n��չ��ʶ����λ ��׼֡Ӧ���������ֽ�1 û��ʹ��

	MCP2515_WriteReg(RXF3SIDH,0x00);						// ���������˲��Ĵ���n��׼��ʶ����λ ����buffer1 û��ʹ��
	MCP2515_WriteReg(RXF3SIDL,0x00);						// ���������˲��Ĵ���n��׼��ʶ����λ ����buffer1 û��ʹ��
	MCP2515_WriteReg(RXF3EID8,c_RXF3EID8);					// ���������˲��Ĵ���n��չ��ʶ����λ ��׼֡Ӧ���������ֽ�0 ���ø�4λ����4λû����
	MCP2515_WriteReg(RXF3EID0,0x00);						// ���������˲��Ĵ���n��չ��ʶ����λ ��׼֡Ӧ���������ֽ�1 û��ʹ��

	MCP2515_WriteReg(RXM1SIDH,0x00);						// �����������μĴ���n��׼��ʶ����λ ����buffer1 û��ʹ��
	MCP2515_WriteReg(RXM1SIDL,0x00);						// �����������μĴ���n��׼��ʶ����λ ����buffer1 û��ʹ��
	MCP2515_WriteReg(RXM1EID8,0xFF);						// �����������μĴ���n��չ��ʶ����λ ���ø�4λ����4λû����				   ��
	MCP2515_WriteReg(RXM1EID0,0xF0);						// �����������μĴ���n��չ��ʶ����λ û��ʹ��

	// �����ж�
	MCP2515_WriteReg(CANINTF,0x00);							// ���CAN�жϱ�־�Ĵ���������λ(������MCU���)
	MCP2515_WriteReg(CANINTE,RX0IE|RX1IE);						// ����CAN�ж�ʹ�ܼĴ����Ľ��ջ�����0���ж�RX0IEʹ��,����λ��ֹ�ж�

	// ���ù���ģʽ
	MCP2515_WriteReg(CANCTRL,REQOP_NORMAL);					// ��MCP2515����Ϊ����ģʽ,�˳�����ģʽ
//	MCP2515_WriteReg(CANCTRL,REQOP_LOOPBACK);				// ��MCP2515����Ϊ����ģʽ,�˳�����ģʽ

	// ȷ�Ϲ���ģʽ
	temp=MCP2515_ReadReg(CANSTAT);							// ��ȡCAN״̬�Ĵ�����ֵ
	if(OPMODE_NORMAL!=(temp&&0xE0))							// �ж�MCP2515�Ƿ��Ѿ���������ģʽ
	{
		MCP2515_WriteReg(CANCTRL,REQOP_NORMAL);				// �ٴν�MCP2515����Ϊ����ģʽ,�˳�����ģʽ
//		MCP2515_WriteReg(CANCTRL,REQOP_LOOPBACK);			// �ٴν�MCP2515����Ϊ����ģʽ,�˳�����ģʽ
	}
}

/*******************************************************************************
* ������  :	CAN_Send_Buffer0
* ����	  :	CAN���ͳ���4������
* ����	  :	*CAN_TX_Buf(���������ݻ�����ָ��))
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	������4���ֽڵ����ݣ�ǰ���ʼ��ʱ��ȷ��һ��
*******************************************************************************/
void CAN_Send_Buffer0(unsigned char	*CAN_TX_Buf)
{
	CLI();
	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_LOAD_TXD0);									// ����ת����������	TX������0,��ʼ��TXB0D0
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ

	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_RTS_TXB0);									// ֱ�������ͱ���0����
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
	SEI();
}

/*******************************************************************************
* ������  :	CAN_Send_Buffer1
* ����	  :	CAN���ͳ���4������
* ����	  :	*CAN_TX_Buf(���������ݻ�����ָ��))
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	������4���ֽڵ����ݣ�ǰ���ʼ��ʱ��ȷ��һ��
*******************************************************************************/
void CAN_Send_Buffer1(unsigned char	*CAN_TX_Buf)
{
	CLI();
	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_LOAD_TXD1);									// ����ת����������	TX������0,��ʼ��TXB0D0
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ

	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_RTS_TXB1);									// ֱ�������ͱ���0����
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
	SEI();
}

/*******************************************************************************
* ������  :	CAN_Send_Buffer2
* ����	  :	CAN���ͳ���4������
* ����	  :	*CAN_TX_Buf(���������ݻ�����ָ��))
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	������4���ֽڵ����ݣ�ǰ���ʼ��ʱ��ȷ��һ��
*******************************************************************************/
void CAN_Send_Buffer2(unsigned char	*CAN_TX_Buf)
{
	CLI();
	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_LOAD_TXD2);									// ����ת����������	TX������0,��ʼ��TXB0D0
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// ����д������
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ

	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ
	SPI_RW(CAN_RTS_TXB2);									// ֱ�������ͱ���0����
	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
	SEI();
}

/*******************************************************************************
* ������  :	CAN_Send_Buffer
* ����	  :	CAN����ָ�����ȵ�����
* ����	  :	*CAN_TX_Buf(���������ݻ�����ָ��),len(���������ݳ���)
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	��
*******************************************************************************/
void CAN_Send_Buffer(unsigned char *CAN_TX_Buf,unsigned	char len)
{
	unsigned char j,dly,count;

	count=0;

	CLI();

	while(count<len)
	{
		dly=0;
		while((MCP2515_ReadReg(TXB0CTRL)&0x08) && (dly<50))	// ���ٶ�ĳЩ״ָ̬��,�ȴ�TXREQ��־����
		{
			Delay_Nms(1);									// ͨ�������ʱԼnms(��׼ȷ)
			dly++;
		}

		for(j=0;j<8;)
		{
			MCP2515_WriteReg(TXB0D0+j,CAN_TX_Buf[count++]);	// �������͵�����д�뷢�ͻ���Ĵ���
			j++;
			if(count>=len) break;
		}
		MCP2515_WriteReg(TXB0DLC,j);						// ����֡�����͵����ݳ���д�뷢�ͻ�����0�ķ��ͳ��ȼĴ���
//		MCP2515_CS_Low();
		MCP2515_WriteReg(TXB0CTRL,0x08);					// �����ͱ���
//		MCP2515_CS_High();
	}

	SEI();
}

/*******************************************************************************
* ������  :	CAN_Receive_Buffer0
* ����	  :	CAN����һ֡����
* ����	  :	*CAN_TX_Buf(���������ݻ�����ָ��)
* ���	  :	��
* ����ֵ  :	��
* ˵��	  :	������4���ֽڵ����ݣ�ǰ���ʼ��ʱ��ȷ��һ��
*******************************************************************************/

void CAN_Receive_Buffer0(unsigned char *CAN_RX_Buf,unsigned char CAN_RD_RXDn)
{
	MCP2515_CS_Low();										// ��MCP2515��CSΪ�͵�ƽ

	SPI_RW(CAN_RD_RXDn);									// ����ת����������	RX������0,��ʼ��RXB0D0
	*CAN_RX_Buf	= SPI_RW(0x55);								// ������������1
	CAN_RX_Buf++;
	*CAN_RX_Buf	= SPI_RW(0x55);								// ������������1
	CAN_RX_Buf++;
	*CAN_RX_Buf	= SPI_RW(0x55);								// ������������1
	CAN_RX_Buf++;
	*CAN_RX_Buf	= SPI_RW(0x55);								// ������������1


	MCP2515_CS_High();										// ��MCP2515��CSΪ�ߵ�ƽ
															// ������CS	����Ϊ�ߵ�ƽ����ص�RX ��־λ��CANINTF.RXnIF�������Զ�����
}

/*******************************************************************************
* ������  :	CAN_Receive_Buffer
* ����	  :	CAN����һ֡����
* ����	  :	*CAN_TX_Buf(���������ݻ�����ָ��)
* ���	  :	��
* ����ֵ  :	len(���յ����ݵĳ���,0~8�ֽ�)
* ˵��	  :	��
*******************************************************************************/
unsigned char CAN_Receive_Buffer(unsigned char *CAN_RX_Buf)
{
	unsigned char i=0,len=0,temp=0;

	temp = MCP2515_ReadReg(CANINTF);

	if(temp	& RX0IE)										// �ж��ǽ��ջ�����0 ���ж�RX0IF
	{
		len=MCP2515_ReadReg(RXB0DLC);						// ��ȡ���ջ�����0���յ������ݳ���(0~8���ֽ�)
		while(i<len)
		{
			CAN_RX_Buf[i]=MCP2515_ReadReg(RXB0D0+i);		// ��CAN���յ������ݷ���ָ��������
			i++;
		}
	}

	MCP2515_WriteReg(CANINTF,0);							// ����жϱ�־λ(�жϱ�־�Ĵ���������MCU����)

	return len;
}

void CAN_ReceiveIRQHandler(void)
{


	if (MCP2515_ReadReg(CANINTF) & RX0IF)
	{
	//	MCP2515_WriteReg(CANINTF,0x00);
	//	MCP2515_BitModify(CANINTF, RX0IF, 0);
		MCP2515_BitModify(CANINTF, 0xFF, 0);
		CAN_Receive_Buffer0(g_pu08CANReceiveBuffer + g_u08CANReceiveBufferWriteIndex,CAN_RD_RXD0);

		g_u08CANReceiveBufferWriteIndex += 4;

		if (g_u08CANReceiveBufferWriteIndex >= CAN_RECEIVE_BUFFER_SIZE)
		{
			g_u08CANReceiveBufferWriteIndex = 0;
		}
	}
	if (MCP2515_ReadReg(CANINTF) & RX1IF)
	{
	//	MCP2515_WriteReg(CANINTF,0x00);
		MCP2515_BitModify(CANINTF, 0xff, 0);
		CAN_Receive_Buffer0(g_pu08CANReceiveBuffer + g_u08CANReceiveBufferWriteIndex,CAN_RD_RXD1);

		g_u08CANReceiveBufferWriteIndex += 4;

		if (g_u08CANReceiveBufferWriteIndex >= CAN_RECEIVE_BUFFER_SIZE)
		{
			g_u08CANReceiveBufferWriteIndex = 0;
		}
	}



}

unsigned char CAN_ReadReceiveBuffer(unsigned char *pReadBuffer, unsigned char u08ReadSize)
{
	unsigned char u08ReadCnt;

	u08ReadCnt = 0;

	if(g_u08CANReceiveBufferReadIndex != g_u08CANReceiveBufferWriteIndex)
	{
		do
		{
			pReadBuffer[u08ReadCnt++] = g_pu08CANReceiveBuffer[g_u08CANReceiveBufferReadIndex++];

			if (g_u08CANReceiveBufferReadIndex >= CAN_RECEIVE_BUFFER_SIZE)
			{
				g_u08CANReceiveBufferReadIndex = 0;
			}

			if (u08ReadCnt == u08ReadSize)
			{
				break;
			}

		}while(g_u08CANReceiveBufferReadIndex != g_u08CANReceiveBufferWriteIndex);
	}

	return u08ReadCnt;
}
