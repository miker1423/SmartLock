/*
 * mqtt_tasks.c
 *
 *  Created on: 1 dic. 2020
 *      Author: Miguel Pérez García
 */


#include <stddef.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "mqtt.h"
#include "mqtt_tasks.h"
#include "../../models/messages.h"
#include "../../models/csv_parser.h";

extern QueueHandle_t send_queue;
extern QueueHandle_t receive_queue;
extern SemaphoreHandle_t mqtt_mutex;
extern mqtt_client_t *mqtt_client;
extern char *device_topic;

static void mqtt_message_published_cb(void *arg, err_t err)
{
    const char *topic = (const char *)arg;

    if (err == ERR_OK) PRINTF("Published to the topic \"%s\".\r\n", topic);
    else PRINTF("Failed to publish to the topic \"%s\": %d.\r\n", topic, err);
}

void mqtt_send_task(void *args) {
	if(NULL == mqtt_mutex) {
		PRINTF("No mutex configured");
	}

	while(1){
		if(NULL == receive_queue){
			PRINTF("No queue configured");
		}

		RequestMessage *message = (RequestMessage *)malloc(sizeof(RequestMessage));
		BaseType_t result = xQueueReceive(send_queue, message, portMAX_DELAY);
		if(pdFALSE == result) {
			PRINTF("OH Crap");
			vTaskDelete(NULL);
			return;
		}

		char *buffer = ToCsv(message);
		mqtt_publish(mqtt_client, device_topic, buffer, strlen(buffer), 1, 0, mqtt_message_published_cb, (void *)device_topic);

		if(NULL == mqtt_mutex) {
			PRINTF("No mutex configured");
		}
		if(xSemaphoreTake(mqtt_mutex, portMAX_DELAY) == pdFALSE){
			PRINTF("Failed to acquire lock, ups");
		}

		PRINTF("Sending message");
	}
}


void mqtt_receive_task(void *args) {
	vTaskSuspend(NULL);
}

void init_mqtt_tasks(void *args) {
	BaseType_t result;
	TaskHandle_t xSendHandle;
	result = xTaskCreate(mqtt_send_task, "send_task", configMINIMAL_STACK_SIZE + 50, NULL, tskIDLE_PRIORITY, &xSendHandle);
	if(pdPASS != result){
		PRINTF("Failed to create send task");
	}

	TaskHandle_t xReceiveHandle;
	result = xTaskCreate(mqtt_receive_task, "receive_task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xReceiveHandle);
	if(pdPASS != result){
		PRINTF("Failed to create receive task");
	}

	vTaskDelete(NULL);
}
