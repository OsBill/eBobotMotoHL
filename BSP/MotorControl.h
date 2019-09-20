#ifndef _MOTORCONTROL_H_
#define _MOTORCONTROL_H_
#include "mydef.h"
/************************************************************/
/*	外部变量声明 */											//
/************************************************************/

extern u08 g_bReset_Motor;
extern u08 g_bHead_Operation;
extern u08 g_bCharge_MotorStop;
extern u08 g_bHandWaving;
extern u08 g_bMoto_Err;
extern u08 g_Head_Target;
extern u08 g_Head_State;
extern u08 g_blrHandWavingOn;
extern u08 g_brHandWaveFinished;
extern u08 g_bHandOriTar_Operation;

/* 函数说明	*/												//
/************************************************************/
extern void Moto_Init(	void );
extern void Motor_MoveToStop(void);
extern void Motor_BrakeProcess(void);
extern void Motor_RunErrorProcess(void);
extern void Hole_ReadMotorState(void);
extern void Current_CollectData(void);
extern void Current_CompareToBrake(void);
extern void Motor_HandToControl(u08 pos, u08 speed, u08 times);
extern void Motor_HandOriTarToControl(u08 origin_target, u08 speed, u08 times);
extern void Motor_HandOriTarToControlProcess(void);
extern void Motor_HandControlProcess(void);
extern void RobotDoubleHandWavingOn(u08 WaveSpeed);
extern void RobotDoubleHandWavingOff(u08 operation);
extern void RobotDoubleHandWavingTask(void);
extern void HandWavingHandler(void);
extern void MotoTask(void);
extern void CAN_MotorErrSend(void);

extern void test(u08 para);

typedef  enum
{
   Hole0 = 0,
   Hole8
}toHoleNo;

#endif