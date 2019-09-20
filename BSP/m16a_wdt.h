#ifndef __M16A_WDT_H_
#define __M16A_WDT_H_

#include "iom16v.h"
#include "board.h"

#define WDT_PERIOD_16_3ms		0x00
#define WDT_PERIOD_32_5ms		0x01
#define WDT_PERIOD_65ms			0x02
#define WDT_PERIOD_130ms		0x03
#define WDT_PERIOD_260ms		0x04
#define WDT_PERIOD_520ms		0x05
#define WDT_PERIOD_1s			0x06
#define WDT_PERIOD_2_1s			0x07


extern void WDT_Enable(u08 u8Period);
extern void WDT_Disable(void);
extern void WDT_Reset(void);

#endif






























