/**********************************************************************************
 * 文件名  ：MCP2515.c
 * 描述	   ：MCP2515驱动函数库
**********************************************************************************/
#include "includes.h"


/*	外部函数声明 */											//
extern u08 SPI_RW(u08 dat);									// SPI读写函数

// CAN总线ID配置
#define	c_TXB0SIDH	0x95									// 发送ID TX Buffer0 标准帧ID SID10～SID3
#define	c_TXB0SIDL	0x55									// 发送ID TX Buffer0 标准帧ID 高3bit对应 SID2～SID0
#define	c_RXF0EID8	0x60									// 接收ID 仅用高4位，低4位没有用 RXBUFF0有2个验收ID
#define	c_RXF1EID8	0xF0									// 接收ID 仅用高4位，低4位没有用 RXBUFF0有2个验收ID
#define	c_RXF2EID8	0x61									// 接收ID   RXBUFF1有4个验收ID
#define	c_RXF3EID8	0xF1									// 接收ID   RXBUFF0有2个验收ID


// MCP2515系统配置 8Mhz时钟下MCP2515波特率预分频(时钟16Mhz时如下波特率要×2)
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
* 函数名  :	Delay_Nms
* 描述	  :	通过软件延时约nms(不准确)
* 输入	  :	无
* 输出	  :	无
* 返回值  :	无
* 说明	  :	此方式延时时间是不准确的,准确延时建议用定时器
*******************************************************************************/
void Delay_Nms(unsigned	int	x)
{
	unsigned int y;

	for	(;x>0;x--)
		for	(y=0;y<100;y++);
}

/*******************************************************************************
* 函数名  :	MCP2515_WriteReg
* 描述	  :	通过SPI向MCP2515指定地址寄存器写1个字节数据
* 输入	  :	addr:MCP2515寄存器地址,dat:待写入的数据
* 输出	  :	无
* 返回值  :	无
* 说明	  :	无
*******************************************************************************/
void MCP2515_WriteReg(unsigned char	addr,unsigned char dat)
{
	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_WRITE);										// 发送写命令
	SPI_RW(addr);											// 发送地址
	SPI_RW(dat);											// 写入数据
	MCP2515_CS_High();										// 置MCP2515的CS为高电平
}

/*******************************************************************************
* 函数名  :	MCP2515_WriteRegN
* 描述	  :	通过SPI向MCP2515指定地址寄存器写N个字节数据
* 输入	  :	addr:MCP2515寄存器首地址,DataBuff:待写入的数据缓冲区,n：写入数据个数
* 输出	  :	无
* 返回值  :	无
* 说明	  :	无
*******************************************************************************/
void MCP2515_WriteRegN(unsigned	char addr,unsigned char	*DataBuff,unsigned char	n)
{
	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_WRITE);										// 发送写命令
	SPI_RW(addr);											// 发送地址
	while(n)
	{
		SPI_RW(*DataBuff);									// 连续写入数据
		DataBuff++;
		n--;
	}
	MCP2515_CS_High();										// 置MCP2515的CS为高电平
}

/*******************************************************************************
* 函数名  :	MCP2515_ReadReg
* 描述	  :	通过SPI从MCP2515指定地址寄器读1个字节数据
* 输入	  :	addr:MCP2515寄存器地址
* 输出	  :	无
* 返回值  :	rByte:读取到寄存器的1个字节数据
* 说明	  :	无
*******************************************************************************/
unsigned char MCP2515_ReadReg(unsigned char	addr)
{
	unsigned char rByte;

	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_READ);										// 发送读命令
	SPI_RW(addr);											// 发送地址
	rByte=SPI_RW(0);										// 读取数据
	MCP2515_CS_High();										// 置MCP2515的CS为高电平
	return rByte;											// 返回读到的一个字节数据
}

/*******************************************************************************
* 函数名  :	MCP2515_ReadRegN
* 描述	  :	通过SPI从MCP2515指定地址寄器读N个字节数据
* 输入	  :	addr:MCP2515寄存器地址,,DataBuff:n：读取数据个数
* 输出	  :	无
* 返回值  :	rByte:读取到寄存器的N个字节数据缓冲区
* 说明	  :	无
*******************************************************************************/
void MCP2515_ReadRegN(unsigned char	addr,unsigned char *DataBuff,unsigned char n)
{
	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_READ);										// 发送读命令
	SPI_RW(addr);											// 发送地址
	while(n)
	{
		*DataBuff=SPI_RW(0);								// 连续读取数据
		DataBuff++;
		n--;
	}
	MCP2515_CS_High();										// 置MCP2515的CS为高电平
}

