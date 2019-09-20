#include "includes.h"

/************************************************************/
/* 全局变量、常量定义	*/									//
/* 霍尔元件	    */
//u08	CAN_Test[4]	= {	0xFF,0xBB,0x00,0x00	};
  u08   Current_CollectBuf[4] = { 0xF1,0xAA,0xA1,0x00 };
/* 位置寄存器	*/
#define	c_Head_State			7							// 头部位置上电默认值 4居中
#define	c_Head_Speed_Forward	120+0x37					// 头部运动正转速度上电默认值 慢动作比较合适
#define	c_Head_Speed_Reverse	120+0x37					// 头部运动反转速度上电默认值 慢动作比较合适
#define	c_Holzer_Num			8							// 冗余判断数据，霍尔元件数量最大值，命令的目标位置数大于此数，不处理
#define c_Holzer_positive           0xA0					//磁铁正极
#define c_Holzer_negative           0x60					//磁铁负极

//u08	g_Holzer_Data[c_ADC_NUM+1]={0};								// 霍尔元件采集数据，共有8个模拟霍尔，1个开关型霍尔
/* 电机	        */
// 电机PWM寄存器											//
#define	g_Moto_PWMA				OCR1AL						// 左电机PWMA
#define	g_Moto_PWMB				OCR1BL						// 右电机PWMB
#define	c_Moto_Brake_Time		8							// 电机刹车延时，减缓过冲行程 20mS为单位
#define	c_Moto_Run_Err			300							// 电机堵转计数器 10mS为单位
#define	c_Moto_Err				1							// 头部堵转计数器 以电机堵转为单位 即电机堵转多少次之后停止电机操作


u08 g_Head_Target 		    = 0;
u08	g_bMoto_Err		 	    = false;						// 电机堵转标志位
u08 g_bReset_Motor          = false;
u08 g_bHead_Operation       = false;
u08 g_bHandOriFinish	    = false;
u08 g_bHandOriTar_Operation = false;
u08 g_bCharge_MotorStop  	= false;
u08 g_bHandWaving			= false;
/************************************************************/
/* 变量定义	*/												//
u08 Cur_ADC_Buf          = 0;
u08 Cur_Delay			 = 0;
u08	b_Moto_Run       	 = false;							// 电机运行标志位
u08	b_Moto_Brake         = false;							// 电机刹车标志位
u08 b_Head_Target		 = false;
u08	g_Moto_Brake_Time    = 0;							    // 刹车计时器
u16	g_Moto_Run_Err  	 = 0;								// 电机堵转计数器
u08	g_Moto_Err		 	 = 0;								// 头部堵转计数器

u08 g_Head_Speed_Forward = 0;
u08 g_Head_Speed_Reverse = 0;
u08 g_Head_Loop  		 = 0;
u08 g_Head_State_Buf     = 0;
u08 g_Head_Origin_Buf    = 0;
u08 g_Head_Target_Buf	 = 0;
u08 g_Head_State		 = 0;
u08 g_blrHandWavingOn					= false;
u08 g_u8lrHandWavingSpeed				= 0;
u08 g_u8lrHandWavingLeftSpeed			= 0;
u08 g_u8lrHandWavingRightSpeed			= 0;
u08 g_blHandWaveFinished				= true;
u08 g_brHandWaveFinished				= true;
u16 g_nlrHandWavingCnt					= 0;
int g_nlrHandWavingDeltaTime 			= 0;
u08 g_u08lrHandWaveRoundCnt				= 0;
u08 g_u08lrHandWaveAdjustState			= 0;

/************************************************************/
// 说明：电机参数初始化
//
void Moto_Init(	void )										//
{
	Moto_Sleep_Off();										//
	g_Moto_PWMA	= 0xFF;										// 电机停转	不刹车 0刹车
	g_Moto_PWMB	= 0xFF;										// 电机停转	不刹车 0刹车
	b_Moto_Run = false;										// 电机停转标识位
	g_Moto_Brake_Time =	0x00;								// 电机刹车计时器初值
	b_Moto_Brake = false;									// 电机没有刹车
	g_Moto_Run_Err = 0x00;									// 电机堵转计数器初值
	g_bMoto_Err = false;									// 堵转溢出标识位
	g_Moto_Err = 0x00;										// 头部堵转计数器初值
	g_bReset_Motor = true;

	g_Head_State_Buf = c_Head_State;						// 默认参数	目标位置7
	g_Head_Target_Buf =	c_Head_State;						// 默认参数	目标位置7
	g_Head_Speed_Forward = c_Head_Speed_Forward;			// 默认参数	正转目标速度C_Head_Speed_Forward
	g_Head_Speed_Reverse = c_Head_Speed_Reverse;			// 默认参数	反转目标速度C_Head_Speed_Forward
	g_Head_Loop	= 0;										// 默认参数	循环次数，0进行到目标位置不返回
	g_bHead_Operation = true;								// 申请头部操作
	b_Head_Target =	false;									// 是否循环一遍到达目标目标位置，false：否
}

