#include "includes.h"


/*
typedef enum
{
    normal,			//正常状态
	lowPower,			//低电量状态
	ultraLowPower,		//超低电量状态
	charging,			//充电中
	chargingComple,		//充饱
	sendCmd,			//发送2.4g指令
	pairing,			//2.4g配对
	pairingSuccess		//配对成功
}LEDWORKSTAUS;
*/
Uchar batteryState = 0;



/****************************************************************************************************
**	函	数: 进入模式
**	参	数: 无
**	返	回: 无
*****************************************************************************************************/

extern void EnterMode(uint8_t mode)
{
	currentMode = mode;
}

extern void checkCharge(void)
{
	if(USB_IN && (batteryState!=fullCharge))
	{
		ENABLE_TIMER1_INT();		//开定时器1中断
	}
	else
	{
		DISABLE_TIMER1_INT();
		ExitRedBreathLed();
		setLedTime(green,0,1,255);		//绿灯常亮
		setLedTime(red,1,0,255);		//红灯灭
	}
}


extern void setLedState(void)
{
	//LED灯初始化配置
	switch (batteryState)
	{
		case lowPower:
			setLedTime(red,0,2,6);			//红灯常亮
			break;
		case powerOnCharging:
			break;
		case fullCharge:
			break;
		case normal:
		default:
			setLedTime(blue,0,1,255);		//蓝灯亮
			setLedTime(green,1,0,255);		//红绿灭
			setLedTime(red,1,0,255);
			break;
	}
	checkCharge();


}


extern void checkBatState(void)
{
	uint16 batVoltage;
	batVoltage = get_adcVal(SingleMode,BAT_PIN);
	if (USB_IN)		//usb插入，充电中
	{
		if(batVoltage > 680)			//关机充电，bat电压>4.4,不工作
		{
			batteryState = powerOffCharging;
			EnterMode(_POWEROFFMODE_);
		}
		else
		{
			batteryState = powerOnCharging;
			EnterMode(_REGMODE_);
		}
		//if(CHG_STAT)batteryState = fullCharge;	//充饱
	}
	else
	{
		if(batVoltage > 527)				//正常电压，>3.4v
		{
			batteryState = normal;
		}
		else if(batVoltage <= 259)			//超低电压，<=3.35v
		{
			batteryState = ultraLowPower;
			EnterMode(_POWEROFFMODE_);
		}
		else								//低电压，3.35-3.4v
		{
			batteryState = lowPower;
		}
	}

}

/*
typedef struct
{
	Uchar  currentState;
	Uchar  lastState;
    Uchar  count;
	Uchar  voltage;
}ADC_STRUCT;

ADC_STRUCT *YGKEY0;
ADC_STRUCT *BAT;
*/


void keyProcess(void)
{
	Uchar YGKeyState,keyLState,keyRState;
	static Uchar count = 0,contiSendCount = 0;
	static Uchar lastState = 0;
	Uchar (*P_KEY)[4] = RF_Buf;
	YGKeyState = checkYGKey();
	keyLState = getKeyValue(KEY_L);
	keyRState = getKeyValue(KEY_R);
	lastState = YGKeyState;
//	if(YGKeyState != 0 && keyLState != 0 && keyRState != 0)return;
	if(YGKeyState <= 4 && YGKeyState > 0)					//破坏结构，待改20190903
	{
		if(++contiSendCount >= 5)
		{
			memcpy(FIFO_data,P_KEY[YGKeyState],4);
			RF_SendData(FIFO_data,8);
		}
	}
	else
	{
		contiSendCount = 0;
	}
	if(lastState != 0 && YGKeyState == 0 && lastState != 5)		//旧代码5没有回正功能
	{
		memcpy(FIFO_data,P_KEY[YGKeyState],4);
		RF_SendData(FIFO_data,8);
	}
/**************破坏结构，待改20190903*****************/
	if(YGKeyState == 5)
	{
		if(++count >= 100)
		{
			memcpy(FIFO_data,P_KEY[10],4);
			RF_SendData(FIFO_data,8);
			count = 0;
		}
	}
	else
	{
		if(lastState == 5 && count > 1)
		{
			memcpy(FIFO_data,P_KEY[9],4);
			RF_SendData(FIFO_data,8);
		}
		count = 0;
	}
/*******************************/
	if(keyLState == KEY_SHORT)
	{
		memcpy(FIFO_data,P_KEY[5],4);
		RF_SendData(FIFO_data,8);
	}
	else if(keyLState == KEY_LONG)
	{
		memcpy(FIFO_data,P_KEY[6],4);
		RF_SendData(FIFO_data,8);
	}
	if(keyRState == KEY_SHORT)
	{
		memcpy(FIFO_data,P_KEY[7],4);
		RF_SendData(FIFO_data,8);
	}
	else if(keyRState == KEY_LONG)
	{
		memcpy(FIFO_data,P_KEY[8],4);
		RF_SendData(FIFO_data,8);
	}
	if(keyLState == KEY_LONG && keyRState == KEY_LONG)
	{
		//配对
		EnterMode(_PAIRINGMODE_);
	}



}

extern void testMode(void)
{


		pollint();


}

extern void pairingMode(void)
{
	setLedTime(blue,2,2,255);		
	setLedTime(green,2,2,255);
	setLedTime(red,2,0,6);
	DISABLE_TIMER1_INT();
	ExitRedBreathLed();
	while(1)
	{
		RF_RX_Check();
	}
	
		

}


extern void powerOffMode(void)
{
	if(batteryState == powerOffCharging)		//关机充电
	{
		setLedTime(blue,1,0,255);		//蓝绿灭
		setLedTime(green,1,0,255);
		ENABLE_TIMER1_INT();
	}
	else if(batteryState == ultraLowPower)				//超低电量
	{
		setLedTime(blue,2,2,6); 		//闪烁3次
		setLedTime(red,2,2,6);
	}

	//低功耗关闭电源

	while(1)
	{
		delay_ms(1000);
		updateLedStatus();
		checkBatState();
		checkCharge();
		if(currentMode != _POWEROFFMODE_)
		{
			//开电源 外设
			return;
		}
	}
}



