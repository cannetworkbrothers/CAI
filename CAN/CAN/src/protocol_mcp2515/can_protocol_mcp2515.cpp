/*
 * CAN_PROTOCOL_MCP2515.cpp
 *
 * Created: 18.11.2015 13:05:41
 *  Author: Brothers
 */ 


#include "can_protocol_mcp2515.h"
#include "math.h"
#ifndef MAIN_H_
	#include "../../main.hpp"
#endif
#include <util/delay.h>

// macro for micro controller callback, will be used in appropriate MCP2515 functions 
#define SPI_TRANSMIT(adr) ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, adr);



void ProtocolHandlerMcp2515::Reset(void)
{
	//to start SPI transfer need to push CS low level (chip select or slave select)
	setPin(MCP2515_CS, false);
	//send 8bit via SPI
	SPI_TRANSMIT(RESET);
	//to end SPI transfer need to push CS to high level (chip select or slave select)
	setPin(MCP2515_CS, false);
	_delay_ms(10);
	
}

unsigned char ProtocolHandlerMcp2515::init(const unsigned char canSpeed)
{
	unsigned char result = 0;
	
	Reset();
	//Set CS pin to low level
	setPin(MCP2515_CS, false);
	
	// initialization only configure mode
	
	if (GET_MODE != MODE_CONFIG) // this configure Mode?
			SET_MODE(MODE_CONFIG);
		
			
	// set all interrupt Flags 
	
	writeRegister(CANINTE, 0xFF);
	
	// configure filter
	
	writeRegister(RXB0CTRL, 0x00); // use filter for standard and extended frames
	writeRegister(RXB1CTRL, 0x00); // use filter for standard and extended frames

	// initialize filter mask
	writeRegister(RXM0SIDH, 0x00);
	writeRegister(RXM0SIDL, 0x00);
	writeRegister(RXM0EID8, 0x00);
	writeRegister(RXM0EID0, 0x00);
	writeRegister(RXM1SIDH, 0x00);
	writeRegister(RXM1SIDL, 0x00);
	writeRegister(RXM1EID8, 0x00);
	writeRegister(RXM1EID0, 0x00);
	
	// set pin assigment RX0BF and RX1BF
	
	writeRegister(BFPCTRL, 0x0F);    // RXnBF pin contrjl and status reg all 1 pins setting interrupt ������ �� ��� ����� 0, ����� ������ ����� � ��������������� ������.
	
	mcp2515_set_bittiming(MCP2515_TIMINGS_125K);
	
	return result;
}

/**
 * \brief Set bit timing registers
 *
 * \param cnf1 Configuration register 1
 * \param cnf2 Configuration register 2
 * \param cnf3 Configuration register 3
 *
 * This function has only affect if mcp2515 is in configuration mode
 */
void ProtocolHandlerMcp2515::mcp2515_set_bittiming(unsigned char cnf1, unsigned char cnf2, unsigned char cnf3) {


    writeRegister(MCP2515_REG_CNF1, cnf1);
    writeRegister(MCP2515_REG_CNF2, cnf2);
    writeRegister(MCP2515_REG_CNF3, cnf3);
}

bool ProtocolHandlerMcp2515::getPin(PIN pin)
{
	return true;
}

void ProtocolHandlerMcp2515::setPin(PIN pin, bool level)
{
	if (level == true)
	{
		pin = 0;
	}
	else
	{
		pin = 1;
	}
	
}

unsigned char ProtocolHandlerMcp2515::ReadRegister(unsigned char address){
	unsigned char data;
	
	//Set CS pin to low level
	setPin(MCP2515_CS, false);
	
	SPI_TRANSMIT(READ);
	SPI_TRANSMIT(address);
	
	data = SPI_TRANSMIT(0xFF);
	
	setPin(MCP2515_CS, true);
	
	return data;
}

void ProtocolHandlerMcp2515::writeRegister(unsigned char address, unsigned char data)
{
	  // set CS pin to low lewel
		setPin(MCP2515_CS, false);
		
		ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, WRITE);
		ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p,address);
		ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, data);
		
		
		// release SS
		setPin(MCP2515_CS, true);
	
	//just a stub
}

