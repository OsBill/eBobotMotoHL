#include "includes.h"

/************************************************************/
/* ȫ�ֱ�������������	*/									//
//u08 CAN_TouchBuf[4]     = {0xF1,0x03,0x01,0x0A}			// Touch_Key  ������Ϣ
//u08 CAN_BellBuf [4]	  = {0xF0,0x12,0x08,0x00}			// Buzz       ������Ϣ
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
/* ��������������	*/		
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
// ������
#define	g_Lamp_PWM				OCR2						// ������PWM
/************************************************************/
/* Touch������Ϣ*/											 //
void CAN_TouchKeyPressSend(u08 u8PressOrUnpress)
{
    g_pu8CanSendBuffer[0] = 0xF1;
    g_pu8CanSendBuffer[1] = 0x03;
    if(u8PressOrUnpress == 0x01)							 // ����
    {
       g_pu8CanSendBuffer[2] = 0x01;	
    }
    else if(u8PressOrUnpress == 0x02)						 // �ɿ��˴���
    {
       g_pu8CanSendBuffer[2] = 0x02;	
    }					
    g_pu8CanSendBuffer[3] = 0x0A;							 // ���ֲ�λ����
    CAN_Send_Buffer0(g_pu8CanSendBuffer);
}

/************************************************************/
/* Buzz���Ʒ�����Ϣ*/		
void CAN_BuzzControlSend(u08 u8BeepNumber)
{ 
    g_pu8CanSendBuffer[0] = 0xF0;
    g_pu8CanSendBuffer[1] = 0x12;
    g_pu8CanSendBuffer[2] = 0x08;
    g_pu8CanSendBuffer[3] = u8BeepNumber;
    CAN_Send_Buffer2(g_pu8CanSendBuffer);
}
/************************************************************/
/* ָʾ�ƿ��ƽ���*/	
void LedFlashProcess(void)
{
    if(g_bLed_Flash == true)                              		 // LED��˸����
    {
	    Led_Test_On();                                  	 // ����LED
	    g_bLed_Flash= false;
	}
	else
	{
		Led_Test_Off();										 // Ϩ��LED
	}
}
/************************************************************/
/* �������ƽ���*/	
void TouchKeyProcess(void)
{
	if(g_bTouchKeyEnable == true)							// ʹ�ܴ����������ƹ���
	{
		if(Read_Touch_Button == low)						// �͵�ƽ��Ч
		{
		     
			 if(b_TouchKey_Ok == true)
			 {
			    b_TouchKey_Ok = false;
			    CAN_TouchKeyPressSend(0x01);				// ���ʹ���ʱPress��Ϣ
				
				if(g_bTouchLedControl == true)				// ����ָʾ�ƿ��ƿ�
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
// ˵���������ƴ�ĳ����ɫ����,����������ϰ�
// ���룺�������
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
// ˵���������Ʋ�����ʼ��
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
/* ��˸�ƿ���*/	
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
    if(b_Lamp_Flash_On)										 // Ƶ�����Ĳ��ִ���
	{
		g_Lamp_PWM = 255;									 //
		g_Lamp_Flash--;										 // Ƶ�����ڼ�������һ
		if(	g_Lamp_Flash ==	0 )								 // Ƶ�����ڼ������Ƿ����
		{
			g_Lamp_Flash = g_Lamp_Flash_Time;				 // Ƶ����������ֵ��Ƶ������
			if(	g_Lamp_Flash_Time != 0 )					 // �ж��Ƿ�0��˸���������������0������
			b_Lamp_Flash_On = false;						 // �����ֽ�����ת���𲿷�
		}
	}
	else													 // Ƶ����Ĳ���
	{
		g_Lamp_PWM = 0;										 // ����
		g_Lamp_Flash--;										 // Ƶ�����ڼ�������һ
		if(	g_Lamp_Flash ==	0 )								 // Ƶ�����ڼ������Ƿ����
		{
			g_Lamp_Flash = g_Lamp_Flash_Time;				 // Ƶ��ʱ���������ֵ��Ƶ������
			b_Lamp_Flash_On = true;							 // �𲿷ֽ���ת�����Ĳ���
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
    if (g_bLampFlashToggle == false)									// Ƶ�����Ĳ��ִ���
	{
		g_Lamp_PWM = 0;												//

		g_u8LampFlashTimeCnt++;									// Ƶ�����ڼ�������һ

		if(g_u8LampFlashTimeCnt == g_u8LampFlashTime)				// Ƶ�����ڼ������Ƿ����
		{
			g_u8LampFlashTimeCnt	= 0;
			g_bLampFlashToggle		= true;
		}
	}
	else												// Ƶ����Ĳ���
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
/* ��ɫ�����߲ʺ����ƿ���*/
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
    if(b_Lamp_PwmAdd)								   		 // ����ռ�ձȼ�
	{
		g_Lamp_PWM += 1;							   		 // �������ȿ���
		if(	g_Lamp_PWM == 160 )						   		 // �����ڵ�
		{
			b_Lamp_PwmAdd =	false;					   		 // ת������
		}
	}
    else                                               		 // ����ռ�ձȼ�
	{
		g_Lamp_PWM -= 1;							   		 // �������ȿ���
		if(	g_Lamp_PWM == 0 )						   		 // �����ڵ�
		{
			b_Lamp_PwmAdd =	true;					   		 // ת�Ӳ���
		}
	}
	
}
/************************************************************/
/* �߲ʺ����ƿ���*/
void Lamp_ColorfulBreathProcess(void)
{
    if(b_Lamp_PwmAdd)										  // ����ռ�ձȼ�
	{
		g_Pwm_Add_Delay++;									  // �����ٶȿ���	����LEDռ�ձȵ����ȷ�����(�ɼ��㣬Ҳ�ɲ��)
		if(	g_Pwm_Add_Delay	== g_Pwm_Add_Delay1	)			  // g_Pwm_Add_Delay1:�����Ʊ仯���ٶȿ���
		{
			g_Pwm_Add_Delay	= 0;							  //
			g_Lamp_PWM += 1;								  // �������ȿ���

			if(	g_Lamp_PWM == 15 )							  // �����Ե����ڵ�
			{
				g_Pwm_Add_Delay1 = 4;						  // ���ٶȵ���
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
			else if( g_Lamp_PWM	== 135 )					  // ���õ�255��135�������Ѳ������������ռ�ձ�LED�������ӳ�
			{
				g_Lamp_PWM	= 138;                     		  // �����ȵ�ʱ����΢���һЩ�������¼�����������
				b_Lamp_PwmAdd =	false;						  // ת������
				g_Pwm_Add_Delay1 = 1;						  // ������
			}
		}
	}
	else													  // ����ռ�ձȼ�
	{
		g_Pwm_Add_Delay++;									  // �����ٶȿ���	����LEDռ�ձȵ����ȷ�����(�ɼ��㣬Ҳ�ɲ��)
		if(	g_Pwm_Add_Delay	== g_Pwm_Add_Delay1	)			  // g_Pwm_Add_Delay1:�����Ʊ仯���ٶȿ���
		{
			g_Pwm_Add_Delay	= 0;							  //
			g_Lamp_PWM -= 1;								  // �������ȿ���

			if(	g_Lamp_PWM == 25 )							  // �����Ե����ڵ�
			{
				g_Pwm_Add_Delay1 = 2;						  // ���ٶȵ���
			}
			else if( g_Lamp_PWM	== 15 )						  //
			{
				g_Pwm_Add_Delay1 = 3;						  //
			}
			else if( g_Lamp_PWM	== 5 )						  // ��Ҫ��Ϊ0��5�Ѿ��Ǻܰ��ˣ����ٰ��Ĺ���ʱ�䡣
			{
				g_Lamp_Colour++;							  // ��ɫ��0�졢1�ơ�2�̡�3�ࡢ4����5�ϡ�6��
				if(	g_Lamp_Colour == 7 )
				g_Lamp_Colour =	0;
				Lamp_On( g_Lamp_Colour );					  // ������ɫ
				b_Lamp_PwmAdd =	true;						  // ת������
				g_Pwm_Add_Delay1 = 5;						  // �ظ�
			}
		 }
	}

}
/************************************************************/
/* �ƿ�����������*/
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
/* �ƹرս������*/
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
/* �ƹرտ���*/
void Lamp_Off(void)
{
	g_bLamp_Flash          = false;							 // �ر�Ƶ����ģʽ
	g_bLamp_ColorfulBreath = false;							 // �ر���ˮ��ģʽ
	g_bLamp_Monochrome 	   = false;							 // �رյ�ɫ������ģʽ
	g_bLampTurnUpEnable    = false;							 // �رտ��ƽ���ģʽ
	g_bLampTurnDownEnable  =false;							 // �رչصƽ���ģʽ
	Led_Red_Off();
	Led_Green_Off();
	Led_Blue_Off();
}

extern void LAMP_Task(void)
{
	if(g_bLamp_ColorfulBreath == true)
	{
		Lamp_ColorfulBreathProcess();	//�߲ʺ�����
	}
 	else if(g_bLamp_Flash == true)
	{  
		Lamp_FlashProcess(); 			//��˸��
	}
	else if(g_bLamp_Monochrome == true)
	{
		Lamp_MonochromeProcess();		//��ɫ������
	}	
	else if(g_bLampTurnDownEnable == true)
	{
		Lamp_TurnDownGradualProcess(); 	//�����ƽ���
	}
	else if(g_bLamp_Flash == true)
	{	
		Lamp_TurnUpGradualProcess();       //�����ƽ��� 
	}

}


