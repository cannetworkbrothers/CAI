/*
 * CAN.cpp
 *
 * Created: 11/29/2015 11:06:35 PM
 * Author : Igor
 */ 

#include <avr/io.h>
#include "src/can_interface/can_interface.h"


int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
		DDRB =0b00000001;
		canmsg_t can_message_buffer;
		CAN_USB_INTERFACE can_interface;
		
		PORTB |= (1<<PINB0);
		
		
		can_interface.init();
		can_interface.sendMessage(&can_message_buffer);
		
		

    }
}

