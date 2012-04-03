
#include "izuvo.h"
#include "utility.h"

extern unsigned char mTimer0_Flag;

int main (void)
{
	init_hardware();	// init Hardware
	init_system();		// init system parameter
	sei();				// Enable Interrupt

	u_puts(VERSION);

	while (1)			// Start Task
	{
		process_tmr();	// process timer event			
		process_cmd();	// process uart command
	}

	return(0);

}