/*******************************************************************************
* 函数名  :	MCP2515_Reset
* 描述	  :	发送复位指令软件复位MCP2515
* 输入	  :	无
* 输出	  :	无
* 返回值  :	无
* 说明	  :	将内部寄存器复位为缺省状态,并将器件设定为配置模式
*******************************************************************************/
void MCP2515_Reset(void)
{
	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_RESET);										// 发送寄存器复位命令
	MCP2515_CS_High();										// 置MCP2515的CS为高电平
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
* 函数名  :	MCP2515_Init
* 描述	  :	MCP2515初始化配置
* 输入	  :	无
* 输出	  :	无
* 返回值  :	无
* 说明	  :	初始化包括：软件复位、工作波特率设置、标识符相关配置等。
*******************************************************************************/
void MCP2515_Init(void)
{
	unsigned char temp=0;

	MCP2515_Reset();										// 发送复位指令软件复位MCP2515
	Delay_Nms(1);											// 通过软件延时约nms(不准确)

	// TQ/Bit：5至25之间 一般16或以上 全部四个时间段的TQ总数必须介于5和25之间。良好的开端是选择每位16个TQ
	// 最大化SJW会降低对振荡器容差的要求
	// TPROP = 总延时(ns) PRSEG最小值 =	TPROP/TQ。
	// 举例使用40m长的总线：TPROP =	870	ns，PRSEG最小值	= TPROP/TQ = 6.96 TQ。因此PRSEG选择7才允许使用40m长的总线
	// 具体可参考官网MCP25625规格书第23页
	// 设置波特率为125Kbps
	MCP2515_WriteReg(CNF1,SJW_1TQ|CAN_500Kbps);				// set CNF1,SJW=3,同步段(SyncSeg)为1TQ,BRP=0,TQ=[2*(BRP+1)]/Fsoc=2/16M=0.125us
	MCP2515_WriteReg(CNF2,0x80|PHSEG1_3TQ|PRSEG_2TQ);		// set CNF2,SAM=0,在采样点对总线进行一次采样，PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
	MCP2515_WriteReg(CNF3,PHSEG2_2TQ);						// set CNF3,PHSEG2=(2+1)TQ=3TQ,同时当CANCTRL.CLKEN=1时设定CLKOUT引脚为时间输出使能位

	// 配置发送缓冲区0
	MCP2515_WriteReg(TXB0CTRL,0x03);						// 发送缓冲器0最高的报文发送优先级
	MCP2515_WriteReg(TXB0DLC,0x04);							// 将数据长度写入发送缓冲器0的发送长度寄存器 4个字节数据
	MCP2515_WriteReg(TXB0SIDH,c_TXB0SIDH);					// 发送缓冲器0标准标识符高位 本机ID	为了提高bit同步可靠性，所有bit尽量避免连续为1或连续为0
	MCP2515_WriteReg(TXB0SIDL,c_TXB0SIDL&(~EXIDE));			// 发送缓冲器0标准标识符低位 本机ID	bit3是扩展帧标志位 标准帧
//	MCP2515_WriteReg(TXB0SIDL,c_TXB0SIDL|(EXIDE));			// 发送缓冲器0标准标识符低位 本机ID	bit3是扩展帧标志位 扩展帧

	// 配置发送缓冲区1
	MCP2515_WriteReg(TXB1CTRL,0x03);						// 发送缓冲器1最高的报文发送优先级
	MCP2515_WriteReg(TXB1DLC,0x04);							// 将数据长度写入发送缓冲器1的发送长度寄存器 4个字节数据
	MCP2515_WriteReg(TXB1SIDH,c_TXB0SIDH);					// 发送缓冲器1标准标识符高位 本机ID	为了提高bit同步可靠性，所有bit尽量避免连续为1或连续为0
	MCP2515_WriteReg(TXB1SIDL,c_TXB0SIDL&(~EXIDE));			// 发送缓冲器1标准标识符低位 本机ID	bit3是扩展帧标志位 标准帧
//	MCP2515_WriteReg(TXB1SIDL,c_TXB0SIDL|(EXIDE));			// 发送缓冲器1标准标识符低位 本机ID	bit3是扩展帧标志位 扩展帧

	// 配置发送缓冲区2
	MCP2515_WriteReg(TXB2CTRL,0x02);						// 发送缓冲器2较低的报文发送优先级
	MCP2515_WriteReg(TXB2DLC,0x04);							// 将数据长度写入发送缓冲器1的发送长度寄存器 4个字节数据
	MCP2515_WriteReg(TXB2SIDH,c_TXB0SIDH);					// 发送缓冲器1标准标识符高位 本机ID	为了提高bit同步可靠性，所有bit尽量避免连续为1或连续为0
	MCP2515_WriteReg(TXB2SIDL,c_TXB0SIDL&(~EXIDE));			// 发送缓冲器1标准标识符低位 本机ID	bit3是扩展帧标志位 标准帧
//	MCP2515_WriteReg(TXB1SIDL,c_TXB0SIDL|(EXIDE));			// 发送缓冲器1标准标识符低位 本机ID	bit3是扩展帧标志位 扩展帧

	// 配置接收缓冲区0
	MCP2515_WriteReg(RXB0SIDH,0x00);						// 清空接收缓冲器0的标准标识符高位
	MCP2515_WriteReg(RXB0SIDL,0x00);						// 清空接收缓冲器0的标准标识符低位
	MCP2515_WriteReg(RXB0CTRL,0x04);						// 接收符合滤波器条件的所有带扩展标识符或标准标识符的有效报文。
// 扩展标识符滤波器寄存器RXFnEID8:RXFnEID0用于带有标准标识符的报文中的前两个数据字节。
//	MCP2515_WriteReg(RXB0DLC,DLC_4);						// 设置接收数据的长度为4个字节
	MCP2515_WriteReg(RXF0SIDH,0x00);						// 配置验收滤波寄存器n标准标识符高位 用于buffer0 没有使用
	MCP2515_WriteReg(RXF0SIDL,0x00);						// 配置验收滤波寄存器n标准标识符低位 用于buffer0 没有使用
	MCP2515_WriteReg(RXF0EID8,c_RXF1EID8);					// 配置验收滤波寄存器n扩展标识符高位 标准帧应用在数据字节0 仅用高4位，低4位没有用
	MCP2515_WriteReg(RXF0EID0,0x00);						// 配置验收滤波寄存器n扩展标识符低位 标准帧应用在数据字节1 没有使用

	MCP2515_WriteReg(RXF1SIDH,0x00);						// 配置验收滤波寄存器n标准标识符高位 用于buffer0 没有使用
	MCP2515_WriteReg(RXF1SIDL,0x00);						// 配置验收滤波寄存器n标准标识符低位 用于buffer0 没有使用
	MCP2515_WriteReg(RXF1EID8,c_RXF0EID8);					// 配置验收滤波寄存器n扩展标识符高位 标准帧应用在数据字节0 仅用高4位，低4位没有用
	MCP2515_WriteReg(RXF1EID0,0x00);						// 配置验收滤波寄存器n扩展标识符低位 标准帧应用在数据字节1 没有使用

	MCP2515_WriteReg(RXM0SIDH,0x00);						// 配置验收屏蔽寄存器n标准标识符高位 用于buffer0 没有使用
	MCP2515_WriteReg(RXM0SIDL,0x00);						// 配置验收屏蔽寄存器n标准标识符低位 用于buffer0 没有使用
	MCP2515_WriteReg(RXM0EID8,0xFF);						// 配置验收屏蔽寄存器n扩展标识符高位 仅用高4位，低4位没有用
	MCP2515_WriteReg(RXM0EID0,0x00);						// 配置验收屏蔽寄存器n扩展标识符低位 没有使用


	// 配置接收缓冲区1,新增通道2，3
	MCP2515_WriteReg(RXB1SIDH,0x00);						// 清空接收缓冲器1的标准标识符高位
	MCP2515_WriteReg(RXB1SIDL,0x00);						// 清空接收缓冲器1的标准标识符低位
	MCP2515_WriteReg(RXB1CTRL,0x00);						// 接收符合滤波器条件的所有带扩展标识符或标准标识符的有效报文。

	MCP2515_WriteReg(RXF2SIDH,0x00);						// 配置验收滤波寄存器n标准标识符高位 用于buffer1 没有使用
	MCP2515_WriteReg(RXF2SIDL,0x00);						// 配置验收滤波寄存器n标准标识符低位 用于buffer1 没有使用
	MCP2515_WriteReg(RXF2EID8,c_RXF2EID8);					// 配置验收滤波寄存器n扩展标识符高位 标准帧应用在数据字节0 仅用高4位，低4位没有用
	MCP2515_WriteReg(RXF2EID0,0x00);						// 配置验收滤波寄存器n扩展标识符低位 标准帧应用在数据字节1 没有使用

	MCP2515_WriteReg(RXF3SIDH,0x00);						// 配置验收滤波寄存器n标准标识符高位 用于buffer1 没有使用
	MCP2515_WriteReg(RXF3SIDL,0x00);						// 配置验收滤波寄存器n标准标识符低位 用于buffer1 没有使用
	MCP2515_WriteReg(RXF3EID8,c_RXF3EID8);					// 配置验收滤波寄存器n扩展标识符高位 标准帧应用在数据字节0 仅用高4位，低4位没有用
	MCP2515_WriteReg(RXF3EID0,0x00);						// 配置验收滤波寄存器n扩展标识符低位 标准帧应用在数据字节1 没有使用

	MCP2515_WriteReg(RXM1SIDH,0x00);						// 配置验收屏蔽寄存器n标准标识符高位 用于buffer1 没有使用
	MCP2515_WriteReg(RXM1SIDL,0x00);						// 配置验收屏蔽寄存器n标准标识符低位 用于buffer1 没有使用
	MCP2515_WriteReg(RXM1EID8,0xFF);						// 配置验收屏蔽寄存器n扩展标识符高位 仅用高4位，低4位没有用				   改
	MCP2515_WriteReg(RXM1EID0,0xF0);						// 配置验收屏蔽寄存器n扩展标识符低位 没有使用

	// 配置中断
	MCP2515_WriteReg(CANINTF,0x00);							// 清空CAN中断标志寄存器的所有位(必须由MCU清空)
	MCP2515_WriteReg(CANINTE,RX0IE|RX1IE);						// 配置CAN中断使能寄存器的接收缓冲器0满中断RX0IE使能,其它位禁止中断

	// 配置工作模式
	MCP2515_WriteReg(CANCTRL,REQOP_NORMAL);					// 将MCP2515设置为正常模式,退出配置模式
//	MCP2515_WriteReg(CANCTRL,REQOP_LOOPBACK);				// 将MCP2515设置为环回模式,退出配置模式

	// 确认工作模式
	temp=MCP2515_ReadReg(CANSTAT);							// 读取CAN状态寄存器的值
	if(OPMODE_NORMAL!=(temp&&0xE0))							// 判断MCP2515是否已经进入正常模式
	{
		MCP2515_WriteReg(CANCTRL,REQOP_NORMAL);				// 再次将MCP2515设置为正常模式,退出配置模式
//		MCP2515_WriteReg(CANCTRL,REQOP_LOOPBACK);			// 再次将MCP2515设置为环回模式,退出配置模式
	}
}

