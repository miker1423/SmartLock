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
#include "event_groups.h"
#include "mqtt.h"
#include "mqtt_tasks.h"
#include "../../models/messages.h"
#include "../../models/csv_parser.h"

#define MAX_TOPIC_SIZE 22
#define BASE_TOPIC_SIZE 20

extern QueueHandle_t send_queue;
extern QueueHandle_t receive_queue;
extern SemaphoreHandle_t mqtt_mutex;
extern mqtt_client_t *mqtt_client;
extern EventGroupHandle_t ready_events;
extern char *device_topic;

static void mqtt_topic_subscribed_cb(void *arg, err_t err)
{
    const char *topic = (const char *)arg;

    if (err == ERR_OK)
    {
        PRINTF("Subscribed to the topic \"%s\".\r\n", topic);
    }
    else
    {
        PRINTF("Failed to subscribe to the topic \"%s\": %d.\r\n", topic, err);
    }
}
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    int i;

    LWIP_UNUSED_ARG(arg);

    for (i = 0; i < len; i++)
    {
        if (isprint(data[i]))
        {
            PRINTF("%c", (char)data[i]);
        }
        else
        {
            PRINTF("\\x%02x", data[i]);
        }
    }

    if (flags & MQTT_DATA_FLAG_LAST)
    {
        PRINTF("\"\r\n");
    }


    ResponseMessage *message = FromCsv(data);
    xQueueSendToBack(receive_queue, message, portTICK_PERIOD_MS * 0);
}

/*!
 * @brief Called when there is a message on a subscribed topic.
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    LWIP_UNUSED_ARG(arg);

    PRINTF("Received %u bytes from the topic \"%s\": \"", tot_len, topic);
}

void mqtt_subscribe_to_topic(void *context){

	char *receive_topic = (char *)malloc(MAX_TOPIC_SIZE);
	memcpy(receive_topic, device_topic, BASE_TOPIC_SIZE);
	char *response_section = "/r";
	memcpy(receive_topic + BASE_TOPIC_SIZE, response_section, 2);

	mqtt_set_inpub_callback(mqtt_client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);

	err_t err = mqtt_subscribe(mqtt_client, receive_topic, 0, mqtt_topic_subscribed_cb, LWIP_CONST_CAST(void *, receive_topic));

	if (err == ERR_OK) PRINTF("Subscribing to the topic \"%s\" with QoS %d...\r\n", receive_topic, 0);
	else PRINTF("Failed to subscribe to the topic \"%s\" with QoS %d: %d.\r\n", receive_topic, 0, err);
}

static void mqtt_message_published_cb(void *arg, err_t err)
{
    const char *topic = (const char *)arg;

    if (err == ERR_OK) PRINTF("Published to the topic \"%s\".\r\n", topic);
    else PRINTF("Failed to publish to the topic \"%s\": %d.\r\n", topic, err);
}

void publish_message(void *context) {
	char *buffer = (char *)context;
	mqtt_publish(mqtt_client, device_topic, buffer, strlen(buffer), 1, 0, mqtt_message_published_cb, (void *)device_topic);
}

void mqtt_send_task(void *args) {

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
		err_t tcp_result = tcpip_callback(publish_message, buffer);
		if(ERR_OK != tcp_result){
			PRINTF("Failed to run publish message");
		}

		PRINTF("Sending message\n");
	}
}

void mqtt_receive_task(void *args) {
	EventBits_t ready_bits = xEventGroupWaitBits(ready_events, READY_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

	err_t result = tcpip_callback(mqtt_subscribe_to_topic, NULL);
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