void Motor_BrakeProcess(void)
{
    if(	b_Moto_Brake )										// 如果电机有刹车，需要解除刹车
	{
		g_Moto_Brake_Time++;
		if(	g_Moto_Brake_Time == c_Moto_Brake_Time )		// 电机刹车定时
		{
			g_Moto_Brake_Time =	0x00;						// 计时器清零
			g_Moto_PWMA	= 0xFF;								// 解除刹车	0刹车
			g_Moto_PWMB	= 0xFF;								// 解除刹车	0刹车
			b_Moto_Brake = false;							// 电机刹车清除
		}
	}
}

void Motor_RunErrorProcess(void)
{
    if(	b_Moto_Run )										// 如果电机有启动，就要进行堵转判定
	{
		g_Moto_Run_Err++;									// 堵转计数器加一
		if(	g_Moto_Run_Err == c_Moto_Run_Err )				// 是否溢出
		{
			g_Moto_Run_Err = 0x00;							// 电机堵转计数器初值
			g_Moto_PWMA	= 0x00;								// 电机刹车
			g_Moto_PWMB	= 0x00;								//
			b_Moto_Brake = true;							// 电机已刹车
			g_bMoto_Err = true;								// 堵转溢出标识位
			b_Moto_Run = false;								// 电机停转标识位

			g_Moto_Err++;									// 头部堵转计数器加一
			if(	g_Moto_Err == c_Moto_Err )					// 头部堵转次数是否溢出
			{
				g_Moto_Err = 0;								// 头部堵转计数器赋初值
				g_bHead_Operation = false;					// 堵转次数达到溢出，停止头部操作
				g_bHandOriTar_Operation =false;
				g_bHandOriFinish = false;
			}
		}
	}
}
void Motor_MoveToStop(void)
{
    g_bHead_Operation = false;
		g_bHandOriTar_Operation = false;
		g_bHandOriFinish = false;
		g_Head_Speed_Forward = 0;
		g_Head_Speed_Reverse = 0;
}

static  u08 Read_HoleState(toHoleNo HoleNo)
{
	  if(HoleNo == Hole0)
	  	{
	  		 if(Read_Hole0 == 0)
	  		 	{
	  		 		  return  0;
	  		 	}

	  	}
	  else if(HoleNo == Hole8)
	  	{
	  		if(Read_Hole8 == 0)
	  		 	{
	  		 		  return  8;
	  		 	}

	  	}
	  	return 10;
}

void Hole_ReadMotorState(void)         			//读电机当前位置
{
     u08 i = 0;
	if(ADC_SampHole_Flag)       //采样完成才处理
	{
		ADC_SampHole_Flag = 0;
		for( i = 1;i <c_ADC_NUM; i++ )					    // 数据累加到IR数据缓冲区
		{
			if((g_Adc_Buf[i] < c_Holzer_negative)||(g_Adc_Buf[i] > c_Holzer_positive) )			// 磁铁接近霍尔元件上方
			{
		  	 g_Head_State = i;
			 return;
			}
		}

		if(Read_Hole0 == 0 )
		{
			g_Head_State = 0;								// 磁铁接近第1个霍尔元件上方
			return;
		}

		if(Read_Hole8 == 0 )
		{
			g_Head_State = 8;								// 磁铁接近第9个霍尔元件上方
			return;
		}


	}



}

