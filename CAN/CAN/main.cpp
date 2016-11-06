/*
 * CAN.cpp
 *
 * Created: 11/29/2015 11:06:35 PM
 * Author : Igor
 */ 

#ifndef MAIN_H_
	#include "main.hpp"
#endif
#include "src/can_interface/can_interface.h"
#include <util/delay.h>
#include <stdlib.h>

int main(void)
{
	DDRB = 0b00000010; // PB1 as output  and PB2 output
	PORTB |= (1<<PB1); // light the LED for fun :);
	
	//wait 10 seconds for start logging
	_delay_ms(10*1000);
	CREATE_LOGGER(logger);
	LOG(logger, (char*) "Start!!!")

 	//canmsg_t can_message_buffer;
 	CanInterface can_interface;
	LOG(logger, (char*) "Initialized CanInterface!!!")
 	can_interface.init();
	//can_interface.init_filtering();
	
	// create buffer for CAN message
	canmsg_t * p_can_message_1 = (canmsg_t*) malloc(sizeof(canmsg_t));
	
	p_can_message_1->id = 0xbf3c;
	p_can_message_1->flags.rtr = 0;
	p_can_message_1->flags.extended = 1;
	p_can_message_1->data[0] = 0x0;
	p_can_message_1->data[1] = 0x80;
	p_can_message_1->data[2] = 0xae;
	p_can_message_1->data[3] = 0xff;
	p_can_message_1->data[4] = 0;
	p_can_message_1->data[5] = 0;
	p_can_message_1->data[6] = 0;
	p_can_message_1->data[7] = 0;
	p_can_message_1->dlc = 8;
	p_can_message_1->timestamp = 100;
		
	bool send_status = false;
	uint8_t  counter = 1;
	while (true)
	{
		p_can_message_1->timestamp = 255;
		if (counter == 10)
		{
			counter = 0;
			p_can_message_1->id = 0xbf3c;
		}
		send_status = can_interface.SendMessage(p_can_message_1);
		if(send_status == 1)	
		{
			LOG(logger, (char*) "Sent!!!")
		} 
		else {
			LOG(logger, (char*) "Error occured.")
		}
		_delay_ms(1000);
		p_can_message_1->id++;
		counter++;
	}
}

