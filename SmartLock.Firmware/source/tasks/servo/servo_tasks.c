/*
 * servo_tasks.c
 *
 *  Created on: 1 dic. 2020
 *      Author: Miguel
 */


#include <stddef.h>
#include "../../models/messages.h"

#include "FreeRTOS.h"
#include "servo_tasks.h"
#include "event_groups.h"

extern EventGroupHandle_t servo_group;

void servo_action_task(void* args){
	if(NULL == servo_group) {
		PRINTF("No event group configured");
	}


	uint32_t wait_bits = SERVO_OPEN_BIT | SERVO_CLOSE_BIT;
	while(1){
		EventBits_t event_bits = xEventGroupWaitBits(servo_group, wait_bits, pdTRUE, pdFALSE, portMAX_DELAY);
		if(SERVO_OPEN_BIT == (event_bits & SERVO_OPEN_BIT)) {
			PRINTF("Open servo");
		} else if(SERVO_CLOSE_BIT == (event_bits & SERVO_CLOSE_BIT)) {
			PRINTF("Close servo");
		}
	}
}
