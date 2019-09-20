#include "includes.h"

/************************************************************/
/* 全局变量、常量定义	*/									//
//u08 CAN_TouchBuf[4]     = {0xF1,0x03,0x01,0x0A}			// Touch_Key  发送信息
//u08 CAN_BellBuf [4]	  = {0xF0,0x12,0x08,0x00}			// Buzz       发送信息
  u08 g_bTouchKeyEnable       = true;
  u08 g_bTouchLedControl      = true;
  u08 g_bLampFlashToggle	  = true;
  u08 g_bLamp_Flash			  = false;
  u08 g_bLampTurnUpEnable	  = false;
  u08 g_bLampTurnDownEnable	  = false;
  u08 g_bLamp_Monochrome	  = false;
  u08 g_bLamp_ColorfulBreath  = false;
  
  u08 g_bLed_Flash			  = false;
  u08 g_bLampEffectCanBusCmdDisable	 = true;
/* 变量、常量定义	*/		
  u08 b_TouchKey_Ok	       	  = true;
  u08 b_Lamp_Flash_On	   	  = 0;
  u08 b_Lamp_PwmAdd		   	  = false;
  u08 g_Lamp_Flash		   	  = 0;
  u08 g_Lamp_Flash_Time    	  = 0;
  u08 g_Lamp_Colour		   	  = 0;
  u08 g_Pwm_Add_Delay	   	  = 0;
  u08 g_Pwm_Add_Delay1	   	  = 0;
  u08 g_u8LampFlashTime       = 0;
  u08 g_u8LampFlashTimeCnt 	  = 0;
  u08 g_u8LampFlashLoopTimes  = 0;
  u08 g_u8LampTurnUpStepCnt	  = 0;
  u08 g_u8LampTurnUpStep	  = 1;
  u08 g_u8LampTurnDownStepCnt = 0;
  u08 g_u8LampTurnDownStep	  = 1;
// 呼吸灯
#define	g_Lamp_PWM				OCR2						// 呼吸灯PWM
/************************************************************/
/* Touch发送信息*/											 //
void CAN_TouchKeyPressSend(u08 u8PressOrUnpress)
{
    g_pu8CanSendBuffer[0] = 0xF1;
    g_pu8CanSendBuffer[1] = 0x03;
    if(u8PressOrUnpress == 0x01)							 // 触摸
    {
       g_pu8CanSendBuffer[2] = 0x01;	
    }
    else if(u8PressOrUnpress == 0x02)						 // 松开了触摸
    {
       g_pu8CanSendBuffer[2] = 0x02;	
    }					
    g_pu8CanSendBuffer[3] = 0x0A;							 // 左手部位编码
    CAN_Send_Buffer0(g_pu8CanSendBuffer);
}

/************************************************************/
/* Buzz控制发送信息*/		
void CAN_BuzzControlSend(u08 u8BeepNumber)
{ 
    g_pu8CanSendBuffer[0] = 0xF0;
    g_pu8CanSendBuffer[1] = 0x12;
    g_pu8CanSendBuffer[2] = 0x08;
    g_pu8CanSendBuffer[3] = u8BeepNumber;
    CAN_Send_Buffer2(g_pu8CanSendBuffer);
}
/************************************************************/
/* 指示灯控制进程*/	
void LedFlashProcess(void)
{
    if(g_bLed_Flash == true)                              		 // LED闪烁控制
    {
	    Led_Test_On();                                  	 // 点亮LED
	    g_bLed_Flash= false;
	}
	else
	{
		Led_Test_Off();										 // 熄灭LED
	}
}
/************************************************************/
/* 触摸控制进程*/	
void TouchKeyProcess(void)
{
	if(g_bTouchKeyEnable == true)							// 使能触摸按键控制功能
	{
		if(Read_Touch_Button == low)						// 低电平有效
		{
		     
			 if(b_TouchKey_Ok == true)
			 {
			    b_TouchKey_Ok = false;
			    CAN_TouchKeyPressSend(0x01);				// 发送触摸时Press信息
				
				if(g_bTouchLedControl == true)				// 触摸指示灯控制开
				{
				   Led_Touch_On();
				}
				else
				{
				   Led_Touch_Off();
				}
				CAN_BuzzControlSend(0x01);
			 }
			
		}
		else
		{
			 if(b_TouchKey_Ok == false)
		     {
				 Led_Touch_Off();
				 CAN_TouchKeyPressSend(0x02);
				 b_TouchKey_Ok = true;
			 }
		}
	}
	
}
/************************************************************/
// 说明：呼吸灯打开某个颜色操作,红黄绿青蓝紫白
// 输入：操作编号
void Lamp_On( u08 u8Lamp_Num )								//
{


	 switch(u8Lamp_Num)       
    {
		case 0:
			Led_Red_On();
			Led_Green_Off();
			Led_Blue_Off();
			break;
        case 1:
			Led_Red_On();
			Led_Green_On();
			Led_Blue_Off();
            break;
        case 2:
			Led_Red_Off();
			Led_Green_On();
			Led_Blue_Off();
            break;
        case 3:
			Led_Red_Off();
			Led_Green_On();
			Led_Blue_On();	
            break;
        case 4:
			Led_Red_Off();
			Led_Green_Off();
			Led_Blue_On();
            break;
        case 5:
			Led_Red_On();
			Led_Green_Off();
			Led_Blue_On();
            break;
        case 6:
			LED_ALL_ON();
            break;
	}	
}

