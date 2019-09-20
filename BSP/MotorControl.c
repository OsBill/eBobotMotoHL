#include "includes.h"

/************************************************************/
/* ȫ�ֱ�������������	*/									//
/* ����Ԫ��	    */
//u08	CAN_Test[4]	= {	0xFF,0xBB,0x00,0x00	};
  u08   Current_CollectBuf[4] = { 0xF1,0xAA,0xA1,0x00 };
/* λ�üĴ���	*/
#define	c_Head_State			7							// ͷ��λ���ϵ�Ĭ��ֵ 4����
#define	c_Head_Speed_Forward	120+0x37					// ͷ���˶���ת�ٶ��ϵ�Ĭ��ֵ �������ȽϺ���
#define	c_Head_Speed_Reverse	120+0x37					// ͷ���˶���ת�ٶ��ϵ�Ĭ��ֵ �������ȽϺ���
#define	c_Holzer_Num			8							// �����ж����ݣ�����Ԫ���������ֵ�������Ŀ��λ�������ڴ�����������
#define c_Holzer_positive           0xA0					//��������
#define c_Holzer_negative           0x60					//��������

//u08	g_Holzer_Data[c_ADC_NUM+1]={0};								// ����Ԫ���ɼ����ݣ�����8��ģ�������1�������ͻ���
/* ���	        */
// ���PWM�Ĵ���											//
#define	g_Moto_PWMA				OCR1AL						// ����PWMA
#define	g_Moto_PWMB				OCR1BL						// �ҵ��PWMB
#define	c_Moto_Brake_Time		8							// ���ɲ����ʱ�����������г� 20mSΪ��λ
#define	c_Moto_Run_Err			300							// �����ת������ 10mSΪ��λ
#define	c_Moto_Err				1							// ͷ����ת������ �Ե����תΪ��λ �������ת���ٴ�֮��ֹͣ�������


u08 g_Head_Target 		    = 0;
u08	g_bMoto_Err		 	    = false;						// �����ת��־λ
u08 g_bReset_Motor          = false;
u08 g_bHead_Operation       = false;
u08 g_bHandOriFinish	    = false;
u08 g_bHandOriTar_Operation = false;
u08 g_bCharge_MotorStop  	= false;
u08 g_bHandWaving			= false;
/************************************************************/
/* ��������	*/												//
u08 Cur_ADC_Buf          = 0;
u08 Cur_Delay			 = 0;
u08	b_Moto_Run       	 = false;							// ������б�־λ
u08	b_Moto_Brake         = false;							// ���ɲ����־λ
u08 b_Head_Target		 = false;
u08	g_Moto_Brake_Time    = 0;							    // ɲ����ʱ��
u16	g_Moto_Run_Err  	 = 0;								// �����ת������
u08	g_Moto_Err		 	 = 0;								// ͷ����ת������

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
// ˵�������������ʼ��
//
void Moto_Init(	void )										//
{
	Moto_Sleep_Off();										//
	g_Moto_PWMA	= 0xFF;										// ���ͣת	��ɲ�� 0ɲ��
	g_Moto_PWMB	= 0xFF;										// ���ͣת	��ɲ�� 0ɲ��
	b_Moto_Run = false;										// ���ͣת��ʶλ
	g_Moto_Brake_Time =	0x00;								// ���ɲ����ʱ����ֵ
	b_Moto_Brake = false;									// ���û��ɲ��
	g_Moto_Run_Err = 0x00;									// �����ת��������ֵ
	g_bMoto_Err = false;									// ��ת�����ʶλ
	g_Moto_Err = 0x00;										// ͷ����ת��������ֵ
	g_bReset_Motor = true;

	g_Head_State_Buf = c_Head_State;						// Ĭ�ϲ���	Ŀ��λ��7
	g_Head_Target_Buf =	c_Head_State;						// Ĭ�ϲ���	Ŀ��λ��7
	g_Head_Speed_Forward = c_Head_Speed_Forward;			// Ĭ�ϲ���	��תĿ���ٶ�C_Head_Speed_Forward
	g_Head_Speed_Reverse = c_Head_Speed_Reverse;			// Ĭ�ϲ���	��תĿ���ٶ�C_Head_Speed_Forward
	g_Head_Loop	= 0;										// Ĭ�ϲ���	ѭ��������0���е�Ŀ��λ�ò�����
	g_bHead_Operation = true;								// ����ͷ������
	b_Head_Target =	false;									// �Ƿ�ѭ��һ�鵽��Ŀ��Ŀ��λ�ã�false����
}

