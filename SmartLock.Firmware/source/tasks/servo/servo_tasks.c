/*
 * servo_tasks.c
 *
 *  Created on: 1 dic. 2020
 *      Author: Miguel
 */


#include <stddef.h>
#include "../../../utilities/fsl_debug_console.h"
#include "../../models/messages.h"

#include "FreeRTOS.h"
#include "servo_tasks.h"
#include "event_groups.h"
#include "task.h"

#include "fsl_ftm.h"

extern EventGroupHandle_t servo_events;

const uint8_t OPEN_DUTYCYCLE = 97;
const uint8_t CLOSE_DUTYCYCLE = 88;

void change_dutycycle(uint8_t dutycycle){

    /* Disable channel output before updating the dutycycle */
    FTM_UpdateChnlEdgeLevelSelect(FTM0, kFTM_Chnl_0, 0U);

    /* Update PWM duty cycle */
    //FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_CenterAlignedPwm, updatedDutycycle);
    FTM_UpdatePwmDutycycle(FTM0, kFTM_Chnl_0, kFTM_EdgeAlignedPwm, dutycycle);
    PRINTF("%d.\r\n",dutycycle);

    /* Software trigger to update registers */
    FTM_SetSoftwareTrigger(FTM0, true);

    /* Start channel output with updated dutycycle */
    FTM_UpdateChnlEdgeLevelSelect(FTM0, kFTM_Chnl_0, kFTM_LowTrue);
}

void open_lock(void){
	change_dutycycle(OPEN_DUTYCYCLE);
}


void close_lock(void){
	change_dutycycle(CLOSE_DUTYCYCLE);
}

const TickType_t LOCK_DELAY = 1500 / portTICK_PERIOD_MS;

void servo_action_task(void* args){
	if(NULL == servo_events) {
		PRINTF("No event group configured");
		vTaskDelete(NULL);
	}

	uint32_t wait_bits = SERVO_OPEN_BIT;
	while(1){
		EventBits_t event_bits = xEventGroupWaitBits(servo_events, wait_bits, pdTRUE, pdFALSE, portMAX_DELAY);
		if(SERVO_OPEN_BIT == (event_bits & SERVO_OPEN_BIT)) {
			PRINTF("Open servo");
			open_lock();
			vTaskDelay(LOCK_DELAY);
			close_lock();
		}
	}
}