void Current_CollectData(void)
{
  static s08 tmp_Cur = 0;
	static u08 Cur_ADC_tmp1 = 0;
	static u08 Cur_ADC_tmp2 = 0;
	static u08 Cur_ADC_tmp3 = 0;
	static u08 Cur_ADC_tmp4 = 0;
	tmp_Cur = g_Head_State_Buf  - g_Head_State;
	Cur_ADC_Buf = g_Adc_Buf[0];
	Current_CollectBuf[0] = 0xF1;
	Current_CollectBuf[1] = 0xBB;
	if(tmp_Cur>0)
	{
	   if((g_Head_State == 0)||(g_Head_State == 1))
	   {
	       Current_CollectBuf[2] = 0xA1;
		   if(Cur_ADC_Buf>Cur_ADC_tmp1)
		   {
	   	      Current_CollectBuf[3] = Cur_ADC_Buf;

	   		  Cur_ADC_tmp1 = Cur_ADC_Buf;
		   }
		   else
		   {
		      Current_CollectBuf[3]= Cur_ADC_tmp1;
		   }

		   CAN_Send_Buffer1(Current_CollectBuf);
	   }
	   else if((g_Head_State == 7)||(g_Head_State == 8))
	   {
	       Current_CollectBuf[2] = 0xA2;
		   if(Cur_ADC_Buf>Cur_ADC_tmp2)
		   {
	   	      Current_CollectBuf[3] = Cur_ADC_Buf;
	   		  Cur_ADC_tmp2 = Cur_ADC_Buf;
		   }
		   else
		   {
		      Current_CollectBuf[3] = Cur_ADC_tmp2;
		   }
		   CAN_Send_Buffer1(Current_CollectBuf);
	   }
	}
	else
	{
	   if((g_Head_State == 0)||(g_Head_State == 1))
	   {
	       Current_CollectBuf[2] = 0xA3;
		   if(Cur_ADC_Buf>Cur_ADC_tmp3)
		   {
	   	      Current_CollectBuf[3] = Cur_ADC_Buf;

	   		  Cur_ADC_tmp3 = Cur_ADC_Buf;
		   }
		   else
		   {
		      Current_CollectBuf[3] = Cur_ADC_tmp3;
		   }
		   CAN_Send_Buffer1(Current_CollectBuf);
	   }
	   else if((g_Head_State == 7)||(g_Head_State == 8))
	   {
	       Current_CollectBuf[2] = 0xA4;
		   if(Cur_ADC_Buf>Cur_ADC_tmp4)
		   {
	   	    Current_CollectBuf[3] = Cur_ADC_Buf;

	   		  Cur_ADC_tmp4 = Cur_ADC_Buf;
		   }
		   else
		   {
		      Current_CollectBuf[3] = Cur_ADC_tmp4;
		   }
		   CAN_Send_Buffer1(Current_CollectBuf);
	   }
	}

}

void Current_CompareToBrake(void)
{
    static s08 tmp1 = 0;
		static s08 tmp2 = 0;
		tmp1 = g_Head_State_Buf  - g_Head_State;
		tmp2 = g_Head_Target - g_Head_State;
		if(g_bReset_Motor == false)
		{
			if((abs(tmp1)>=1)&&(abs(tmp2)>=1))
			{
			   Cur_ADC_Buf = g_Adc_Buf[0];
			}
			else
			{
			   Cur_ADC_Buf = 0;
			}
		}
		else
		{
			Cur_ADC_Buf = 0;
		}

}

void Motor_HandToControl(u08 pos, u08 speed, u08 times)
{
	if(g_bCharge_MotorStop==false)
	{
		g_bReset_Motor = false;


		if((pos <= c_Holzer_Num )||(pos!=g_Head_State))						            // 目标位置数大于最大位置数，不处理
		{
			g_Head_Target =	pos;					        // 传递参数	目标位置

			if(speed > 100)
			{
				speed =	100;			                    // 速度溢出判断
			}

			speed 				   += speed;				// 速度比例调整，×2
			if(speed!=0)
			{
				g_Head_Speed_Forward	= speed+0x36;			// 传递参数	正转速度0～100 无级调速	对应十六进制0x3C～0xA0 抬头和点头的速度差0
				g_Head_Speed_Reverse	= speed+0x36;			// 传递参数	正转速度0～100 无级调速	对应十六进制0x20～0x84 抬头和点头的速度差0
			}
			else
			{
					g_Head_Speed_Forward	= 0x00;			// 传递参数	正转速度0～100 无级调速	对应十六进制0x3C～0xA0 抬头和点头的速度差0
					g_Head_Speed_Reverse	= 0x00;			// 传递参数	正转速度0～100 无级调速	对应十六进制0x20～0x84 抬头和点头的速度差0
			}
			g_Head_Loop				= times;				// 传递参数	循环次数，0进行到目标位置不返回
			g_Head_State_Buf 		= g_Head_State;			// 保存当前位置	用于循环返回的位置
			g_Head_Target_Buf 		= pos;					// 保存目标位置	用于循环的目标位置
			g_bHead_Operation		= true;					// 申请头部操作
			b_Head_Target 			= false;				// 是否到达目标目标位置，false：否
			g_bHandWaving			= true;
		}
	}
}

