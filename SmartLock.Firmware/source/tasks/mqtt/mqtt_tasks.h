/*
 * mqtt_tasks.h
 *
 *  Created on: 1 dic. 2020
 *      Author: Miguel Pérez García
 */

#ifndef TASKS_MQTT_MQTT_TASKS_H_
#define TASKS_MQTT_MQTT_TASKS_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "mqtt.h"


void init_mqtt_tasks(void *args);

#endif /* TASKS_MQTT_MQTT_TASKS_H_ */
