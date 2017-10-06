/*
 * buttonISR.c
 *
 *  Created on: 
 *      Author: 
 */

#include "buttonISR.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "io.h"
#include "alt_types.h"
#include "lcd.h"
#include "HexDisplay.h"
alt_u8 timer;

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
void buttonISR(void* context)
#else
void buttonISR(void* context, alt_u32 id)
#endif
{
	alt_u8 timer;
    /* Cast context It is important that this be declared 
     * volatile to avoid unwanted compiler optimization.
     */
	alt_u32* pCount = (alt_u32*)context;
    /* Store the value in the PUSHBUTTONS's edge capture register in *context. */
	alt_u8 buttons = IORD(PUSHBUTTONS_BASE, 3);

    /* Reset the PUSHBUTTONS's edge capture register. */
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSHBUTTONS_BASE, 0);

    /* Act upon the interrupt */
    switch (buttons) {
    case 2: // pause/run "button1"
       	timer = IORD(INTERVAL_TIMER_BASE,1);
    	if (timer&0x8)
    	{ // The program is stopped
    		timer = 0x7;
    		LCD_cursor(0,0);
    		LCD_text ("RUNNING...      ");
    	} else  { // The program is running. We want to stop it.
    		timer = 0xB; // set stop=1, start=0, ...
    		LCD_cursor(0,0);
    		LCD_text("PAUSED...       ");
    	}
    	IOWR(INTERVAL_TIMER_BASE,1,timer);
    	break;
    case 4: // clear the count
    	*pCount = 0;
    	HexDisplay((alt_u32*)HEX3_HEX0_BASE, *pCount);	//Displaying count
    	LCD_cursor(0,0);								//Placing cursor on bottom left block of LCD
    	LCD_text("RESET...        ");
    	break;
    case 8:				// toggle BSD/HEX
    	toggleBCD();
    	HexDisplay((alt_u32*)HEX3_HEX0_BASE, *pCount);
    	LCD_cursor(0,0);
    	LCD_text("TOGGLING...      ");

    	break;

    case 1: // should never get here because KEY0 is wired to reset
    default:
    	break;
    }

    /*
     * Read the PIO to delay ISR exit. This is done to prevent a spurious
     * interrupt in systems with high processor -> pio latency and fast
     * interrupts.
     */
    IORD_ALTERA_AVALON_PIO_EDGE_CAP(PUSHBUTTONS_BASE);
}
