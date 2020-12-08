/*
 * auth_tasks.c
 *
 *  Created on: 1 dic. 2020
 *      Author: Miguel Pérez García
 */

#define THUMBPRINT { 12, 203, 106, 22, 62, 220, 133, 35, 224, 39, 163, 211, 215, 7, 241, 146, 48, 15, 44, 158 }

#include <stddef.h>
#include "../../../utilities/fsl_debug_console.h"
#include "FreeRTOS.h"
#include "auth_tasks.h"
#include "event_groups.h"

extern QueueHandle_t send_queue;
extern QueueHandle_t receive_queue;
extern EventGroupHandle_t servo_events;
extern EventGroupHandle_t auth_events;

BaseType_t send_message(MessageType type){
	RequestMessage message = {
		.type = type,
		.thumbprint = THUMBPRINT
	};
	return xQueueSendToBack(send_queue, &message, portTICK_PERIOD_MS * 100);
}

BoolType wait_response() {
	ResponseMessage *message = (ResponseMessage *)malloc(sizeof(ResponseMessage));
	BaseType_t result = xQueueReceive(receive_queue, message, portTICK_PERIOD_MS * 10000);
	if(pdFALSE == result || RESPONSE != message->type) return FALSE;
	else return message->result;
}

BoolType make_request(MessageType type){
	BaseType_t send_result = send_message(type);
	if(pdTRUE == send_result) PRINTF("Message Sent\n");
	else PRINTF("Failed to send message\n");

	return wait_response();
}

void auth_task(void* args)
{
	uint32_t auth_event_bits = AUTH_AUTHENTICATE | AUTH_REGISTER;

	while(1){
		EventBits_t auth_bits = xEventGroupWaitBits(auth_events, auth_event_bits, pdTRUE, pdFALSE, portMAX_DELAY);
		if(AUTH_AUTHENTICATE == (auth_bits & AUTH_AUTHENTICATE)){
			PRINTF("Auth requested\n");
			BoolType result = make_request(AUTH);
			if(TRUE == result){
				PRINTF("Opening lock\n");
				xEventGroupSetBits(servo_events, SERVO_OPEN_BIT);
			} else {
				PRINTF("Faild to authenticate thumbprint\n");
			}
		} else if(AUTH_REGISTER == (auth_bits & AUTH_REGISTER)){
			PRINTF("Register requested\n");
			BoolType result = make_request(REGISTER);
			if(TRUE == result) PRINTF("Registered successfully\n");
			else PRINTF("Failed to register thumbprint\n");
		}
	}
}