/*从原位置到目标位置循环运动，*/
void Motor_HandOriTarToControl(u08 origin_target, u08 speed, u08 times)
{
    static u08 pos_target = 0;
	static u08 pos_origin = 0;
	if(!g_bCharge_MotorStop)
	{
		g_bReset_Motor = false;
		pos_target = (origin_target&0x0F);
		pos_origin = (origin_target&0xF0);
		pos_origin = pos_origin>>4;

		if(pos_origin <= c_Holzer_Num)					    // 原位置数大于最大位置数，不处理
		{
			g_Head_Origin_Buf =	pos_origin;					// 传递参数原位置

			if(speed > 100)
			{
				speed =	100;						        // 速度溢出判断
			}

			speed 				   += speed;				// 速度比例调整，×2
			g_Head_Speed_Forward	= speed+0x36;			// 传递参数	正转速度0～100 无级调速	对应十六进制0x3C～0xA0 抬头和点头的速度差0
			g_Head_Speed_Reverse	= speed+0x36;			// 传递参数	反转速度0～100 无级调速	对应十六进制0x20～0x84 抬头和点头的速度差0

		}
		if(pos_target <= c_Holzer_Num)						// 目标位置数大于最大位置数，不处理
		{
		 	g_Head_Loop				= times;				// 传递参数	循环次数，0进行到目标位置不返回
			//g_Head_Origin_Buf 		= pos_origin;		// 传递参数	目标位置
			g_Head_Target_Buf 		= pos_target;			// 保存目标位置	用于循环的目标位置
			g_Head_State_Buf 		= g_Head_Target_Buf;	// 保存当前位置	用于循环返回的位置
			g_bHandOriTar_Operation = true;
			g_bHandOriFinish		= false;
		}
	}
}
void Motor_HandOriTarToControlProcess(void)
{
  static s08 tmp_OriTar = 0;
	static s08 tmp_Target = 0;
	//CAN_Test[2] = g_Head_Target_Buf;
	//CAN_Test[3] = g_Head_Origin_Buf;
	//CAN_Send_Buffer0 (CAN_Test);
	if(g_bHandOriFinish == false)
	{
	   tmp_OriTar = g_Head_Origin_Buf - g_Head_State;
	}
	else
	{
	   tmp_OriTar = 0;
	}
	if(tmp_OriTar > 0)
	{
	   if((g_Head_Speed_Forward >0)&&(g_Head_Speed_Forward < 70))
		 {
			 g_Head_Speed_Forward = 70;
		 }
		 g_Moto_PWMB	= 0x00;			   					// 正转速度
		 g_Moto_PWMA	= g_Head_Speed_Forward;
		 b_Moto_Run = true;									// 电机运行
	}
	else if(tmp_OriTar < 0)
	{
	   if((g_Head_Speed_Reverse >0)&&(g_Head_Speed_Reverse < 70))
		 {
			  g_Head_Speed_Reverse = 70;
		 }

		 g_Moto_PWMA	= 0x00;			   		    		// 反转速度
		 g_Moto_PWMB	= g_Head_Speed_Reverse;
		 b_Moto_Run = true;									// 电机运行
	}
	else
	{
	    g_bHandOriFinish = true;

		if(g_Head_Target_Buf == g_Head_Origin_Buf)
		{
		  g_Moto_PWMB	= 0x00;			   		    		// 刹车
		  g_Moto_PWMA	= 0x00;
			g_bHandOriTar_Operation = false;				// 目标位置与当前位置已经一致，头部控制操作结束
			b_Moto_Brake = true;							// 电机已刹车
			b_Moto_Run = false;								// 电机停转标识位
			g_bHandOriFinish = false;
		}
		else
		{
	        tmp_Target  = g_Head_State_Buf - g_Head_State;
	    	if(tmp_Target>0)
			{
		     	if((g_Head_Speed_Forward >0)&&(g_Head_Speed_Forward < 70))
		    	{
			     	g_Head_Speed_Forward = 70;
		    	}
		    	g_Moto_PWMB	= 0x00;			   				// 正转速度
		    	g_Moto_PWMA	= g_Head_Speed_Forward;
		    	b_Moto_Run = true;							// 电机运行
			}
			else if(tmp_Target<0)
			{
		     	if((g_Head_Speed_Reverse >0)&&(g_Head_Speed_Reverse < 70))
		 		{
			 	 	g_Head_Speed_Reverse = 70;
		 	    }
		 		g_Moto_PWMA	= 0x00;			   		    	// 反转速度
		 		g_Moto_PWMB	= g_Head_Speed_Reverse;
		 		b_Moto_Run = true;							// 电机运行
		    }
			else
			{
		     	g_Moto_PWMA	= 0x00;							// 电机刹车
			 	g_Moto_PWMB	= 0x00;							//
			 	g_Moto_Err = 0x00;							// 头部堵转计数器初值
				g_Moto_Run_Err = 0x00;						// 电机堵转计数器初值
			 	b_Moto_Run = false;							// 电机停转标识位

		     	if(g_Head_Loop == 0 )						// 循环结束
			 	{
				 	g_bHandOriTar_Operation = false;		// 目标位置与当前位置已经一致，头部控制操作结束
				 	b_Moto_Brake = true;					// 电机已刹车
					g_bHandOriFinish = false;
			    }
			 	else
			 	{
				 	 if(g_Head_State == g_Head_Target_Buf)	// 判断是否循环一遍到达目标位置，为一个循环
				 	 {
					 	 g_Head_State_Buf = g_Head_Origin_Buf;
				     }
				 	 else if(g_Head_State == g_Head_Origin_Buf)
				 	 {
					 	 g_Head_State_Buf = g_Head_Target_Buf;
					 	 g_Head_Loop--;						// 循环一次减一
				 	 }
			    }
		     }

	      }

	 }

}


