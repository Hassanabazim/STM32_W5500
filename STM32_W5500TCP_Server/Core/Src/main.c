#include <stdio.h>
#include <string.h>
#include "main.h"

//w5500 related
#include "w5500_spi.h"
#include "wizchip_conf.h"
#include "socket.h"

//The TCP Port on which we will listen
#define LISTEN_PORT 		5000
#define RECEIVE_BUFF_SIZE 	128

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

wiz_NetInfo gWIZNETINFO = {
		.mac = { 0x80, 0x34, 0x28, 0x74, 0xA5, 0xCB },//MSB - LSB
		.ip ={ 192, 168, 1, 110 },
		.sn = { 255, 255, 255, 0 },
		.gw ={ 192, 168, 1, 1 },
		.dns = { 8, 8, 8, 8 },
		.dhcp = NETINFO_STATIC };

uint8_t receive_buff[RECEIVE_BUFF_SIZE];

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void UWriteData(const char data);
static void PHYStatusCheck(void);
static void PrintPHYConf(void);

int main(void)
{
	HAL_Init();

	setbuf(stdout, NULL);//disable printf() buffering, output immediately to port.

	SystemClock_Config();

	MX_GPIO_Init();
	MX_SPI1_Init();//for w5500 communications
	MX_USART2_UART_Init();//for printf() style debug

	printf("A Simple TCP Echo Server Application using W5500!\r\n");

	W5500Init();

	//Configure network parameters like IP address etc
	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

//	//Configure PHY by software for maximum compatibility
//	//so that user can use any model w5500 board
//	//else the PINs on the board configure it, which may
//	//lead to different configurations in different boards.
//	wiz_PhyConf phyconf;
//
//	phyconf.by=PHY_CONFBY_SW;
//	phyconf.duplex=PHY_DUPLEX_FULL;
//	phyconf.speed=PHY_SPEED_10;//10MBps Ethernet link speed
//	phyconf.mode=PHY_MODE_AUTONEGO;//best to go with auto-negotiation
//
//	ctlwizchip(CW_SET_PHYCONF, (void*) &phyconf);//PHY Configuration Command
//	//*** End Phy Configuration

	PHYStatusCheck();
	PrintPHYConf();

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//The above code are common to all projects
	//TCP Echo Server Code begins from here
	//We will listen on socket ID:1 and TCP Port Would be 5000

	printf("\r\n*****************SIMPLE TCP ECHO SERVER******************\r\n");


	while(1)
	{
		printf("\r\nInitializing server socket\r\n");

		//Parameters in order socket_id, protocol TCP or UDP, Port number, Flags=0
		//Return value is socket ID on success
		if(socket(1,Sn_MR_TCP,LISTEN_PORT,0)!=1)
		{
			//error
			printf("Cannot create Socket!\r\n");
			while(1);//halt here
		}

		//success
		printf("Socket Created Successfully ! \r\n");

		uint8_t socket_io_mode=SOCK_IO_BLOCK;

		ctlsocket(1, CS_SET_IOMODE , &socket_io_mode);//set blocking IO mode

		printf("Start listening on port %d ! \r\n",LISTEN_PORT);
		printf("Waiting for a client connection. \r\n");

		//Make it a passive socket (i.e. listen for connection)
		if(listen(1)!=SOCK_OK)//our socket id is 1 (w5500 have 8 sockets from 0-7)
		{
			//error
			printf("Cannot listen on port %d",LISTEN_PORT);
			while(1);
		}

		uint8_t sr=0x00;//socket status register

		do
		{
			sr=getSn_SR(1);//read status reg (SR of socket 1)
		}while (sr!=0x17 && sr!=0x00);

		if(sr==0x00)
		{
			printf("Some error occurred on server socket. Please restart.\r\n");
			while(1);
		}

		if(sr==0x17)
		{
			//we come here only when a client has connected.
			//Now we can read data from the socket
			printf("A client connected!\r\n");
			printf("Waiting for Client Data ...!\r\n");

			while(1)
			{
				int len=recv(1, receive_buff, RECEIVE_BUFF_SIZE);

				if(len==SOCKERR_SOCKSTATUS)
				{
					//client has disconnected
					printf("Client has disconnected\r\n");
					printf("*** SESSION OVER ***\r\n\r\n");
					break;
				}

				receive_buff[len]='\0';

				printf("Received %d bytes from client\r\n",len);
				printf("Data Received: %s", receive_buff);

				//Echo the data back encloused in a [] pair
				send(1,(uint8_t*)"[",1);//starting sq bracket
				send(1,receive_buff,len);// the data
				send(1,(uint8_t*)"]",1);//closing sq bracket

				printf("\r\nECHO sent back to client\r\n");

				//Look for quit message and quit if received
				if(strcmp((char*)receive_buff,"QUIT")==0)
				{
					printf("Received QUIT command from client\r\n");
					printf("Disconnecting ... \r\n");
					printf("*** SESSION OVER ***\r\n\r\n");
					disconnect(1);//disconnect from the clinet
					break;//come out of while loop
				}

			}//While loop (as long as client is connected)

		}//if block, client connect success
	}//while loop for next client wait
}

void UWriteData(const char data)
{
	while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TXE)==RESET);

	huart2.Instance->DR=data;

}

int __io_putchar(int ch)
{
	UWriteData(ch);
	return ch;
}

void PHYStatusCheck(void)
{
	uint8_t tmp;

	do
	{
		printf("\r\nChecking Ethernet Cable Presence ...");
		ctlwizchip(CW_GET_PHYLINK, (void*) &tmp);

		if(tmp == PHY_LINK_OFF)
		{
			printf("NO Cable Connected!");
			HAL_Delay(1500);
		}
	}while(tmp == PHY_LINK_OFF);

	printf("Good! Cable got connected!");

}

void PrintPHYConf(void)
{
	wiz_PhyConf phyconf;

	ctlwizchip(CW_GET_PHYCONF, (void*) &phyconf);

	if(phyconf.by==PHY_CONFBY_HW)
	{
		printf("\n\rPHY Configured by Hardware Pins");
	}
	else
	{
		printf("\n\rPHY Configured by Registers");
	}

	if(phyconf.mode==PHY_MODE_AUTONEGO)
	{
		printf("\n\rAutonegotiation Enabled");
	}
	else
	{
		printf("\n\rAutonegotiation NOT Enabled");
	}

	if(phyconf.duplex==PHY_DUPLEX_FULL)
	{
		printf("\n\rDuplex Mode: Full");
	}
	else
	{
		printf("\n\rDuplex Mode: Half");
	}

	if(phyconf.speed==PHY_SPEED_10)
	{
		printf("\n\rSpeed: 10Mbps");
	}
	else
	{
		printf("\n\rSpeed: 100Mbps");
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */
	__HAL_SPI_ENABLE(&hspi1);
	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

	/*Configure GPIO pins : PC14 PC15 */
	GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