/*******************************************************************************
* 函数名  :	CAN_Send_Buffer0
* 描述	  :	CAN发送长度4的数据
* 输入	  :	*CAN_TX_Buf(待发送数据缓冲区指针))
* 输出	  :	无
* 返回值  :	无
* 说明	  :	仅发送4个字节的数据，前面初始化时已确认一致
*******************************************************************************/
void CAN_Send_Buffer0(unsigned char	*CAN_TX_Buf)
{
	CLI();
	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_LOAD_TXD0);									// 发送转载数据命令	TX缓冲器0,开始于TXB0D0
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	MCP2515_CS_High();										// 置MCP2515的CS为高电平

	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_RTS_TXB0);									// 直接请求发送报文0命令
	MCP2515_CS_High();										// 置MCP2515的CS为高电平
	SEI();
}

/*******************************************************************************
* 函数名  :	CAN_Send_Buffer1
* 描述	  :	CAN发送长度4的数据
* 输入	  :	*CAN_TX_Buf(待发送数据缓冲区指针))
* 输出	  :	无
* 返回值  :	无
* 说明	  :	仅发送4个字节的数据，前面初始化时已确认一致
*******************************************************************************/
void CAN_Send_Buffer1(unsigned char	*CAN_TX_Buf)
{
	CLI();
	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_LOAD_TXD1);									// 发送转载数据命令	TX缓冲器0,开始于TXB0D0
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	MCP2515_CS_High();										// 置MCP2515的CS为高电平

	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_RTS_TXB1);									// 直接请求发送报文0命令
	MCP2515_CS_High();										// 置MCP2515的CS为高电平
	SEI();
}

