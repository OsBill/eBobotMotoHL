/********************************************************************************************
* 文件名: main.c																			*
* 创建人: 邱作霖  创建时间 : 2016/4/23 1:46:07												*
* 修改人: linzk   修改时间 : 2017年7月14日10:35:09											*
* 功能	: eBobotMotoHR 贝芽机器人右手臂操作													*
* 版权	: 贝芽																		*
********************************************************************************************/
#include "includes.h"

/************************************************************/
/* 数据缓冲区												*/
//	u08	CAN_Version[4]	= {	0xF1,0x07,0x08,0x31	};			// 版本参数：0xAA：设备编号	2颈部、5右腿、7左手、8右手 0xBB：版本号 10表示V1.0  21表示V2.1 "
/************************************************************/
/* 字符串常数												*/
//	c08	s_Version[]	= "eBobotMotoHL V3.10";					// 版本识别
//	c08	s_Verdata[]	= "2018年4月13日22:19:58";					// 编译时间
/************************************************************/





void SystermInit(void)
{
    g_bLampEffectCanBusCmdDisable	 = true;
	g_bTouchKeyEnable       		 = true;
	g_bTouchLedControl     			 = true;
	g_bLampFlashToggle	    		 = true;
	g_bLamp_Flash					 = false;
	g_bLampTurnUpEnable	   			 = false;
	g_bLampTurnDownEnable			 = false;
	g_bLamp_Monochrome	    		 = false;
	g_bLamp_ColorfulBreath  		 = false;
	g_bLed_Flash					 = false;
	g_bReset_Motor       			 = false;
	g_bHead_Operation    			 = false;
	g_bCharge_MotorStop  			 = false;
    g_bHandWaving		 			 = false;
	g_bMoto_Err		 	 			 = false;
	g_blrHandWavingOn			     = false;
    g_brHandWaveFinished			 = true;
	g_bHandOriTar_Operation 		 = false;
    g_bFactoryBoardTest 			 = false;
	g_bFactoryFunctionTest 			 = false;

}

//不知道做啥用，测试不加使能会被关，似乎安卓几秒关一次灯效，需要开启
extern void Lamp_Enable(void)
{
	if((g_bLampEffectCanBusCmdDisable == true) && (sys1msClock>=5000))
	{
			 g_bLampEffectCanBusCmdDisable = false;
			 sys1msClock = 0;
	}
}




void main( void	)
{
	/*禁止jtag接口，ADC1和LEDB才可使用，或在烧录时熔丝位设置取消jtag使能*/
	MCUCSR |= 1 << JTD; //禁止JTAG接口
	MCUCSR |= 1 << JTD; //需要连续两次操作才能被有效执行
//	MCUCSR &= ~(1 << JTD); //允许JTAG接口
//	MCUCSR &= ~(1 << JTD); //需要连续两次操作才能被有效执行


	//Delay_ms(500);

    SystermInit();
	Init_Board();
	MCP2515_Init();											// 初始化MCP2515
	WDT_Enable(WDT_PERIOD_2_1s);




	while(1)
	{


	 	ReceiveToControlProcess();   //接收处理can数据

	//	Lamp_Enable();


		 // Current_CompareToBrake();
		Motor_HandControlProcess();
		pollint();


		//Led_Test_On();
/*__________________________________________________________*/








/*__________________________________________________________*/
#if 0
//定时5秒，应该是做开机5秒内禁止其他灯光的功能，然而没做
		/* 变量														*/
		u08 g_LampEffectCanBusCmdTime		= 0;

		if(g_bTimer_50mS == true)
		{
		     g_bTimer_50mS = false;

			 if(g_bLampEffectCanBusCmdDisable == true)
		     {
		         g_LampEffectCanBusCmdTime++;
			     if(g_LampEffectCanBusCmdTime == 100)
			     {
			        g_bLampEffectCanBusCmdDisable = false;
			        g_LampEffectCanBusCmdTime =0;
			     }

		     }

		     /*
 		     if(g_bVersion_Call == true)
		     {
		         CAN_SoftVersionSend();
		         g_bVersion_Call = false;
		     }
			*/

		}
#endif

	}
}