void Motor_HandControlProcess(void)
{
    static s08 tmp = 0;
	if(g_bHead_Operation)
	{
		tmp  = g_Head_Target_Buf	- g_Head_State; 		// 判断电机正转还是反转
			if(Cur_ADC_Buf > 0x20)
			{
				 Cur_Delay++;
				 if(Cur_Delay>2)
				 {
					 g_Moto_PWMA	= 0x00; 						// 正转速度
					 g_Moto_PWMB	= 0x00;

					 g_Head_Speed_Forward = 0;
					 b_Moto_Run = false;							// 电机运行
					 b_Moto_Brake = true;							// 电机已刹车
					 g_bHead_Operation = false;
				 }
			 }
			 else
			 {
				 Cur_Delay = 0;
				 Cur_ADC_Buf = 0;
				 if( tmp > 0)									// 目标位置大于当前位置，电机正转
				 {

					 if((g_Head_Speed_Forward >0)&&(g_Head_Speed_Forward < 70))
					 {
						 g_Head_Speed_Forward = 70;
					 }
					 g_Moto_PWMB	= 0x00; 					// 正转速度
					 g_Moto_PWMA	= g_Head_Speed_Forward;
					 b_Moto_Run = true; 						// 电机运行
				 }
				 else if( tmp < 0 ) 							// 目标位置小于当前位置，电机正转
				 {

					 if((g_Head_Speed_Reverse >0)&&(g_Head_Speed_Reverse < 70))
					 {
						 g_Head_Speed_Reverse = 70;
					 }

					 g_Moto_PWMA	= 0x00; 					// 反转速度
					 g_Moto_PWMB	= g_Head_Speed_Reverse;
					 b_Moto_Run = true; 						// 电机运行
				 }
				 else											// 目标位置与当前位置一样
				 {

					 g_Moto_PWMA	= 0x00; 					// 电机刹车
					 g_Moto_PWMB	= 0x00; 					//
					 g_Moto_Run_Err = 0x00; 					// 电机堵转计数器初值
					 g_Moto_Err = 0x00; 						// 头部堵转计数器初值
					 b_Moto_Run = false;						// 电机停转标识位

					 if(g_Head_Loop == 0 )						// 循环结束
					 {
						 g_bHead_Operation = false; 		// 目标位置与当前位置已经一致，头部控制操作结束
						 b_Moto_Brake = true;					// 电机已刹车
					 }
					 else
					 {
						 if(b_Head_Target)						// 判断是否循环一遍到达目标位置，为一个循环
						 {
							 g_Head_Target_Buf = g_Head_Target; // 重新获取目标位置编号	循环
							 b_Head_Target = false; 			// 回到原位置 往返一遍标志位 false：否
						 }
						 else
						 {
							 g_Head_Target_Buf = g_Head_State_Buf;	 // 重新获取起始位置编号	循环
							 g_Head_Loop--; 						 // 循环一次减一
							 b_Head_Target = true;					 // 是否往返一遍到达目标位置 true：是
						 }
					 }
				 }
			 }

	}


}


