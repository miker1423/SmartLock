/*
 * auth_tasks.c
 *
 *  Created on: 1 dic. 2020
 *      Author: Miguel Pérez García
 */


#include "FreeRTOS.h"
#include "auth_tasks.h"
#include "event_groups.h"


extern QueueHandle_t send_queue;
extern QueueHandle_t receive_queue;
extern EventGroupHandle_t servo_events;
extern EventGroupHandle_t auth_events;

void auth_task(void* args)
{
	uint32_t auth_event_bits = AUTH_AUTHENTICATE | AUTH_REGISTER;

	while(1){
		EventBits_t auth_bits = xEventGroupWaitBits(auth_events, auth_event_bits, pdTRUE, pdFALSE, portMAX_DELAY);
		if(AUTH_AUTHENTICATE == (auth_bits & AUTH_AUTHENTICATE)){
			PRINTF("Auth requested");
		} else if(AUTH_REGISTER == (auth_bits & AUTH_REGISTER)){
			PRINTF("Register requested");
		}
	}
}
