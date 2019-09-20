#include "includes.h"


void WDT_Enable(u08 u8Period)
{
//	asm("cli");
	asm("wdr");

	WDTCR = 0x1F;
	WDTCR = ((1 << WDE) | (u8Period));
}

void WDT_Disable(void)
{
	asm("wdr");

	WDTCR =  ((1 << WDTOE) | (1 << WDE));
	WDTCR = 0x00;
}

void WDT_Reset(void)
{
	asm("wdr");
}