void HandWavingHandler(void)
{
	if (g_bHandWaving == true)
	{
		if (g_bHead_Operation == false)
		{
			g_bHandWaving = false;

			g_pu8CanSendBuffer[0] = 0xF0;
			g_pu8CanSendBuffer[1] = 0x60;
			g_pu8CanSendBuffer[2] = g_Head_State_Buf;      //起始位置
			g_pu8CanSendBuffer[3] = g_Head_State;    	   //当前位置，在这也是目标位置

			//CAN_Send_Buffer0(g_pu8CanSendBuffer);
			CAN_Send_Buffer1(g_pu8CanSendBuffer);
			//Delay_ms(50);
		}
	}
}

/************************************************************/
/* 电机异常	*/												//
/************************************************************/
void CAN_MotorErrSend(void)
{
    if( g_bMoto_Err )										// 电机堵转操作
	{
	    g_pu8CanSendBuffer[0] = 0xF1;
		g_pu8CanSendBuffer[1] = 0x02;
		g_pu8CanSendBuffer[2] = 0x08;
		g_pu8CanSendBuffer[3] = 0x01;
		g_bMoto_Err = false;
		CAN_Send_Buffer2( g_pu8CanSendBuffer );			   // 发送异常数据
		g_bLed_Flash	= true;
	}
}



void MotoTask(void)
{
		static u08 Last_Head_State = 10;    //记录上次位置状态


		if(g_bHandOriTar_Operation)
		{
		   Motor_HandOriTarToControlProcess();
		   if(g_Head_State!=Last_Head_State)    //与前一次位置不同时
		   {
			 if(g_bFactoryBoardTest == true)
		  	 {

				g_pu8CanSendBuffer[0] = 0xFF;
				g_pu8CanSendBuffer[1] = 0xAA;
				g_pu8CanSendBuffer[2] = 0xBB;
    			g_pu8CanSendBuffer[3] = g_Head_State;
				CAN_Send_Buffer0(g_pu8CanSendBuffer);

		  	 }
		 	  else if(g_bFactoryFunctionTest == true)
		  	 {
		        g_pu8CanSendBuffer[0] = 0xFF;
				g_pu8CanSendBuffer[1] = 0xAA;
				g_pu8CanSendBuffer[2] = 0xCC;
    			g_pu8CanSendBuffer[3] = g_Head_State;
				CAN_Send_Buffer0(g_pu8CanSendBuffer);
			 }
				//Delay_ms(10);
				Current_CollectData();
		   }
		   Last_Head_State = g_Head_State;

		}
}

extern void test(u08 para)
{

	g_pu8CanSendBuffer[0] = para;
	//g_pu8CanSendBuffer[1] = g_u08CANReceiveBufferWriteIndex;
	//g_pu8CanSendBuffer[2] = g_u08CANReceiveBufferReadIndex;
	//g_pu8CanSendBuffer[3] = g_Head_State;
	//CAN_Send_Buffer0(g_pu8CanSendBuffer);
	CAN_Send_Buffer(g_Adc_Buf,8);
}


