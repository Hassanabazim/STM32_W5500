/*
 * MGPIO_Lconfig.c
 *
 *  Created on: Jan 21, 2024
 *      Author: Hassa
 */


#include "MGPIO_config.h"

MGPIO_CFG_t GPIO_PORT[MGPIO_PINS_NUM] =
								   {{PORTA, PIN0,  OUT_2MHZ_PUSH_PULL },
									{PORTA, PIN1,  IN_FLOATING },
									{PORTA, PIN2,  OUT_2MHZ_AF_PUSH_PULL },
									{PORTA, PIN3,  IN_FLOATING },
									{PORTA, PIN4,  IN_FLOATING },
									{PORTA, PIN5,  OUT_2MHZ_AF_PUSH_PULL },
									{PORTA, PIN6,  IN_FLOATING },
									{PORTA, PIN7,  OUT_2MHZ_AF_PUSH_PULL },
									{PORTA, PIN8,  IN_FLOATING },
									{PORTA, PIN9,  OUT_2MHZ_AF_PUSH_PULL},
									{PORTA, PIN10, IN_FLOATING },
									{PORTA, PIN11, IN_FLOATING },
									{PORTA, PIN12, IN_FLOATING },
									{PORTA, PIN13, OUT_2MHZ_AF_PUSH_PULL },
									{PORTA, PIN14, OUT_2MHZ_AF_PUSH_PULL },
									{PORTA, PIN15, IN_FLOATING },
									{PORTB, PIN0,  OUT_2MHZ_PUSH_PULL },
									{PORTB, PIN1,  OUT_2MHZ_PUSH_PULL },
									{PORTB, PIN2,  IN_FLOATING },
							        {PORTB, PIN3,  IN_FLOATING },
									{PORTB, PIN4,  IN_FLOATING },
									{PORTB, PIN5,  IN_FLOATING },
									{PORTB, PIN6,  IN_FLOATING },
									{PORTB, PIN7,  IN_FLOATING },
									{PORTB, PIN8,  IN_FLOATING },
									{PORTB, PIN9,  IN_FLOATING },
									{PORTB, PIN10, OUT_2MHZ_AF_PUSH_PULL },
									{PORTB, PIN11, IN_FLOATING },
									{PORTB, PIN12, OUT_2MHZ_PUSH_PULL },
									{PORTB, PIN13, OUT_2MHZ_PUSH_PULL },
									{PORTB, PIN14, OUT_2MHZ_PUSH_PULL },
									{PORTB, PIN15, OUT_2MHZ_PUSH_PULL },
									{PORTC, PIN13, IN_FLOATING },
									{PORTC, PIN14, OUT_2MHZ_PUSH_PULL },
									{PORTC, PIN15, IN_FLOATING }};
