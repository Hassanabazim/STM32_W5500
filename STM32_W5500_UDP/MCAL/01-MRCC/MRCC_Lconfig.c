/*
 * MRCC_config.c
 *
 *  Created on: Jan 20, 2024
 *      Author: Hassa
 */


#include "MRCC_config.h"

MRCC_CFG RCC_PORT[MRCC_PREPHERIALS_NUM] =
 	 	 	 				{{MRCC_AFIO		,ENABLE } ,
							{MRCC_IOPA		,ENABLE } ,
							{MRCC_IOPB		,ENABLE } ,
							{MRCC_IOPC		,ENABLE } ,
							{MRCC_USART1 	,ENABLE},
 	 	 	 	 	 	 	{MRCC_USART2 	,ENABLE} ,
 							{MRCC_USART3 	,ENABLE} ,
							{MRCC_SPI1		,ENABLE } ,
							{MRCC_SPI2		,DISABLE } ,
							{MRCC_I2C1		,DISABLE } ,
							{MRCC_I2C2		,DISABLE } ,
							{MRCC_CAN		,DISABLE } ,
							{MRCC_USB		,DISABLE } ,
							{MRCC_TIM1		,DISABLE } ,
							{MRCC_TIM2		,DISABLE } ,
							{MRCC_TIM3 		,DISABLE } ,
							{MRCC_TIM4		,DISABLE } ,
							{MRCC_WWD		,DISABLE } ,
							{MRCC_ADC1		,DISABLE } ,
							{MRCC_ADC2		,DISABLE } ,
							{MRCC_DMA1		,DISABLE} ,
							{MRCC_SRAM		,DISABLE } ,
							{MRCC_FLITF		,DISABLE } ,
							{MRCC_CRC		,DISABLE } ,
 	 	 	 	 	 	 	{MRCC_FSMCEN 	,DISABLE} ,
							{MRCC_SDIOEN 	,DISABLE} ,
							{MRCC_BKPRST 	,DISABLE} ,
							{MRCC_PWRRST 	,DISABLE} ,
							{MRCC_DACRST 	,DISABLE}};
