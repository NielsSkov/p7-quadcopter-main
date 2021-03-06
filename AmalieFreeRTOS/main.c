#include "main.h"
#include "functions.h"

#include <avr/io.h>
#include <util/delay.h>

// Free-RTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"

int count=0;
int counterold;
int countercontrol;
char c_low = 0;
char c_high = 0;
int kill;
float u_k1[4];
short reading;
float y_k[3];
xTaskHandle xHandle;

int nrOfSend = 10;

int main()
{
	// Initialization
	PWM_init(0);
	USART_Init(MYUBRR);
	//ADC_Init();
	LED_DDR = 0xFF;
	LED = 0x00;
	LED2_DDR = 0xFF;
	LED2 = 0x00;

	// Task Creation
	xTaskCreate(Controllers, "Control", 1000, NULL, configMAX_PRIORITIES - 1, NULL );
	xTaskCreate(Communication, "Com", 1000, NULL, configMAX_PRIORITIES - 2, &xHandle);

	//while (1)
	//{
	//	LED = 0xFF;
	//	_delay_ms(1000);
	//	LED = 0x00;
	//	_delay_ms(1000);
	//}

	//LED |= (~0x00);
	_delay_ms(1000);
	int duty = 128;
	Set_PWM_duty(duty, duty, duty, duty);
	_delay_ms(10000);
	//Set_PWM_duty(DUTY_INIT, DUTY_INIT, DUTY_INIT, DUTY_INIT);
	//_delay_ms(1000);
	//duty = (int)(EQU_SPEED * 0.1598 + 122.79);
	//Set_PWM_duty(duty, duty, duty, duty);

	//while (count<40)
	//{
	//	Set_PWM_duty(duty, duty, duty, duty);
	//	_delay_ms(500);
	//	if (reading)
	//	{
	//		duty = DUTY_INIT + count;
	//		reading = 0;
	//	}
	//	else
	//	{
	//		duty = DUTY_INIT - count;
	//		reading = 1;
	//	}
	//	count += 2;

	//	if (duty > 254)
	//		duty = 254;
	//	if (duty < 128)
	//		duty = 128;
	//}
	//while (1)
	//{
	//	duty = 128;
	//	Set_PWM_duty(duty, duty, duty, duty);
	//}


	//unsigned char dummy[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//int i = 0;
	//while (1)
	//{
	//	//LED = 0xFF;
	//	for (i = 0; i < 1; i++)
	//	{
	//		dummy[i] = USART_Receive();
	//	}
	//	for (i = 0; i < 1; i++)
	//	{
	//		USART_Transmit(dummy[i]);
	//	}
	//}

	// Scheduler Start
	vTaskStartScheduler();
	return 0;
}


void Controllers(void *pvParameters)
{
	c_low = 0;
	c_high = 0;
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while (1)
	{
		if (count<1000)
		{
			//LED = 0xFF;
			Controller();
			count++;
			ApplyVelocities();
			//LED = 0x00;
		}
		else
		{
			Set_PWM_duty(128, 128, 128, 128);
			//if (nrOfSend > 0)
			//{
			//	c_low = (char)(countercontrol & 0x00FF);
			//	c_high = (char)((countercontrol & 0xFF00) >> 8);
			//	USART_Transmit(c_high);
			//	USART_Transmit(c_low);
			//	c_low = (char)(counterold & 0x00FF);
			//	c_high = (char)((counterold & 0xFF00) >> 8);
			//	USART_Transmit(c_high);
			//	USART_Transmit(c_low);
			//	nrOfSend = nrOfSend - 1;
			//}
			count = 6000;
		}
		if (reading)
		{
			vTaskDelete(xHandle);
			xTaskCreate(Communication, "Com", 1000, NULL, configMAX_PRIORITIES - 2, &xHandle);
			reading = 0;
			//LED2 = 0x00;
		}
		vTaskDelayUntil(&xLastWakeTime, 35);
	}
	vTaskDelete(NULL);
}


void Communication(void *pvParameters)
{
	while (1)
	{
		int pack = 0;
		pack = CheckPackageArrival();
		if (pack)
			GetPackage();
	}
	vTaskDelete(NULL);
}


void vApplicationIdleHook( void ) //N�r FreeRTOS ikke har en task den skal lave
{
}