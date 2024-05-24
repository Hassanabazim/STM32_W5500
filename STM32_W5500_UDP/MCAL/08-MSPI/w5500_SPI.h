/*
 * w5500_SPI.h
 *
 *  Created on: Oct 7, 2023
 *      Author: Khaled Saleh
 */

#ifndef SRC_W5500_W5500_SPI_H_
#define SRC_W5500_W5500_SPI_H_

#include "stdio.h"
#include "wizchip_conf.h"

/********************************************************************************
 * MOST USED REGISTER PINS
 ********************************************************************************/

//SPI_CR1 Register Pins

#define CPHA		0		//Clock phase
#define CPOL		1		//Clock polarity
#define MSTR		2		//Master selection
#define SPE			6		//SPI enable
#define FIRST_LSB	7		//Frame format
#define SSI			8		//Internal slave select
#define SSM			9		//Software slave management
#define RX_ONLY		10		//Receive only
#define DFF			11		//Data frame format
#define BIDI_OE		14		//Output enable in bidirectional mode
#define BIDI_MODE	15		//Bidirectional data mode enable


//SPI_CR2 Register Pins
#define RXDMAE		0		//Rx buffer DMA enable
#define TXDMAEN		1		//Tx buffer DMA enable
#define SSOE		2		//SS output enable


//SPI_SR Register Pins
#define RXN			0		//Receive buffer not empty
#define TXE			1		//Transmit buffer empty

/********************************************************************************
 * PRIVATE MACROS CONFIGURATION
 ********************************************************************************/
#define MSPI_PRESCALER_DIV_2		0
#define MSPI_PRESCALER_DIV_4		1
#define MSPI_PRESCALER_DIV_8		2
#define MSPI_PRESCALER_DIV_16		3
#define MSPI_PRESCALER_DIV_32		4
#define MSPI_PRESCALER_DIV_64		5
#define MSPI_PRESCALER_DIV_128		6
#define MSPI_PRESCALER_DIV_256		7

/********************************************************************************
 *  PRIVATE MACROS MASKS
 ********************************************************************************/
#define MSPI_PRESCALER_MASK	0xFFC7


/********************************************************************************
 *  PRIVATE OFFSET MACROS
 ********************************************************************************/
#define MSPI_PRESCALER_OFFSET		3



#define W5500_NRST_GPIO_Pin 		PIN14
#define W5500_CS_GPIO_Pin 			PIN0
#define W5500_CS_GPIO_Port 			PORTA
#define W5500_NRST_GPIO_Port 		PORTC


void MSPI_enMasterInit(void);

void W5500Init();
void  wizchip_select(void);
void  wizchip_deselect(void);

u8 wizchip_read();
void  wizchip_write(u8 wb);

void wizchip_readburst(u8* pBuf, u16 len);
void  wizchip_writeburst(u8* pBuf, u16 len);


#endif