/************************************************************/
// 说明：呼吸灯参数初始化
//
void Lamp_Init(	void )										//
{
	LED_ALL_OFF();

	
	g_Lamp_Flash 		   = 0;
	g_Lamp_Flash_Time	   = 0;
	b_Lamp_Flash_On		   = false;
	g_bLamp_Flash		   = false;
	
	g_Lamp_Colour 		   = 0;									
	g_Pwm_Add_Delay		   = 0;							
	g_Pwm_Add_Delay1 	   = 5;								
	g_Lamp_PWM 			   = 0;									
	b_Lamp_PwmAdd		   = true;					
	g_bLamp_ColorfulBreath  = false;
	g_bLamp_Monochrome	   = false;
}
/************************************************************/
/* 闪烁灯控制*/	
void Lamp_FlashToControl(u08 u8Color,u16 u16FlashTime)		 //
{
    //u16FlashTime = u16FlashTime / 10;

	Lamp_On(u8Color);

	g_bLampTurnUpEnable			= false;
	g_bLampTurnDownEnable		= false;
	g_bLamp_Monochrome			= false;
	g_bLamp_ColorfulBreath		= false;

	if (g_bLamp_Flash == false)
	{
		g_bLamp_Flash			= true;
		b_Lamp_Flash_On		    = true;
		g_Lamp_Flash			= (u08)u16FlashTime;
	}

	g_Lamp_Flash_Time			= (u08)u16FlashTime;
	
}

void Lamp_FlashProcess(void)
{
    if(b_Lamp_Flash_On)										 // 频闪亮的部分处理
	{
		g_Lamp_PWM = 255;									 //
		g_Lamp_Flash--;										 // 频闪周期计数器减一
		if(	g_Lamp_Flash ==	0 )								 // 频闪周期计数器是否溢出
		{
			g_Lamp_Flash = g_Lamp_Flash_Time;				 // 频闪计数器初值＝频闪周期
			if(	g_Lamp_Flash_Time != 0 )					 // 判断是否0闪烁，即常亮，如果是0即不灭
			b_Lamp_Flash_On = false;						 // 亮部分结束，转入灭部分
		}
	}
	else													 // 频闪灭的部分
	{
		g_Lamp_PWM = 0;										 // 灯灭
		g_Lamp_Flash--;										 // 频闪周期计数器减一
		if(	g_Lamp_Flash ==	0 )								 // 频闪周期计数器是否溢出
		{
			g_Lamp_Flash = g_Lamp_Flash_Time;				 // 频闪时间计数器初值＝频闪周期
			b_Lamp_Flash_On = true;							 // 灭部分结束转入亮的部分
		}
	}
	
}

void Lamp_FlashLoopToControl(u08 u8Color, u08 u8FlashTime, u08 u8LoopTimes)
{
	if (u8FlashTime == 0)
	{
		u8FlashTime = 200;
	}

	if (u8LoopTimes == 0)
	{
		u8LoopTimes = 1;
	}

	//u8FlashTime = u8FlashTime;

	Lamp_On(u8Color);

	g_u8LampFlashTime			= u8FlashTime;
	g_u8LampFlashLoopTimes		= u8LoopTimes;
	g_u8LampFlashTimeCnt		= 0;
	g_bLampFlashToggle			= true;
	
}

void Lamp_FlashLoopProcess(void)
{
    if (g_bLampFlashToggle == false)									// 频闪亮的部分处理
	{
		g_Lamp_PWM = 0;												//

		g_u8LampFlashTimeCnt++;									// 频闪周期计数器减一

		if(g_u8LampFlashTimeCnt == g_u8LampFlashTime)				// 频闪周期计数器是否溢出
		{
			g_u8LampFlashTimeCnt	= 0;
			g_bLampFlashToggle		= true;
		}
	}
	else												// 频闪灭的部分
	{
		g_Lamp_PWM = 255;

		g_u8LampFlashTimeCnt++;

		if (g_u8LampFlashTimeCnt == g_u8LampFlashTime)
		{
			g_u8LampFlashTimeCnt = 0;

			if (g_u8LampFlashLoopTimes != 0)
			{
				g_u8LampFlashLoopTimes--;
			}

			if (g_u8LampFlashLoopTimes == 0)
			{
				g_Lamp_PWM					= 0;
				g_bLampFlashToggle			= true;
			}
			else
			{
				g_bLampFlashToggle			= false;
			}
				
		}
	}
	
}

