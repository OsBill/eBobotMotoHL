#include "includes.h"


extern volatile unsigned char pollintack;
extern void Lamp_Enable(void);



static unsigned char _1MsCnt,_10MsCnt,_100MsCnt;
unsigned char turnonDelay=true;



extern void pollint(void)
{
    if (!pollintack)
    {

        return;

    }
    pollintack = 0;
    if (++_1MsCnt >= 10)
    {
        _1MsCnt = 0;
        if (++_10MsCnt >= 10)
        {
            _10MsCnt = 0;
            if (++_100MsCnt >= 10)
            {
                _100MsCnt = 0;
            }
        }
    }

    switch(_1MsCnt)       // 10ms执行一次
    {
        case 1:

            break;
        case 2:

            break;
        case 3:
				//Led_Test_Toggle();
			Hole_ReadMotorState();	 //读电机位置
            break;
        case 4:
			MotoTask();

            break;
        case 5:
			 Motor_BrakeProcess();							// 电机制动
			 Motor_RunErrorProcess();						// 电机堵转异常
            break;
        case 6:
			HandWavingHandler();	  //发送手位置信息，同步作用


            break;
        case 7:
			LAMP_Task();      //灯效处理


            break;
        case 8:


            break;
        case 9:


            break;
        case 0:
            switch(_10MsCnt)        //100ms执行一次
            {
                case 1:
       				LedFlashProcess();
                    break;
                case 2:
					TouchKeyProcess();   //触摸按键处理
                    break;
                case 3:
					CAN_MotorErrSend();		// 堵转异常信息发送
                    break;
                case 4:
                    break;
                case 5:




                    break;
                case 6:
					Lamp_Enable();
                    break;
                case 7:
                    break;
                case 8:
                    break;
                case 9:
                    break;
                case 0:
                    switch(_100MsCnt)   // 1 秒钟执行一次
                    {
                        case 1:
							WDT_Reset();
                            break;
                        case 2:

                            break;
                        case 3:
							//test(0XBB);
							if(HoleTest)
							{
								test(0XAA);
							}


                            break;
                        case 4:

                            break;
                        case 5:
							if (turnonDelay)	//未开机状态下插入适配器电压不稳，造成AD异常，延迟一秒
							{
								Moto_Init();
								turnonDelay = false;
							}


                            break;
                        case 6:
							WDT_Reset();

                            break;
                        case 7:
                            break;
                        case 8:
                            break;
                        case 9:
                            break;
                    }
                    break;
            }
            break;
    }
}
