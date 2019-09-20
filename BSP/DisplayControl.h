#ifndef _DISPLAYCONTROL_H_
#define _DISPLAYCONTROL_H_

#include "mydef.h"

#define LED_ALL_OFF() PORTC &=~0x07
#define LED_ALL_ON()  PORTC |=0x07

/************************************************************/
/*	外部变量声明 */											//
extern  u08 g_bTouchKeyEnable;
extern  u08 g_bTouchLedControl;
extern  u08 g_bLampFlashToggle;
extern  u08 g_bLamp_Flash;
extern  u08 g_bLampTurnUpEnable;
extern  u08 g_bLampTurnDownEnable;
extern  u08 g_bLamp_Monochrome;
extern  u08 g_bLamp_ColorfulBreath;
extern  u08 g_bLed_Flash;
extern  u08 g_bLampEffectCanBusCmdDisable;
/************************************************************/
/* 函数说明	*/												//
/************************************************************/
extern void LedFlashProcess(void);
extern void TouchKeyProcess(void);
extern void Lamp_On( u08 u8Lamp_Num );
extern void Lamp_Init(	void );
extern void Lamp_FlashToControl(u08 u8Color,u16 u16FlashTime);
extern void Lamp_FlashProcess(void);
extern void Lamp_FlashLoopToControl(u08 u8Color, u08 u8FlashTime, u08 u8LoopTimes);
extern void Lamp_FlashLoopProcess(void);
extern void Lamp_MonoOrColorfulControl(u08 u8Mode, u08 u8Color);
extern void Lamp_MonochromeProcess(void);
extern void Lamp_ColorfulBreathProcess(void);
extern void Lamp_TurnUp(u08 u8Color);
extern void Lamp_TurnUpGradualProcess(void);
extern void LampTurnDown(void);
extern void Lamp_TurnDownGradualProcess(void);
extern void Lamp_Off(void);
extern void LAMP_Task(void);












#endif 