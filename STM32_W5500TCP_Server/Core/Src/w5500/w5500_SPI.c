/*
 * w5500_SPI.c
 *
 *  Created on: Oct 7, 2023
 *      Author: Khaled Saleh
 */

#include "w5500_SPI.h"
#include "wizchip_conf.h"
#include "stdio.h"


extern SPI_HandleTypeDef hspi1;


uint8_t SPIReadWrite(uint8_t data)
{
	//wait till FIFO has a free slot
	while((hspi1.Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE);

	*(__IO uint8_t*)&hspi1.Instance->DR=data;

	//Now wait till data arrives
	while((hspi1.Instance->SR & SPI_FLAG_RXNE)!=SPI_FLAG_RXNE);

	return (*(__IO uint8_t*)&hspi1.Instance->DR);
}



void wizchip_select(void)
{
HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_GPIO_Pin, GPIO_PIN_RESET); //CS LOW
}

void wizchip_deselect(void)
{
HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_GPIO_Pin, GPIO_PIN_SET); //CS HIGH
}



uint8_t wizchip_read()
{
	uint8_t rb;
	rb=SPIReadWrite(0x00);
	return rb;
}

void  wizchip_write(uint8_t wb)
{
	SPIReadWrite(wb);
}


void wizchip_readburst(uint8_t* pBuf, uint16_t len)
{
	for(uint16_t i=0;i<len;i++)
	{
		*pBuf=SPIReadWrite(0x00);
		pBuf++;
	}
}

void  wizchip_writeburst(uint8_t* pBuf, uint16_t len)
{
	for(uint16_t i=0;i<len;i++)
	{
		SPIReadWrite(*pBuf);
		pBuf++;
	}
}


void W5500Init()
{
	uint8_t tmp;
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };

	HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_GPIO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(W5500_NRST_GPIO_Port, W5500_NRST_GPIO_Pin, GPIO_PIN_RESET);
	tmp = 0xFF;
	while(tmp--);
	HAL_GPIO_WritePin(W5500_NRST_GPIO_Port,W5500_NRST_GPIO_Pin, GPIO_PIN_SET);

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);


	/* wizchip initialize*/
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		printf("WIZCHIP Initialized Failed.\r\n");
		while (1);
	}
	printf("WIZCHIP Initialization Success.\r\n");
}