/************************************************************/
/* 单色或者七彩呼吸灯控制*/
void Lamp_MonoOrColorfulControl(u08 u8Mode, u08 u8Color)		 //
{
	if (u8Mode == 0)
	{
		Lamp_On(u8Color);							    
		g_Lamp_Colour 		= u8Color;			
		b_Lamp_PwmAdd 		= true;	
		g_Lamp_PWM 			= 0;
		g_bLamp_ColorfulBreath = false;
		g_bLampTurnDownEnable  = false;
		g_bLampTurnUpEnable	= false;	
		g_bLamp_Flash 		= false;
		g_bLamp_Monochrome 	= true;
	}
	else
	{
		Lamp_On(u8Color);	
		g_Lamp_Colour 		= u8Color;
		g_Pwm_Add_Delay		= 0;	
		g_Pwm_Add_Delay1 	= 5;	
		g_Lamp_PWM 			= 0;                       
		b_Lamp_PwmAdd 		= true;	
		g_bLamp_ColorfulBreath 	= true;	
		g_bLampTurnDownEnable   = false;
		g_bLampTurnUpEnable	= false;		
		g_bLamp_Flash		= false;	
		g_bLamp_Monochrome 	= false;
	}
}

void Lamp_MonochromeProcess(void)
{
    if(b_Lamp_PwmAdd)								   		 // 吸：占空比加
	{
		g_Lamp_PWM += 1;							   		 // 呼吸亮度控制
		if(	g_Lamp_PWM == 160 )						   		 // 调整节点
		{
			b_Lamp_PwmAdd =	false;					   		 // 转减操作
		}
	}
    else                                               		 // 呼：占空比减
	{
		g_Lamp_PWM -= 1;							   		 // 呼吸亮度控制
		if(	g_Lamp_PWM == 0 )						   		 // 调整节点
		{
			b_Lamp_PwmAdd =	true;					   		 // 转加操作
		}
	}
	
}
/************************************************************/
/* 七彩呼吸灯控制*/
void Lamp_ColorfulBreathProcess(void)
{
    if(b_Lamp_PwmAdd)										  // 吸：占空比加
	{
		g_Pwm_Add_Delay++;									  // 呼吸速度控制	矫正LED占空比的亮度非线性(可计算，也可查表)
		if(	g_Pwm_Add_Delay	== g_Pwm_Add_Delay1	)			  // g_Pwm_Add_Delay1:呼吸灯变化加速度控制
		{
			g_Pwm_Add_Delay	= 0;							  //
			g_Lamp_PWM += 1;								  // 呼吸亮度控制

			if(	g_Lamp_PWM == 15 )							  // 非线性调整节点
			{
				g_Pwm_Add_Delay1 = 4;						  // 加速度调整
			}
			else if( g_Lamp_PWM	== 20 )						  //
			{
				g_Pwm_Add_Delay1 = 3;						  //
			}
			else if( g_Lamp_PWM	== 25 )						  //
			{
				g_Pwm_Add_Delay1 = 2;						  //
			}
			else if( g_Lamp_PWM	== 35 )						  //
			{
				g_Pwm_Add_Delay1 = 1;						  //
			}
			else if( g_Lamp_PWM	== 135 )					  // 不用到255，135的亮度已差不多最亮，降低占空比LED寿命可延长
			{
				g_Lamp_PWM	= 138;                     		  // 减亮度的时候稍微提高一些亮度往下减，增加亮度
				b_Lamp_PwmAdd =	false;						  // 转减操作
				g_Pwm_Add_Delay1 = 1;						  // 呼操作
			}
		}
	}
	else													  // 呼：占空比减
	{
		g_Pwm_Add_Delay++;									  // 呼吸速度控制	矫正LED占空比的亮度非线性(可计算，也可查表)
		if(	g_Pwm_Add_Delay	== g_Pwm_Add_Delay1	)			  // g_Pwm_Add_Delay1:呼吸灯变化加速度控制
		{
			g_Pwm_Add_Delay	= 0;							  //
			g_Lamp_PWM -= 1;								  // 呼吸亮度控制

			if(	g_Lamp_PWM == 25 )							  // 非线性调整节点
			{
				g_Pwm_Add_Delay1 = 2;						  // 加速度调整
			}
			else if( g_Lamp_PWM	== 15 )						  //
			{
				g_Pwm_Add_Delay1 = 3;						  //
			}
			else if( g_Lamp_PWM	== 5 )						  // 不要减为0，5已经是很暗了，减少暗的过渡时间。
			{
				g_Lamp_Colour++;							  // 颜色；0红、1黄、2绿、3青、4蓝、5紫、6白
				if(	g_Lamp_Colour == 7 )
				g_Lamp_Colour =	0;
				Lamp_On( g_Lamp_Colour );					  // 设置颜色
				b_Lamp_PwmAdd =	true;						  // 转减操作
				g_Pwm_Add_Delay1 = 5;						  // 重复
			}
		 }
	}

}
/************************************************************/
/* 灯开启渐亮控制*/
void Lamp_TurnUp(u08 u8Color)
{
	Lamp_On(u8Color);

	g_Lamp_PWM				= 0;

	g_u8LampTurnUpStep	= 1;
	g_u8LampTurnUpStepCnt	= 0;
	g_bLampTurnUpEnable	= true;
}

