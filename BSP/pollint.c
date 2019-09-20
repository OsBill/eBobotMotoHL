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

    switch(_1MsCnt)       // 10msִ��һ��
    {
        case 1:

            break;
        case 2:

            break;
        case 3:
				//Led_Test_Toggle();
			Hole_ReadMotorState();	 //�����λ��
            break;
        case 4:
			MotoTask();

            break;
        case 5:
			 Motor_BrakeProcess();							// ����ƶ�
			 Motor_RunErrorProcess();						// �����ת�쳣
            break;
        case 6:
			HandWavingHandler();	  //������λ����Ϣ��ͬ������


            break;
        case 7:
			LAMP_Task();      //��Ч����


            break;
        case 8:


            break;
        case 9:


            break;
        case 0:
            switch(_10MsCnt)        //100msִ��һ��
            {
                case 1:
       				LedFlashProcess();
                    break;
                case 2:
					TouchKeyProcess();   //������������
                    break;
                case 3:
					CAN_MotorErrSend();		// ��ת�쳣��Ϣ����
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
                    switch(_100MsCnt)   // 1 ����ִ��һ��
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
							if (turnonDelay)	//δ����״̬�²�����������ѹ���ȣ����AD�쳣���ӳ�һ��
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