/*******************************************************************************
* 函数名  :	CAN_Send_Buffer2
* 描述	  :	CAN发送长度4的数据
* 输入	  :	*CAN_TX_Buf(待发送数据缓冲区指针))
* 输出	  :	无
* 返回值  :	无
* 说明	  :	仅发送4个字节的数据，前面初始化时已确认一致
*******************************************************************************/
void CAN_Send_Buffer2(unsigned char	*CAN_TX_Buf)
{
	CLI();
	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_LOAD_TXD2);									// 发送转载数据命令	TX缓冲器0,开始于TXB0D0
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	CAN_TX_Buf++;
	SPI_RW(*CAN_TX_Buf);									// 连续写入数据
	MCP2515_CS_High();										// 置MCP2515的CS为高电平

	MCP2515_CS_Low();										// 置MCP2515的CS为低电平
	SPI_RW(CAN_RTS_TXB2);									// 直接请求发送报文0命令
	MCP2515_CS_High();										// 置MCP2515的CS为高电平
	SEI();
}

/*******************************************************************************
* 函数名  :	CAN_Send_Buffer
* 描述	  :	CAN发送指定长度的数据
* 输入	  :	*CAN_TX_Buf(待发送数据缓冲区指针),len(待发送数据长度)
* 输出	  :	无
* 返回值  :	无
* 说明	  :	无
*******************************************************************************/
void CAN_Send_Buffer(unsigned char *CAN_TX_Buf,unsigned	char len)
{
	unsigned char j,dly,count;

	count=0;

	CLI();

	while(count<len)
	{
		dly=0;
		while((MCP2515_ReadReg(TXB0CTRL)&0x08) && (dly<50))	// 快速读某些状态指令,等待TXREQ标志清零
		{
			Delay_Nms(1);									// 通过软件延时约nms(不准确)
			dly++;
		}

		for(j=0;j<8;)
		{
			MCP2515_WriteReg(TXB0D0+j,CAN_TX_Buf[count++]);	// 将待发送的数据写入发送缓冲寄存器
			j++;
			if(count>=len) break;
		}
		MCP2515_WriteReg(TXB0DLC,j);						// 将本帧待发送的数据长度写入发送缓冲器0的发送长度寄存器
//		MCP2515_CS_Low();
		MCP2515_WriteReg(TXB0CTRL,0x08);					// 请求发送报文
//		MCP2515_CS_High();
	}

	SEI();
}