void ProtocolHandlerMcp2515::mcp2515_bit_modify(unsigned char address, unsigned char mask, unsigned char data) {

	// set CS pin to low lewel
	setPin(MCP2515_CS, false);
	
	
	ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, MCP2515_CMD_BIT_MODIFY);
	ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, address);
	ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, mask);
	ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, data);
	
	// release SS
	setPin(MCP2515_CS, true);
}

unsigned char ProtocolHandlerMcp2515::mcp2515_read_status() {

	// set CS pin to low lewel
	setPin(MCP2515_CS, false);
	
	ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, MCP2515_CMD_READ_STATUS );
	unsigned char status = ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff);
	
	// release SS
	setPin(MCP2515_CS, true);

	return status;
}

unsigned char ProtocolHandlerMcp2515::mcp2515_rx_status(){
	
	// function implementation command rx status in SPI interface. This command return whether message any buffers and wich format can message
	
	// set CS pin to low lewel
	setPin(MCP2515_CS, false);
	
	
	ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, MCP2515_CMD_RX_STATUS);
	unsigned char status = ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff);
	
	// release SS
	setPin(MCP2515_CS, true);
	
	return status;
	
	
}

bool ProtocolHandlerMcp2515::receiveMessage(canmsg_t * p_canmsg){
		
	unsigned char status = mcp2515_rx_status();
	unsigned char Mask_address_rx_buffer;
	
	if(status == Message_in_RX0) Mask_address_rx_buffer = Buffer_RX0;
	else if(status == Message_in_RX1) Mask_address_rx_buffer = Buffer_RX1;
	else return 0;
	
	// store flags
	p_canmsg->flags.rtr = (status >> 3) & 0x01;
	p_canmsg->flags.extended = (status >> 4) & 0x01;

	
	setPin(MCP2515_CS, false);
	
	ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, MCP2515_CMD_READ_RX | Mask_address_rx_buffer);
	
	if(p_canmsg->flags.extended){
		p_canmsg->id = (unsigned long) ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff) << 21;
		 unsigned long temp = ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff);
		 p_canmsg->id |= (temp & 0xe0) << 13;
		 p_canmsg->id |= (temp & 0x03) << 16;
		 p_canmsg->id |= (unsigned long) ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff) << 8;
		 p_canmsg->id |= (unsigned long)  ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff);
		 } else {
		 p_canmsg->id =  (unsigned long)  ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff) << 3;
		 p_canmsg->id |= (unsigned long)  ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff) >> 5;
		  ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff);
		  ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff);
	 }
	 
	 // get length and data
	
	 p_canmsg->dlc = ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff) & 0x0f;
	 if (!p_canmsg->flags.rtr) {
		 unsigned char i;
		 unsigned char length = p_canmsg->dlc;
		 if (length > 8) length = 8;
		 for (i = 0; i < length; i++) {
			 p_canmsg->data[i] = ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0xff);
		 }
	 }
     // reset flag rx buffers register when empty
	 
	 if (Mask_address_rx_buffer == 0) Mask_address_rx_buffer = 1;
	 else Mask_address_rx_buffer = 2;
	 mcp2515_bit_modify(MCP2515_REG_CANINTF, Mask_address_rx_buffer, 0);
	 
	 
 // release SS
 // set CS pin to low lewel
 setPin(MCP2515_CS, false);
	 return 1;
	 }

	
	