void Lamp_TurnUpGradualProcess(void)
{
    g_u8LampTurnUpStepCnt++;
		
	if (g_u8LampTurnUpStepCnt == g_u8LampTurnUpStep)
	{
		g_u8LampTurnUpStepCnt = 0;

		g_Lamp_PWM += 1;

		if(g_Lamp_PWM == 15)
		{
			g_u8LampTurnUpStep = 4;
		}
		else if(g_Lamp_PWM == 20)
		{
			g_u8LampTurnUpStep = 3;
		}
		else if(g_Lamp_PWM == 25)
		{
			g_u8LampTurnUpStep = 2;
		}
		else if(g_Lamp_PWM == 35)	
		{
			g_u8LampTurnUpStep = 1;	
		}
		else if(g_Lamp_PWM == 135)
		{
			g_u8LampTurnUpStep	= 1;
			g_bLampTurnUpEnable	= false;
		}
	 }

}

/************************************************************/
/* 灯关闭渐灭控制*/
void LampTurnDown(void)
{
	g_u8LampTurnDownStep		= 1;
	g_u8LampTurnDownStepCnt	= 0;
	g_bLampTurnDownEnable		= true;
}

void Lamp_TurnDownGradualProcess(void)
{
    g_u8LampTurnDownStepCnt++;

	if (g_u8LampTurnDownStepCnt == g_u8LampTurnDownStep)
	{
		g_u8LampTurnDownStepCnt = 0;

		g_Lamp_PWM -= 1;

		if (g_Lamp_PWM == 30)
		{
			g_u8LampTurnUpStep = 2;	
		}
		else if (g_Lamp_PWM == 25)
		{
			g_u8LampTurnUpStep = 3;
		}
		else if (g_Lamp_PWM == 20)
		{
			g_u8LampTurnUpStep = 4;
		}
		else if (g_Lamp_PWM == 15)
		{
			g_u8LampTurnUpStep = 5;
		}
		else if (g_Lamp_PWM == 5)
		{
			g_u8LampTurnUpStep = 1;
		}

		if (g_Lamp_PWM == 0)
		{
			Led_Red_Off();
			Led_Green_Off();
			Led_Blue_Off();
			g_bLampTurnDownEnable = false;
		}
	}
}
/************************************************************/
/* 灯关闭控制*/
void Lamp_Off(void)
{
	g_bLamp_Flash          = false;							 // 关闭频闪灯模式
	g_bLamp_ColorfulBreath = false;							 // 关闭流水灯模式
	g_bLamp_Monochrome 	   = false;							 // 关闭单色呼吸灯模式
	g_bLampTurnUpEnable    = false;							 // 关闭开灯渐亮模式
	g_bLampTurnDownEnable  =false;							 // 关闭关灯渐灭模式
	Led_Red_Off();
	Led_Green_Off();
	Led_Blue_Off();
}

extern void LAMP_Task(void)
{
	if(g_bLamp_ColorfulBreath == true)
	{
		Lamp_ColorfulBreathProcess();	//七彩呼吸灯
	}
 	else if(g_bLamp_Flash == true)
	{  
		Lamp_FlashProcess(); 			//闪烁灯
	}
	else if(g_bLamp_Monochrome == true)
	{
		Lamp_MonochromeProcess();		//单色呼吸灯
	}	
	else if(g_bLampTurnDownEnable == true)
	{
		Lamp_TurnDownGradualProcess(); 	//呼吸灯渐灭
	}
	else if(g_bLamp_Flash == true)
	{	
		Lamp_TurnUpGradualProcess();       //呼吸灯渐亮 
	}

}


