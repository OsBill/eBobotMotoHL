/********************************************************************************************
* �ļ���: main.c																			*
* ������: ������  ����ʱ�� : 2016/4/23 1:46:07												*
* �޸���: linzk   �޸�ʱ�� : 2017��7��14��10:35:09											*
* ����	: eBobotMotoHR ��ѿ���������ֱ۲���													*
* ��Ȩ	: ��ѿ																		*
********************************************************************************************/
#include "includes.h"

/************************************************************/
/* ���ݻ�����												*/
//	u08	CAN_Version[4]	= {	0xF1,0x07,0x08,0x31	};			// �汾������0xAA���豸���	2������5���ȡ�7���֡�8���� 0xBB���汾�� 10��ʾV1.0  21��ʾV2.1 "
/************************************************************/
/* �ַ�������												*/
//	c08	s_Version[]	= "eBobotMotoHL V3.10";					// �汾ʶ��
//	c08	s_Verdata[]	= "2018��4��13��22:19:58";					// ����ʱ��
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

//��֪����ɶ�ã����Բ���ʹ�ܻᱻ�أ��ƺ���׿�����һ�ε�Ч����Ҫ����
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
	/*��ֹjtag�ӿڣ�ADC1��LEDB�ſ�ʹ�ã�������¼ʱ��˿λ����ȡ��jtagʹ��*/
	MCUCSR |= 1 << JTD; //��ֹJTAG�ӿ�
	MCUCSR |= 1 << JTD; //��Ҫ�������β������ܱ���Чִ��
//	MCUCSR &= ~(1 << JTD); //����JTAG�ӿ�
//	MCUCSR &= ~(1 << JTD); //��Ҫ�������β������ܱ���Чִ��


	//Delay_ms(500);

    SystermInit();
	Init_Board();
	MCP2515_Init();											// ��ʼ��MCP2515
	WDT_Enable(WDT_PERIOD_2_1s);




	while(1)
	{


	 	ReceiveToControlProcess();   //���մ���can����

	//	Lamp_Enable();


		 // Current_CompareToBrake();
		Motor_HandControlProcess();
		pollint();


		//Led_Test_On();
/*__________________________________________________________*/








/*__________________________________________________________*/
#if 0
//��ʱ5�룬Ӧ����������5���ڽ�ֹ�����ƹ�Ĺ��ܣ�Ȼ��û��
		/* ����														*/
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