unsigned char ProtocolHandlerMcp2515::sendMessage(canmsg_t * p_canmsg) {

 unsigned char status = mcp2515_read_status();
 unsigned char address_load_buffer;
 unsigned char length;

 
 // check length
 length = p_canmsg->dlc;
 if (length > 8) length = 8;
 


 
 
  // get offest address of next free tx buffer  TXREQ - Message Transmit Request bit 
	//Buffer is currently pending transmission or Buffer is not currently pending transmission
		
  if ((status & TXB0CTRL_TXREQ) == 0) {                  ;
	  address_load_buffer = LOADBUF0_TX_SH;
	  } else if ((status & TXB1CTRL_TXREQ) == 0) {
	  address_load_buffer =LOADBUF1_TX_SH;
	  } else if ((status & TXB2CTRL_TXREQ) == 0) {
	  address_load_buffer = LOADBUF2_TX_SH;
	  } else {
	  // no free transmit buffer
	  return 0;
  }
  
   
  
  // set CS pin to low level..
   setPin(MCP2515_CS, false);
  
  ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, address_load_buffer);

   if (p_canmsg->flags.extended) {
	   // All id divided into  
	   //               hight standard id    low standard id(ext id)   hight ext id    low ext id
	   //                  76543210         43210                     76543210        76543210  
	   //                     >> 21         calculation 432xxx10         >>8             direct transive
	   // transmit low standard id
	   ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, p_canmsg->id >> 21);     // load standard id hight 29-21 =8 hight bit
	   // transmit hight standard id which the must include bit4=0, bit2=0, bit3=1(massage id extended)
	   
	   ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, ((p_canmsg->id >> 13) & 0xe0) | ((p_canmsg->id >> 16) & 0x03) | 0x08);
	   // transmit hight extended id
	   ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, p_canmsg->id >> 8);
	   // transmit low extended id
	   ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, p_canmsg->id);
	   } else {
	    ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, p_canmsg->id >> 3);   // load standard id hight  11-3=8 bit
	    ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, p_canmsg->id << 5);   // load standard id low  3=8-5 bit
	    ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0);                   // load extended id hight
	    ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, 0);                   // load extended id low
   }
   
   // length and data
   if (p_canmsg->flags.rtr) {				// RTR Remote Transmission Request (Frame request - recessive  Frame data -dominate)
	    ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, p_canmsg->dlc | CAN_TXBxDLC_RTR);  // RTR fisical locate in TXBnDLC register in 6 bit. (this is a 0x40)
											// if 0 - data if  1- remoute  transmit request/ if true then setiings 1 in 6 bit 
	   } else {
	   ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, p_canmsg->dlc);
	   unsigned char i;
	   for (i = 0; i < length; i++) {
		   ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, p_canmsg->data[i]);
	   }
   }
   
   
   // release SS
   setPin(MCP2515_CS, true);
   
   //_delay(1); ????????????????????????????????????????????????
   
   
    // Send massage in CAN in select RTS bit Request to Send
	
	// When Send via SPI the flags
	//     ABTF -Message Aborted Flag bit       1- Massage was aborted
	//    MLOA -Message Lost Arbitration        1-Message lost arbitration while being sent
	//     TXERR -Transmission Error Detected   1 = A bus error occurred while the message was being transmitted
	//    will be cleared automatically
	
   // set CS pin to low lewel..
   setPin(MCP2515_CS, false);
   
   if (address_load_buffer == LOADBUF0_TX_SH) address_load_buffer = RTS_TXB0;
   ProtocolHandler::controller_spi_transmit_(ProtocolHandler::controller_p, address_load_buffer);
   
   // release SS
   setPin(MCP2515_CS, true);
   // check if interrupt
   unsigned char INTERRUPT_FLAGS = ReadRegister(CANINTF);
   
   // check empty tx buffers
   
   while((INTERRUPT_FLAGS && CAN_TX0IF_BIT == 0) | (INTERRUPT_FLAGS && CAN_TX1IF_BIT == 0 )| (INTERRUPT_FLAGS && CAN_TX2IF_BIT == 0)) {
	   
	  //  check error massage tx
	  INTERRUPT_FLAGS = ReadRegister(CANINTF);
	   if(INTERRUPT_FLAGS && CAN_MERRF_BIT ==1)  {
		   mcp2515_bit_modify(CANINTF,CAN_MERRF_BIT,0 );
		   return 0;
		   
	   }
	   
	  }
	  // again check error massage
	   if(INTERRUPT_FLAGS && CAN_MERRF_BIT ==1)  {
		   mcp2515_bit_modify(CANINTF,CAN_MERRF_BIT,0 );
		   return 0;
	   }
	  // set flag interrupt when buffer is empty
	  INTERRUPT_FLAGS = ReadRegister(CANINTF);
	  if(INTERRUPT_FLAGS && CAN_TX0IF_BIT == 1) mcp2515_bit_modify(CANINTF, CAN_TX0IF_BIT, 0);
	  if(INTERRUPT_FLAGS && CAN_TX1IF_BIT == 1) mcp2515_bit_modify(CANINTF, CAN_TX1IF_BIT, 0);
	  if(INTERRUPT_FLAGS && CAN_TX2IF_BIT == 1) mcp2515_bit_modify(CANINTF, CAN_TX2IF_BIT, 0);
   
   return 1;
   
   
   
}

bool ProtocolHandlerMcp2515::writeMessage(canmsg_t * p_canmsg)
{
	//just a stub
	return true;
}