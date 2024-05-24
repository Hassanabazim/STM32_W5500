/*
 * w5500_SPI.c
 *
 *  Created on: Oct 7, 2023
 *      Author: Khaled Saleh
 */

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "REGISTERS.h"
#include "MGPIO_int.h"
#include "w5500_SPI.h"
#include "wizchip_conf.h"
#include "MUSART_int.h"
void MSPI_enMasterInit(void)
{
	/* 1. Set up the pre_configured Prescaler */
	SPI1->SPI_CR1 &= MSPI_PRESCALER_MASK;
	SPI1->SPI_CR1 |= MSPI_PRESCALER_DIV_256 << MSPI_PRESCALER_OFFSET;

	//SPI1->SPI_CR1 = 0;
	/*2. set up the pre_configured CLK Polarity */
	CLR_BIT(SPI1->SPI_CR1, CPOL);

	/*3. set up the pre_configured CLK Phase */
	CLR_BIT(SPI1->SPI_CR1, CPHA);


	/*4. set up the pre_configured Data Size  */
	CLR_BIT(SPI1->SPI_CR1, DFF);

	/*5. set up the pre_configured Data Order  */
	CLR_BIT(SPI1->SPI_CR1, FIRST_LSB);


	/*6. set up the pre_configured Data Full Duplex */
	CLR_BIT(SPI1->SPI_CR1, BIDI_MODE);
	CLR_BIT(SPI1->SPI_CR1, BIDI_OE);

	/*7. set up the Slave select Management  */
	SET_BIT(SPI1->SPI_CR1, SSM);
	SET_BIT(SPI1->SPI_CR1, 8);

	/*9. Enable the SPI Peripheral  */
	SET_BIT(SPI1->SPI_CR1, SPE);
	/*8. set up  the operation Mode Master  */
	SET_BIT(SPI1->SPI_CR1, MSTR);
}


u8 SPIReadWrite(u8 data)
{
	//wait till FIFO has a free slot
	while(!(GET_BIT(SPI1->SPI_SR, TXE)));

	SPI1->SPI_DR = data;

	//Now wait till data arrives
	while(!(GET_BIT(SPI1->SPI_SR, RXN)));

	return SPI1->SPI_DR;
}



void wizchip_select(void)
{
	MGPIO_enSetResetPin(W5500_CS_GPIO_Port, W5500_CS_GPIO_Pin, GPIO_PIN_RESET); //CS LOW
}

void wizchip_deselect(void)
{
	MGPIO_enSetResetPin(W5500_CS_GPIO_Port, W5500_CS_GPIO_Pin, GPIO_PIN_SET); //CS HIGH
}



u8 wizchip_read()
{
	u8 rb;
	rb=SPIReadWrite(0x00);
	return rb;
}

void  wizchip_write(u8 wb)
{
	SPIReadWrite(wb);
}


void wizchip_readburst(u8* pBuf, u16 len)
{
	for(u16 i=0;i<len;i++)
	{
		*pBuf=SPIReadWrite(0x00);
		pBuf++;
	}
}

void  wizchip_writeburst(u8* pBuf, u16 len)
{
	for(u16 i=0;i<len;i++)
	{
		SPIReadWrite(*pBuf);
		pBuf++;
	}
}


void W5500Init()
{
	u8 tmp;
	u8 memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };

	MGPIO_enSetResetPin(W5500_CS_GPIO_Port, W5500_CS_GPIO_Pin, GPIO_PIN_SET);
	MGPIO_enSetResetPin(W5500_NRST_GPIO_Port, W5500_NRST_GPIO_Pin, GPIO_PIN_RESET);
	tmp = 0xFF;
	while(tmp--);
	MGPIO_enSetResetPin(W5500_NRST_GPIO_Port,W5500_NRST_GPIO_Pin, GPIO_PIN_SET);

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);


	/* wizchip initialize*/
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		MUSART_enBusySendString(MUSART2, (u8*)"WIZCHIP Initialized Failed.\r\n");
		while (1);
	}
	MUSART_enBusySendString(MUSART2, (u8*)"WIZCHIP Initialization Success.\r\n");
}




