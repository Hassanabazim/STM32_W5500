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
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"



#define W5500_NRST_GPIO_Pin 		GPIO_PIN_14
#define W5500_CS_GPIO_Pin 			GPIO_PIN_0
#define W5500_CS_GPIO_Port 			GPIOA
#define W5500_NRST_GPIO_Port 		GPIOC


void W5500Init();

void  wizchip_select(void);
void  wizchip_deselect(void);

uint8_t wizchip_read();
void  wizchip_write(uint8_t wb);

void wizchip_readburst(uint8_t* pBuf, uint16_t len);
void  wizchip_writeburst(uint8_t* pBuf, uint16_t len);


#endif
