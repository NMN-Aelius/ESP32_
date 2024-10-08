/*
This programm was made by Nhut Nguyen

Predefine: input --> process --> output

input: pin need to config
process: configuration pin
output: pin act as configurating

*/
//standard for c programming
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//standard int type
#include <inttypes.h>
//rtos library
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
//driver for gpio
#include "config_pin.h"
#include "driver/gpio.h"

/*define macros have been moved to header file "config_pin" in handmade lib*/
#define bitmap_output ((1ULL<<CONFIG_GPIO_OUTPUT_0)|(1ULL<<CONFIG_GPIO_OUTPUT_1))
#define bitmap_input ((1ULL<<CONFIG_GPIO_INPUT_0)|(1ULL<<CONFIG_GPIO_INPUT_1))

#define ESP_INTR_FLAG_DEFAULT 0
/*function for implementing. This also have been copy to "config_pin.c" in lib*/

//Variable for handle task in queue ISR
static QueueHandle_t gpio_evt_queue = NULL;
/*
ISR for GPIO.
This function act each when interrupt happen
And being store in RAM instead of flash memory
*/
static IRAM_ATTR void isr_handler(void* arg)
{
  uint32_t gpio_pin = (uint32_t)arg;
  //this fcn is like a message send an infor to queue (hàng đợi <-> line)
  xQueueSendFromISR(gpio_evt_queue, &gpio_pin, NULL);
}

//Task will run in level of RTOS
static void gpio_task(void* arg)
{
    uint32_t io_num;
    for (;;)
    {
      if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) 
      //receive event value and store it in "io_num"
      //portMAX_DELAY: wait until have new event -> infinitive wait time
      {
        //print information about pin which used to interrupt (this in4 have store in QUEUE)
        printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
      }
    }
}

void app_main(void)
{
  /*BEFORE MAIN RUN IS CONFIG*/
  config_pin(bitmap_output, GPIO_MODE_OUTPUT, 0, 0, GPIO_INTR_DISABLE);
  config_pin(bitmap_input, GPIO_MODE_OUTPUT, 1, 0, GPIO_INTR_POSEDGE);

  //Specific for one pin change
  gpio_set_intr_type(CONFIG_GPIO_INPUT_0, GPIO_INTR_ANYEDGE);

  //Create task for RTOS, this is size of queue to hold tasks for working
  gpio_evt_queue = xQueueCreate(5, sizeof(uint32_t)); // we have 5 tasks is maximum and each task be declare with uint32 type

  //Create task have unlimit access to entire memory map of mcu
  xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

  //Install interrrupt service routine (isr) service for gpio. This action allow GPIO using interrupt handler
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  //Add ISR for specific pin (OR correspond pin - chan tuong ung) GPIO
  gpio_isr_handler_add(GPIO_OUTPUT_IO_0, isr_handler, (void*) GPIO_INPUT_IO_0);
  gpio_isr_handler_add(GPIO_OUTPUT_IO_1, isr_handler, (void*) GPIO_INPUT_IO_1);
  
  // //remove isr handler for gpio number.            //check function :)
  // gpio_isr_handler_remove(GPIO_INPUT_IO_0);
  // //hook isr handler for specific gpio pin again
  // gpio_isr_handler_add(GPIO_INPUT_IO_0, isr_handler, (void*) GPIO_INPUT_IO_0);
  // END CONFIG

  // MAIN PROGRAM
  bool stateLED = 0;
  while(1)
  {
    gpio_set_level(GPIO_OUTPUT_IO_0, stateLED);
    gpio_set_level(GPIO_OUTPUT_IO_1, !stateLED);
    stateLED = !stateLED;
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}