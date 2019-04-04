/*
 FreeVario http://FreeVario.org

 Copyright (c), FreeVario (http://freevario.org)
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version. see <http://www.gnu.org/licenses/>
 */


#include "gpstask.h"
#include <stdlib.h>
#include <string.h>

gps_t  hgps __attribute__((section(".ccmram")));

extern TaskHandle_t xReceiveNotify;
extern UART_HandleTypeDef FV_UARTGPS;
extern QueueHandle_t uartQueueHandle;

void StartGPSTask(void const * argument)
{
  /* USER CODE BEGIN StartGPSTask */
	gps_init(&hgps);
	uint8_t buffer[SENDBUFFER]; //DMA buffer can't use ccm
	//uint8_t rcvdCount;
	configASSERT(xReceiveNotify == NULL);
	__HAL_UART_ENABLE_IT(&FV_UARTGPS, UART_IT_IDLE);

	/* Infinite loop */
	for (;;) {
		memset(buffer, 0, sizeof(buffer));
		__HAL_UART_CLEAR_IDLEFLAG(&FV_UARTGPS);
		__HAL_UART_ENABLE_IT(&FV_UARTGPS, UART_IT_IDLE);

		if (HAL_UART_Receive_DMA(&FV_UARTGPS, buffer, sizeof(buffer)) != HAL_OK) {
			// error
		}
		xReceiveNotify = xTaskGetCurrentTaskHandle();
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY);

		//rcvdCount = sizeof(buffer) - huart1.hdmarx->Instance->NDTR;
		HAL_UART_DMAStop(&FV_UARTGPS);

		gps_process(&hgps, &buffer, sizeof(buffer));
		xQueueSendToBack(uartQueueHandle, buffer, 10);

	}
  /* USER CODE END StartGPSTask */
}

