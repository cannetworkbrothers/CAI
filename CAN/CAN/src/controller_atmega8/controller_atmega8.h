/*
 * MCU_atmega8.h
 *
 * Created: 18.11.2015 12:30:26
 *  Author: Admin
 */ 


#include "../controller/can_controller.h"

class ControllerAtmega8 : public CanController
{	
	public:
	
	ControllerAtmega8() {};
	~ControllerAtmega8() {};
	
	unsigned char SPI_MasterTransmit(char cData);	
};