/*
void RobotDoubleHandWavingOn(u08 WaveSpeed)
{
	if (WaveSpeed < 70)
	{
		WaveSpeed = 70;
	}

	g_nlrHandWavingDeltaTime		= 0;

	Motor_HandToControl(0, WaveSpeed, 0);

	g_pu8CanSendBuffer[0] = 0x60;
	g_pu8CanSendBuffer[1] = 8;
	g_pu8CanSendBuffer[2] = WaveSpeed;
	g_pu8CanSendBuffer[3] = 0;

	CAN_Send_Buffer0(g_pu8CanSendBuffer);

	Delay_ms(50);

	g_u8lrHandWavingSpeed			= WaveSpeed;
	g_u8lrHandWavingLeftSpeed		= WaveSpeed;
	g_u8lrHandWavingRightSpeed		= WaveSpeed;

	g_blHandWaveFinished			= false;
	g_brHandWaveFinished			= false;
	g_blrHandWavingOn				= true;
	g_nlrHandWavingCnt				= 0;
	g_u08lrHandWaveRoundCnt			= 0;
	g_u08lrHandWaveAdjustState		= 0;
}

void RobotDoubleHandWavingOff(u08 operation)
{
	if (operation == true)
	{
		Motor_HandToControl(7,50, 0);

		g_pu8CanSendBuffer[0] = 0x60;
		g_pu8CanSendBuffer[1] = 7;
		g_pu8CanSendBuffer[2] = 50;
		g_pu8CanSendBuffer[3] = 0;
		CAN_Send_Buffer0(g_pu8CanSendBuffer);

		Delay_ms(50);
	}

	g_blrHandWavingOn	= false;
}*/
/*
void RobotDoubleHandWavingTask(void)
{
	int delta;
	int tmp;
	int k;

	if (g_blrHandWavingOn == true)
	{
		g_nlrHandWavingCnt++;

		if ((g_blHandWaveFinished == true) && (g_brHandWaveFinished == true))
		{
			#ifdef DEBUG_DOUBLE_HAND_WAVE
			g_pu8CanSendBuffer[0] = 0xFF;
			g_pu8CanSendBuffer[1] = 0xDB;
			g_pu8CanSendBuffer[2] = g_nlrHandWavingDeltaTime >> 8;
			g_pu8CanSendBuffer[3] = g_nlrHandWavingDeltaTime;
			CAN_Send_Buffer1(g_pu8CanSendBuffer);
			Delay_ms(30);
			#endif

			if (g_u08lrHandWaveRoundCnt < 3)
			{
				g_u08lrHandWaveRoundCnt++;
			}

			#ifdef DEBUG_DOUBLE_HAND_WAVE
			g_pu8CanSendBuffer[0] = 0xFF;
			g_pu8CanSendBuffer[1] = 0xDD;
			g_pu8CanSendBuffer[2] = 0;
			g_pu8CanSendBuffer[3] = g_u08lrHandWaveRoundCnt;
			CAN_Send_Buffer1(g_pu8CanSendBuffer);
			Delay_ms(30);
			#endif

			if (g_u08lrHandWaveRoundCnt > 1)
			{
				if (g_u08lrHandWaveRoundCnt == 2)
				{
					if (g_nlrHandWavingDeltaTime > 0)
					{
						g_u08lrHandWaveAdjustState = 1;
					}
					else if (g_nlrHandWavingDeltaTime < 0)
					{
						g_u08lrHandWaveAdjustState = 2;
					}
					else
					{
						g_u08lrHandWaveAdjustState = 0;
					}
				}

				if (g_u08lrHandWaveAdjustState == 1)
				{
					if (abs(g_nlrHandWavingDeltaTime) > 5)
					{
						if (g_nlrHandWavingDeltaTime > 0)
						{
							g_u8lrHandWavingLeftSpeed = g_u8lrHandWavingLeftSpeed - (abs(g_nlrHandWavingDeltaTime)*2/10);
						}
						else
						{
							g_u8lrHandWavingLeftSpeed = g_u8lrHandWavingLeftSpeed + (abs(g_nlrHandWavingDeltaTime)*2/10);
						}

						#ifdef DEBUG_DOUBLE_HAND_WAVE
						g_pu8CanSendBuffer[0] = 0xFF;
						g_pu8CanSendBuffer[1] = 0xD1;
						g_pu8CanSendBuffer[2] = 0;
						g_pu8CanSendBuffer[3] = g_u8lrHandWavingLeftSpeed;
						CAN_Send_Buffer1(g_pu8CanSendBuffer);
						Delay_ms(30);
						#endif

						if (g_u8lrHandWavingLeftSpeed > 100)
						{
							g_u8lrHandWavingLeftSpeed = 100;
						}

						if (g_u8lrHandWavingLeftSpeed < 30)
						{
							g_u8lrHandWavingLeftSpeed = 30;
						}
					}
				}
				else if (g_u08lrHandWaveAdjustState == 2)
				{
					if (abs(g_nlrHandWavingDeltaTime) > 5)
					{
						if (g_nlrHandWavingDeltaTime > 0)
						{
							g_u8lrHandWavingRightSpeed = g_u8lrHandWavingRightSpeed + (abs(g_nlrHandWavingDeltaTime)*2/10);
						}
						else
						{
							g_u8lrHandWavingRightSpeed = g_u8lrHandWavingRightSpeed - (abs(g_nlrHandWavingDeltaTime)*2/10);
						}

						#ifdef DEBUG_DOUBLE_HAND_WAVE
						g_pu8CanSendBuffer[0] = 0xFF;
						g_pu8CanSendBuffer[1] = 0xD2;
						g_pu8CanSendBuffer[2] = 0;
						g_pu8CanSendBuffer[3] = g_u8lrHandWavingRightSpeed;
						CAN_Send_Buffer1(g_pu8CanSendBuffer);
						Delay_ms(30);
						#endif

						if (g_u8lrHandWavingRightSpeed > 100)
						{
							g_u8lrHandWavingRightSpeed = 100;
						}

						if (g_u8lrHandWavingRightSpeed < 30)
						{
							g_u8lrHandWavingRightSpeed = 30;
						}
					}
				}
			}

			#ifdef DEBUG_DOUBLE_HAND_WAVE
			g_pu8CanSendBuffer[0] = 0xFF;
			g_pu8CanSendBuffer[1] = 0xDE;
			g_pu8CanSendBuffer[2] = 0;
			g_pu8CanSendBuffer[3] = g_u08lrHandWaveAdjustState;
			CAN_Send_Buffer1(g_pu8CanSendBuffer);
			Delay_ms(30);

			g_pu8CanSendBuffer[0] = 0xFF;
			g_pu8CanSendBuffer[1] = 0xDA;
			g_pu8CanSendBuffer[2] = g_u8lrHandWavingLeftSpeed;
			g_pu8CanSendBuffer[3] = g_u8lrHandWavingRightSpeed;
			CAN_Send_Buffer1(g_pu8CanSendBuffer);
			Delay_ms(30);
			#endif
			Motor_HandToControl(8, g_u8lrHandWavingLeftSpeed, 3);


			g_pu8CanSendBuffer[0] = 0x60;
			g_pu8CanSendBuffer[1] = 0;
			g_pu8CanSendBuffer[2] = g_u8lrHandWavingRightSpeed;
			g_pu8CanSendBuffer[3] = 3;

			CAN_Send_Buffer0(g_pu8CanSendBuffer);
			CAN_Send_Buffer1(g_pu8CanSendBuffer);

			Delay_ms(50);

			//CAN_Send_Buffer1(g_pu8CanSendBuffer);
			//Delay_ms(30);
			//CAN_Send_Buffer2(g_pu8CanSendBuffer);
			//Delay_ms(30);

			g_nlrHandWavingCnt				= 0;
			g_blHandWaveFinished 			= false;
			g_brHandWaveFinished			= false;
			g_nlrHandWavingDeltaTime		= 0;

		//	Delay_ms(50);
		}
		else
		{
			if (g_blHandWaveFinished == true)
			{
				g_nlrHandWavingDeltaTime++;
			}

			if (g_brHandWaveFinished == true)
			{
				g_nlrHandWavingDeltaTime--;
			}

			if (g_nlrHandWavingCnt > 600)
			{
				Motor_HandToControl(0, g_u8lrHandWavingSpeed, 0);
				g_pu8CanSendBuffer[0] = 0x60;
				g_pu8CanSendBuffer[1] = 8;
				g_pu8CanSendBuffer[2] = g_u8lrHandWavingSpeed;
				g_pu8CanSendBuffer[3] = 0;

				CAN_Send_Buffer0(g_pu8CanSendBuffer);
				CAN_Send_Buffer1(g_pu8CanSendBuffer);

				Delay_ms(50);

				g_nlrHandWavingCnt 			= 0;
				g_blHandWaveFinished		= false;
				g_brHandWaveFinished		= false;
				g_nlrHandWavingDeltaTime 	= 0;
				g_u08lrHandWaveRoundCnt		= 0;
				g_u08lrHandWaveAdjustState	= 0;
			}
		}
	}
}
*/

/*
void HandWavingHandler(void)		//左手
{
	if (g_bHandWaving == true)
	{
		if (g_bHead_Operation == false)
		{
			if (g_blrHandWavingOn == false)
			{
				g_bHandWaving = false;

				g_pu8CanSendBuffer[0] = 0xF1;
				g_pu8CanSendBuffer[1] = 0x50;
				g_pu8CanSendBuffer[2] = 0x00;
				g_pu8CanSendBuffer[3] = g_Head_State;
				CAN_Send_Buffer0(g_pu8CanSendBuffer);

				Delay_ms(50);
			}
			else
			{
				g_blHandWaveFinished = true;
			}
		}
	}
}*/

