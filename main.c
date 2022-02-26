#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"

/**
 * Brief:
 * This test code shows how to configure multiple gpio interrupts
 *
 * GPIO status:
 * GPIO2   : output ( built-in led on Devkit-V1 )
 * GPIO34  : output ( externally pulled up )
 * GPIO35  : output ( externally pulled up )
 *
 * Test:
 * Connect GPIO34 with simple switch and ground
 * Connect GPIO35 with simple switch and ground
 */

#define ESP_INTR_FLAG_DEFAULT 0

#define BUILTIN_LED 2
#define GPIO_INPUT_IO_0 34
#define GPIO_INPUT_IO_1 35
#define portMAX_DELAY 0     //++ Delete this line to get infinite delay hault at "xSemaphoreTake" function

int gpio34_count = 0;
int gpio35_count = 0;

SemaphoreHandle_t xSemaphore_1 = NULL;      //++ Initiate 2 interrupt handles
SemaphoreHandle_t xSemaphore_2 = NULL;

void IRAM_ATTR gpio34_isr_handler(void* arg){   //++ First ISR
    //notify the button task
    xSemaphoreGiveFromISR(xSemaphore_1, NULL);
}

void IRAM_ATTR gpio35_isr_handler(void* arg){   //++ Second ISR
    //notify the button task
    xSemaphoreGiveFromISR(xSemaphore_2, NULL);
}

void button_task(void* arg) {

    //infinite loop
    while(1) {

        //check for the notification from the ISR
        if(xSemaphoreTake(xSemaphore_1,portMAX_DELAY) == pdTRUE) {
            vTaskDelay( 50 / portTICK_PERIOD_MS);
            gpio34_count++;
            
            printf("INTERRUPT COUNT ON GPIO : %d is : %d\n", GPIO_INPUT_IO_0, gpio34_count);
        }

        //check for the notification from the ISR
        if(xSemaphoreTake(xSemaphore_2,portMAX_DELAY) == pdTRUE) {
            vTaskDelay( 50 / portTICK_PERIOD_MS);
            gpio35_count--;
            
            printf("INTERRUPT COUNT ON GPIO : %d is : %d\n", GPIO_INPUT_IO_1, gpio35_count);
        }
        
        vTaskDelay( 250 / portTICK_PERIOD_MS);

    }
}

void app_main(void)
{
    gpio_reset_pin(BUILTIN_LED);
    gpio_reset_pin(GPIO_INPUT_IO_0);
    gpio_reset_pin(GPIO_INPUT_IO_1);

    /* Set the GPIO & LED direction */
    gpio_set_direction(BUILTIN_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_INPUT_IO_0, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_INPUT_IO_1, GPIO_MODE_INPUT);

    gpio_set_level(BUILTIN_LED, 0);

    //create the binary semaphore
    xSemaphore_1 = xSemaphoreCreateBinary();
    xSemaphore_2 = xSemaphoreCreateBinary();

    //enable interrupt on anyedge for button pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);     //++ Use "GPIO_INTR_POSEDGE" or "GPIO_INTR_NEGEDGE" for positive or negative edge triggered ISR respectively
    gpio_set_intr_type(GPIO_INPUT_IO_1, GPIO_INTR_ANYEDGE);

    //start the task that will handle the button
    xTaskCreate(button_task, "button_task", 2048, NULL, configMAX_PRIORITIES-1, NULL);

    //install ISR service with default configuration
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    //attach the interrupt service routine 
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio34_isr_handler, NULL);
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio35_isr_handler, NULL);

    while(1)        //++ Blink built-in LED Continuously
    {
        gpio_set_level(BUILTIN_LED,0);
        vTaskDelay(250/portTICK_PERIOD_MS);
        gpio_set_level(BUILTIN_LED,1);
        vTaskDelay(250/portTICK_PERIOD_MS);
    }

}