void Motor_BrakeProcess(void)
{
    if(	b_Moto_Brake )										// ��������ɲ������Ҫ���ɲ��
	{
		g_Moto_Brake_Time++;
		if(	g_Moto_Brake_Time == c_Moto_Brake_Time )		// ���ɲ����ʱ
		{
			g_Moto_Brake_Time =	0x00;						// ��ʱ������
			g_Moto_PWMA	= 0xFF;								// ���ɲ��	0ɲ��
			g_Moto_PWMB	= 0xFF;								// ���ɲ��	0ɲ��
			b_Moto_Brake = false;							// ���ɲ�����
		}
	}
}

void Motor_RunErrorProcess(void)
{
    if(	b_Moto_Run )										// ����������������Ҫ���ж�ת�ж�
	{
		g_Moto_Run_Err++;									// ��ת��������һ
		if(	g_Moto_Run_Err == c_Moto_Run_Err )				// �Ƿ����
		{
			g_Moto_Run_Err = 0x00;							// �����ת��������ֵ
			g_Moto_PWMA	= 0x00;								// ���ɲ��
			g_Moto_PWMB	= 0x00;								//
			b_Moto_Brake = true;							// �����ɲ��
			g_bMoto_Err = true;								// ��ת�����ʶλ
			b_Moto_Run = false;								// ���ͣת��ʶλ

			g_Moto_Err++;									// ͷ����ת��������һ
			if(	g_Moto_Err == c_Moto_Err )					// ͷ����ת�����Ƿ����
			{
				g_Moto_Err = 0;								// ͷ����ת����������ֵ
				g_bHead_Operation = false;					// ��ת�����ﵽ�����ֹͣͷ������
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

void Hole_ReadMotorState(void)         			//�������ǰλ��
{
     u08 i = 0;
	if(ADC_SampHole_Flag)       //������ɲŴ���
	{
		ADC_SampHole_Flag = 0;
		for( i = 1;i <c_ADC_NUM; i++ )					    // �����ۼӵ�IR���ݻ�����
		{
			if((g_Adc_Buf[i] < c_Holzer_negative)||(g_Adc_Buf[i] > c_Holzer_positive) )			// �����ӽ�����Ԫ���Ϸ�
			{
		  	 g_Head_State = i;
			 return;
			}
		}

		if(Read_Hole0 == 0 )
		{
			g_Head_State = 0;								// �����ӽ���1������Ԫ���Ϸ�
			return;
		}

		if(Read_Hole8 == 0 )
		{
			g_Head_State = 8;								// �����ӽ���9������Ԫ���Ϸ�
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


		if((pos <= c_Holzer_Num )||(pos!=g_Head_State))						            // Ŀ��λ�����������λ������������
		{
			g_Head_Target =	pos;					        // ���ݲ���	Ŀ��λ��

			if(speed > 100)
			{
				speed =	100;			                    // �ٶ�����ж�
			}

			speed 				   += speed;				// �ٶȱ�����������2
			if(speed!=0)
			{
				g_Head_Speed_Forward	= speed+0x36;			// ���ݲ���	��ת�ٶ�0��100 �޼�����	��Ӧʮ������0x3C��0xA0 ̧ͷ�͵�ͷ���ٶȲ�0
				g_Head_Speed_Reverse	= speed+0x36;			// ���ݲ���	��ת�ٶ�0��100 �޼�����	��Ӧʮ������0x20��0x84 ̧ͷ�͵�ͷ���ٶȲ�0
			}
			else
			{
					g_Head_Speed_Forward	= 0x00;			// ���ݲ���	��ת�ٶ�0��100 �޼�����	��Ӧʮ������0x3C��0xA0 ̧ͷ�͵�ͷ���ٶȲ�0
					g_Head_Speed_Reverse	= 0x00;			// ���ݲ���	��ת�ٶ�0��100 �޼�����	��Ӧʮ������0x20��0x84 ̧ͷ�͵�ͷ���ٶȲ�0
			}
			g_Head_Loop				= times;				// ���ݲ���	ѭ��������0���е�Ŀ��λ�ò�����
			g_Head_State_Buf 		= g_Head_State;			// ���浱ǰλ��	����ѭ�����ص�λ��
			g_Head_Target_Buf 		= pos;					// ����Ŀ��λ��	����ѭ����Ŀ��λ��
			g_bHead_Operation		= true;					// ����ͷ������
			b_Head_Target 			= false;				// �Ƿ񵽴�Ŀ��Ŀ��λ�ã�false����
			g_bHandWaving			= true;
		}
	}
}

/*��ԭλ�õ�Ŀ��λ��ѭ���˶���*/
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

		if(pos_origin <= c_Holzer_Num)					    // ԭλ�����������λ������������
		{
			g_Head_Origin_Buf =	pos_origin;					// ���ݲ���ԭλ��

			if(speed > 100)
			{
				speed =	100;						        // �ٶ�����ж�
			}

			speed 				   += speed;				// �ٶȱ�����������2
			g_Head_Speed_Forward	= speed+0x36;			// ���ݲ���	��ת�ٶ�0��100 �޼�����	��Ӧʮ������0x3C��0xA0 ̧ͷ�͵�ͷ���ٶȲ�0
			g_Head_Speed_Reverse	= speed+0x36;			// ���ݲ���	��ת�ٶ�0��100 �޼�����	��Ӧʮ������0x20��0x84 ̧ͷ�͵�ͷ���ٶȲ�0

		}
		if(pos_target <= c_Holzer_Num)						// Ŀ��λ�����������λ������������
		{
		 	g_Head_Loop				= times;				// ���ݲ���	ѭ��������0���е�Ŀ��λ�ò�����
			//g_Head_Origin_Buf 		= pos_origin;		// ���ݲ���	Ŀ��λ��
			g_Head_Target_Buf 		= pos_target;			// ����Ŀ��λ��	����ѭ����Ŀ��λ��
			g_Head_State_Buf 		= g_Head_Target_Buf;	// ���浱ǰλ��	����ѭ�����ص�λ��
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
		 g_Moto_PWMB	= 0x00;			   					// ��ת�ٶ�
		 g_Moto_PWMA	= g_Head_Speed_Forward;
		 b_Moto_Run = true;									// �������
	}
	else if(tmp_OriTar < 0)
	{
	   if((g_Head_Speed_Reverse >0)&&(g_Head_Speed_Reverse < 70))
		 {
			  g_Head_Speed_Reverse = 70;
		 }

		 g_Moto_PWMA	= 0x00;			   		    		// ��ת�ٶ�
		 g_Moto_PWMB	= g_Head_Speed_Reverse;
		 b_Moto_Run = true;									// �������
	}
	else
	{
	    g_bHandOriFinish = true;

		if(g_Head_Target_Buf == g_Head_Origin_Buf)
		{
		  g_Moto_PWMB	= 0x00;			   		    		// ɲ��
		  g_Moto_PWMA	= 0x00;
			g_bHandOriTar_Operation = false;				// Ŀ��λ���뵱ǰλ���Ѿ�һ�£�ͷ�����Ʋ�������
			b_Moto_Brake = true;							// �����ɲ��
			b_Moto_Run = false;								// ���ͣת��ʶλ
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
		    	g_Moto_PWMB	= 0x00;			   				// ��ת�ٶ�
		    	g_Moto_PWMA	= g_Head_Speed_Forward;
		    	b_Moto_Run = true;							// �������
			}
			else if(tmp_Target<0)
			{
		     	if((g_Head_Speed_Reverse >0)&&(g_Head_Speed_Reverse < 70))
		 		{
			 	 	g_Head_Speed_Reverse = 70;
		 	    }
		 		g_Moto_PWMA	= 0x00;			   		    	// ��ת�ٶ�
		 		g_Moto_PWMB	= g_Head_Speed_Reverse;
		 		b_Moto_Run = true;							// �������
		    }
			else
			{
		     	g_Moto_PWMA	= 0x00;							// ���ɲ��
			 	g_Moto_PWMB	= 0x00;							//
			 	g_Moto_Err = 0x00;							// ͷ����ת��������ֵ
				g_Moto_Run_Err = 0x00;						// �����ת��������ֵ
			 	b_Moto_Run = false;							// ���ͣת��ʶλ

		     	if(g_Head_Loop == 0 )						// ѭ������
			 	{
				 	g_bHandOriTar_Operation = false;		// Ŀ��λ���뵱ǰλ���Ѿ�һ�£�ͷ�����Ʋ�������
				 	b_Moto_Brake = true;					// �����ɲ��
					g_bHandOriFinish = false;
			    }
			 	else
			 	{
				 	 if(g_Head_State == g_Head_Target_Buf)	// �ж��Ƿ�ѭ��һ�鵽��Ŀ��λ�ã�Ϊһ��ѭ��
				 	 {
					 	 g_Head_State_Buf = g_Head_Origin_Buf;
				     }
				 	 else if(g_Head_State == g_Head_Origin_Buf)
				 	 {
					 	 g_Head_State_Buf = g_Head_Target_Buf;
					 	 g_Head_Loop--;						// ѭ��һ�μ�һ
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
		tmp  = g_Head_Target_Buf	- g_Head_State; 		// �жϵ����ת���Ƿ�ת
			if(Cur_ADC_Buf > 0x20)
			{
				 Cur_Delay++;
				 if(Cur_Delay>2)
				 {
					 g_Moto_PWMA	= 0x00; 						// ��ת�ٶ�
					 g_Moto_PWMB	= 0x00;

					 g_Head_Speed_Forward = 0;
					 b_Moto_Run = false;							// �������
					 b_Moto_Brake = true;							// �����ɲ��
					 g_bHead_Operation = false;
				 }
			 }
			 else
			 {
				 Cur_Delay = 0;
				 Cur_ADC_Buf = 0;
				 if( tmp > 0)									// Ŀ��λ�ô��ڵ�ǰλ�ã������ת
				 {

					 if((g_Head_Speed_Forward >0)&&(g_Head_Speed_Forward < 70))
					 {
						 g_Head_Speed_Forward = 70;
					 }
					 g_Moto_PWMB	= 0x00; 					// ��ת�ٶ�
					 g_Moto_PWMA	= g_Head_Speed_Forward;
					 b_Moto_Run = true; 						// �������
				 }
				 else if( tmp < 0 ) 							// Ŀ��λ��С�ڵ�ǰλ�ã������ת
				 {

					 if((g_Head_Speed_Reverse >0)&&(g_Head_Speed_Reverse < 70))
					 {
						 g_Head_Speed_Reverse = 70;
					 }

					 g_Moto_PWMA	= 0x00; 					// ��ת�ٶ�
					 g_Moto_PWMB	= g_Head_Speed_Reverse;
					 b_Moto_Run = true; 						// �������
				 }
				 else											// Ŀ��λ���뵱ǰλ��һ��
				 {

					 g_Moto_PWMA	= 0x00; 					// ���ɲ��
					 g_Moto_PWMB	= 0x00; 					//
					 g_Moto_Run_Err = 0x00; 					// �����ת��������ֵ
					 g_Moto_Err = 0x00; 						// ͷ����ת��������ֵ
					 b_Moto_Run = false;						// ���ͣת��ʶλ

					 if(g_Head_Loop == 0 )						// ѭ������
					 {
						 g_bHead_Operation = false; 		// Ŀ��λ���뵱ǰλ���Ѿ�һ�£�ͷ�����Ʋ�������
						 b_Moto_Brake = true;					// �����ɲ��
					 }
					 else
					 {
						 if(b_Head_Target)						// �ж��Ƿ�ѭ��һ�鵽��Ŀ��λ�ã�Ϊһ��ѭ��
						 {
							 g_Head_Target_Buf = g_Head_Target; // ���»�ȡĿ��λ�ñ��	ѭ��
							 b_Head_Target = false; 			// �ص�ԭλ�� ����һ���־λ false����
						 }
						 else
						 {
							 g_Head_Target_Buf = g_Head_State_Buf;	 // ���»�ȡ��ʼλ�ñ��	ѭ��
							 g_Head_Loop--; 						 // ѭ��һ�μ�һ
							 b_Head_Target = true;					 // �Ƿ�����һ�鵽��Ŀ��λ�� true����
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
			g_pu8CanSendBuffer[2] = g_Head_State_Buf;      //��ʼλ��
			g_pu8CanSendBuffer[3] = g_Head_State;    	   //��ǰλ�ã�����Ҳ��Ŀ��λ��

			//CAN_Send_Buffer0(g_pu8CanSendBuffer);
			CAN_Send_Buffer1(g_pu8CanSendBuffer);
			//Delay_ms(50);
		}
	}
}

/************************************************************/
/* ����쳣	*/												//
/************************************************************/
void CAN_MotorErrSend(void)
{
    if( g_bMoto_Err )										// �����ת����
	{
	    g_pu8CanSendBuffer[0] = 0xF1;
		g_pu8CanSendBuffer[1] = 0x02;
		g_pu8CanSendBuffer[2] = 0x08;
		g_pu8CanSendBuffer[3] = 0x01;
		g_bMoto_Err = false;
		CAN_Send_Buffer2( g_pu8CanSendBuffer );			   // �����쳣����
		g_bLed_Flash	= true;
	}
}



void MotoTask(void)
{
		static u08 Last_Head_State = 10;    //��¼�ϴ�λ��״̬


		if(g_bHandOriTar_Operation)
		{
		   Motor_HandOriTarToControlProcess();
		   if(g_Head_State!=Last_Head_State)    //��ǰһ��λ�ò�ͬʱ
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
void HandWavingHandler(void)		//����
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