/*******************************************************************************
* 函数名  :	CAN_Receive_Buffer0
* 描述	  :	CAN接收一帧数据
* 输入	  :	*CAN_TX_Buf(待接收数据缓冲区指针)
* 输出	  :	无
* 返回值  :	无
* 说明	  :	仅接收4个字节的数据，前面初始化时已确认一致
*******************************************************************************/

void CAN_Receive_Buffer0(unsigned char *CAN_RX_Buf,unsigned char CAN_RD_RXDn)
{
	MCP2515_CS_Low();										// 置MCP2515的CS为低电平

	SPI_RW(CAN_RD_RXDn);									// 发送转载数据命令	RX缓冲器0,开始于RXB0D0
	*CAN_RX_Buf	= SPI_RW(0x55);								// 连续读入数据1
	CAN_RX_Buf++;
	*CAN_RX_Buf	= SPI_RW(0x55);								// 连续读入数据1
	CAN_RX_Buf++;
	*CAN_RX_Buf	= SPI_RW(0x55);								// 连续读入数据1
	CAN_RX_Buf++;
	*CAN_RX_Buf	= SPI_RW(0x55);								// 连续读入数据1


	MCP2515_CS_High();										// 置MCP2515的CS为高电平
															// 在拉升CS	引脚为高电平后，相关的RX 标志位（CANINTF.RXnIF）将被自动清零
}

/*******************************************************************************
* 函数名  :	CAN_Receive_Buffer
* 描述	  :	CAN接收一帧数据
* 输入	  :	*CAN_TX_Buf(待接收数据缓冲区指针)
* 输出	  :	无
* 返回值  :	len(接收到数据的长度,0~8字节)
* 说明	  :	无
*******************************************************************************/
unsigned char CAN_Receive_Buffer(unsigned char *CAN_RX_Buf)
{
	unsigned char i=0,len=0,temp=0;

	temp = MCP2515_ReadReg(CANINTF);

	if(temp	& RX0IE)										// 判断是接收缓冲器0 满中断RX0IF
	{
		len=MCP2515_ReadReg(RXB0DLC);						// 读取接收缓冲器0接收到的数据长度(0~8个字节)
		while(i<len)
		{
			CAN_RX_Buf[i]=MCP2515_ReadReg(RXB0D0+i);		// 把CAN接收到的数据放入指定缓冲区
			i++;
		}
	}

	MCP2515_WriteReg(CANINTF,0);							// 清除中断标志位(中断标志寄存器必须由MCU清零)

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
