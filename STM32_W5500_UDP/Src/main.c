/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         @Author	:  Hassan Abdelazim Abdelsalam
 *         @File	:  main.c
 *         @Module	:  Application
 *
 *  Description:  This file provide Application APIs code Implementation
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "string.h"
#include "STD_TYPES.h"
#include "ERROR_STATE.h"
#include "BIT_MATH.h"
#include "REGISTERS.h"
#include "MRCC_int.h"
#include "MGPIO_int.h"
#include "MSYSTICK_int.h"
#include "MUSART_int.h"

// W5500 Module
#include "w5500.h"
#include "w5500_SPI.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "loopback.h"


/**********************************************************************************************************************
 *  PRIVIATE FUNCTIONS
 *********************************************************************************************************************/
static void PHYStatusCheck(void);
static void PrintPHYConf(void);
static void UDP_RecW5500Client(u8 *copy_u8Buffer);

/**********************************************************************************************************************
 *  GLOBAL MACROS
 *********************************************************************************************************************/
#define Front_TxBUFFER		12
#define UDP_SOCKET			1
#define UDP_RxBUFFER		690

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/
u8 strX[]={"\n\rHello From W5500 Client"};
u8 UDP_recBufer[UDP_RxBUFFER];
u8 Front_Txbuffer[Front_TxBUFFER];

//IP of server application
u8 dest_ip[]={192,168,1,4}; // my network which assigned as server
u16 dest_Port = 4000;		// assigned port

u16 packet_len;

// Network Configuration //
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
		.ip = {192, 168, 1,123},   // stm32 client by my choice
		.sn = {255, 255, 255, 0},
		.gw = {192, 168, 1, 1},
		.dns = {8, 8, 8, 8},
		.dhcp = NETINFO_STATIC };

/**********************************************************************************************************************
 * main function
 *********************************************************************************************************************/
int main(void)
{
	MUSART_INIT_t Debug = {MUSART_FULL_DUPLEX, MUSART_8BIT_DATA, MUSART_PARITY_DISABLE, MUSART_STOPBIT1, MUSART_BAUDRATE_9600_CLK_8M};

	/*	System initialization */
	MRCC_enSysClkINIT();
	MRCC_enPeripheralInit();
	MGPIO_enInit();
	MSYSTICK_enInit();

	/*	Debug serial for UART3 */
	MUSART_enInit(MUSART2, &Debug);

	/* Ethernet Module and Socket Initialization */
	MSPI_enMasterInit();
	W5500Init();
	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
	PHYStatusCheck();
	PrintPHYConf();

	if(socket(UDP_SOCKET, Sn_MR_UDP,dest_Port,0x00)==1)
	{
		MUSART_enBusySendString(MUSART2,(u8*) "\r\nSocket Created Successfully");
	}
	else
	{
		MUSART_enBusySendString(MUSART2,(u8*) "\r\nCannot create socket");
		while(1);
	}


	while (1)
	{
		/*	receive packets from UDP */
		UDP_RecW5500Client(UDP_recBufer);

		/*	send back the packet */
		MUSART_enBusySendString(MUSART2, UDP_recBufer);

		MUSART_enBusySendString(MUSART2,(u8*)"\r\n");

		for (u16 i = 0; i < UDP_RxBUFFER; i++)
		{
			UDP_recBufer[i] = 0;
		}

		MSYSTICK_enDelayMS(500);


	}
}





static void UDP_RecW5500Client(u8 *copy_u8Buffer)
{
	if(sendto(UDP_SOCKET,strX,sizeof(strX),dest_ip,dest_Port)<=SOCK_ERROR)

	{
		MUSART_enBusySendString(MUSART2,(u8*)"\r\nSending Failed!");
		while(1);
	}
	else
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\r\nSending Success!");
	}

	switch(getSn_SR(UDP_SOCKET))
	{
	case SOCK_UDP:
	{
		MUSART_enBusySendString(MUSART2,(u8*)"SOCK_UDP_SEND\n");
		if(getSn_IR(UDP_SOCKET) & Sn_IR_RECV)
		{
			setSn_IR(UDP_SOCKET, Sn_IR_RECV);
			MUSART_enBusySendString(MUSART2,(u8*)"SOCK_UDP_RECEIVE");
		}
		if((packet_len=getSn_RX_RSR(UDP_SOCKET))>0)
		{
			packet_len = recvfrom(UDP_SOCKET,UDP_recBufer, packet_len, dest_ip,&dest_Port);
			UDP_recBufer[packet_len] = '\0';  // Null-terminate the received string
			MGPIO_enSetPinValue(PORTA, PIN0, HIGH); // LED INDICATOR
			MSYSTICK_enDelayMS(500);
			MGPIO_enSetPinValue(PORTA, PIN0, LOW);



		}
	}
	}
	//Get_Nums_From_String(); INCASE YOU WANT EXTRACT NUMBR  DATA FROM THE SERVER
	MSYSTICK_enDelayMS(500);
}




void PHYStatusCheck(void)
{
	u8 tmp;
	do
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\r\nChecking Ethernet Cable Presence ...");
		ctlwizchip(CW_GET_PHYLINK, (void*) &tmp);

		if(tmp == PHY_LINK_OFF)
		{
			MUSART_enBusySendString(MUSART2,(u8*)"NO Cable Connected!");
			MSYSTICK_enDelayMS(1000);
		}
	}while(tmp == PHY_LINK_OFF);
	MUSART_enBusySendString(MUSART2,(u8*)"Good! Cable got connected!");
}

void PrintPHYConf(void)
{
	wiz_PhyConf phyconf;

	ctlwizchip(CW_GET_PHYCONF, (void*) &phyconf);

	if(phyconf.by==PHY_CONFBY_HW)
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\n\rPHY Configured by Hardware Pins");
	}
	else
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\n\rPHY Configured by Registers");
	}

	if(phyconf.mode==PHY_MODE_AUTONEGO)
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\n\rAutonegotiation Enabled");
	}
	else
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\n\rAutonegotiation NOT Enabled");
	}

	if(phyconf.duplex==PHY_DUPLEX_FULL)
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\n\rDuplex Mode: Full");
	}
	else
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\n\rDuplex Mode: Half");
	}

	if(phyconf.speed==PHY_SPEED_10)
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\n\rSpeed: 10Mbps");
	}
	else
	{
		MUSART_enBusySendString(MUSART2,(u8*)"\n\rSpeed: 100Mbps");
	}
}

