#include "includes.h"
/************************************************************/
/* 变量、常量定义	*/										//
/************************************************************/
//u08 CAN_Version[4]	= {0xF1,0x07,0x07,0x31};
  u08 pu08CanCmd[4]		= {0};
  u08 tmp0,tmp1,tmp2,tmp3;
  u08 DoubleHandWave = 0;   //新增，专用于双手摆动
  //u08 g_bVersion_Call = false;
  u08 HoleTest=false;	//工厂hole值测试
/************************************************************/
/* 版本信息	*/												//
/************************************************************/
void CAN_SoftVersionSend(void)								 // 右手
{
    g_pu8CanSendBuffer[0] = 0xF1;
	g_pu8CanSendBuffer[1] = 0xA3;
	g_pu8CanSendBuffer[2] = 0x23;
	g_pu8CanSendBuffer[3] = 0x03;
	CAN_Send_Buffer2(g_pu8CanSendBuffer);
}

/************************************************************/
/* 接收处理	*/												//
/************************************************************/
void ReceiveToControlProcess(void)
{
    	if (CAN_ReadReceiveBuffer(pu08CanCmd, 4) == 4)
		{

			g_bLed_Flash	= true;											// 指示灯
			tmp0 = pu08CanCmd[0];
			tmp1 = pu08CanCmd[1];
			tmp2 = pu08CanCmd[2];
			tmp3 = pu08CanCmd[3];
            switch(tmp0)
			{
			    case 0x60:
				    Motor_HandToControl(tmp1,tmp2,tmp3);
					 break;
				case 0x61:
				     if (g_bLampEffectCanBusCmdDisable == false)
					 {
					  	 if(tmp1==0x00 )									// 判断是否关闭呼吸灯
						 {
						    Lamp_Off();
						 }
						 else if( tmp1==0x01 )								// 判断是否频闪灯
						 {
					        Lamp_FlashToControl(tmp2,tmp3);  				//
						 }
						 else if( tmp1==0x02 )			   					// 判断是否呼吸灯模式
						 {
						    Lamp_MonoOrColorfulControl(1,0);
						 }
						 else if(tmp1==0x03)
						 {
					        Lamp_MonoOrColorfulControl(0,tmp2);
						 }
					 }
					 break;
					 /*
				case 0x62:

							if(abs(tmp1-g_Head_State)!=8)
							{
								Motor_HandToControl(8,tmp2,tmp3);
							}
							else
							{
								Motor_HandToControl(tmp1,tmp2,tmp3);
							}

					 break;
*/
			    case 0xF0:
				     switch(tmp1)
					 {
					    case 0x01:
						     g_bReset_Motor          =true;
   						     g_blrHandWavingOn	     = false;    			//
							 g_bFactoryFunctionTest  = false;
							 g_bFactoryBoardTest     = false;
							 g_bHandOriTar_Operation = false;
							 Motor_HandToControl(7, 60, 0);
							 break;
					    case 0x02:
						     if(tmp2 ==0x01)
							 {
								//Delay_ms(50);  2
								CAN_SoftVersionSend();
								//g_bVersion_Call = true;
							 }
							 break;
						//case 3:

						//	 break;
					    case 0x05:
							 /*MCUCR |= BIT(SE);							// 进入休眠前最后一条指令 确保休眠是正常操作
							 Power_Down();									// 休眠处理
							 _SLEEP();										// 休眠，等待唤醒 按键或其他中断 寄存器配置不变
							 MCUCR &=~BIT(SE);								// 关闭休眠使能	防止非正常操作
							 Power_Up();	*/
							 g_bLed_Flash = true;
							 break;
						case 0x06:
							 if(tmp3==0x00)
							 {
							    Motor_MoveToStop();
							 }
							 else if(tmp3 == 0x01)
							 {
							    Motor_MoveToStop();
								g_bCharge_MotorStop = true;
							 }
							 else if(tmp3==0x02)
							 {
							    g_bCharge_MotorStop = false;
							 }
							 break;
						case 0x08:
						     if(tmp3 == 0x0A)
							 {
							    if(tmp2 == 0x01)
								{
								   g_bTouchLedControl = true;
								}
								else if(tmp2 == 0x00)
								{

								   g_bTouchLedControl = false;
								}
							 }
							 break;
					     case 0x11:
						 	  if(tmp2 == 0xAC)
							  {
							       if(tmp3 == 0x01)
								   {
								      // Factory_BoardTest_On();
									   g_bFactoryBoardTest = true;
									   g_bFactoryFunctionTest = false;
									   Lamp_MonoOrColorfulControl(0, 6);
									   Motor_HandOriTarToControl(0x08,40,2);
									   CAN_SoftVersionSend();

								   }
								   else if(tmp3 == 0x02)
								   {
								       //Factory_BoardTest_Off();
									   Motor_MoveToStop();
								       g_bFactoryBoardTest = false;
									   Lamp_Init();
									   Motor_HandToControl(7, 60, 0);
								   }
								   else if(tmp3 == 0x03)
								   {
								       g_bFactoryFunctionTest = true;
									   g_bFactoryBoardTest = false;
									   Motor_HandOriTarToControl(0x08,70,3);
									   Lamp_MonoOrColorfulControl(0, 6);
									   //Factory_FunctionTest_On();
							       }
								   else if(tmp3 == 0x04)
								   {
								       Motor_MoveToStop();
									   g_bFactoryFunctionTest = false;
									   Lamp_Init();
									   Motor_HandToControl(7, 50, 0);
									   //Factory_FunctionTest_Off();
								   }
								    else if(tmp3 == 0x05)
								   {
										HoleTest = true;
								   }
							   }
							   break;
						  case 0xB6:
						       if(tmp3 == 0x01)
							   {
							       g_bTouchKeyEnable =  true;
							   }
							   else if(tmp3 == 0x00)
							   {
							       g_bTouchKeyEnable =  false;
							   }
							   break;
						  /*case 0x56:
						       if (tmp3 == 0x00)
							   {
							       RobotDoubleHandWavingOff(true);
							   }
							   else if (tmp3 == 0x02)
							   {
							       RobotDoubleHandWavingOn(tmp2);
							   }
							   else
							   {
							       RobotDoubleHandWavingOff(false);
							   }
							   break;*/
							   default:
							   break;
					 }
					 case 0xF1:
					      if (tmp1 == 0x06)
						  {
						      switch(tmp2)
							  {
							   // Adpter disconnect
						   		  case 0xF0:
								  g_bLampEffectCanBusCmdDisable = false;

								  break;

							   // Adpter connect
								  case 0xF1:
								  g_bLampEffectCanBusCmdDisable = true;
								  break;

							   // Charging: Red
						   		  case 0xF2:
							   // 颜色；0红、1黄、2绿、3青、4蓝、5紫、6白
							   	  #if 0
							   	  Lamp_On(0);
								  g_Lamp_Colour 	= 0;
								  b_Lamp_PwmAdd 	= true;
								  g_Lamp_PWM 		= 0;
								  b_Lamp_LiuShui 	= false;
								  b_Lamp_Flash 	= false;
								  b_Lamp_Charge 	= true;
								  #endif
								  Lamp_MonoOrColorfulControl(0, 0);	 //
								  break;

							   // Charge full: Green
							   	  case 0xF3:
							   // 颜色；0红、1黄、2绿、3青、4蓝、5紫、6白
							      #if 0
							   	  Lamp_On(2);
								  g_Lamp_Colour 	= 2;
								  b_Lamp_PwmAdd 	= true;
								  g_Lamp_PWM 		= 0;
								  b_Lamp_LiuShui 	= false;
								  b_Lamp_Flash 	= false;
								  b_Lamp_Charge 	= true;
								  #endif
								  Lamp_MonoOrColorfulControl(0, 2);		 //
								  break;

							   // Fault: Yellow
							   	  case 0xF4:
							   // 颜色；0红、1黄、2绿、3青、4蓝、5紫、6白
							      #if 0
							   	  Lamp_On(1);
								  g_Lamp_Colour 		= 1;
								  g_Lamp_Flash_Time 	= 50;
								  g_Lamp_Flash 		= 50;
								  b_Lamp_Flash_On 	= true;
								  b_Lamp_Flash 		= true;
								  b_Lamp_LiuShui 		= false;
								  b_Lamp_Charge 		= false;
								  #endif
								  Lamp_FlashToControl(1,50);

								  break;
							  }
						  }
						  /*else if (tmp1 == 0x60)
						  {
						  	  g_brHandWaveFinished = true;
						  }*/
					 	  break;
					 default:
					 break;

			 }


        }

}