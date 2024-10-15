#include "stdio.h"
#include "stdlib.h"
#include "inttypes.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#define out_1 2
#define out_2 3
#define out_3 4
#define out_4 5

#define in_1 7
#define in_2 8
#define in_3 9
#define in_4 10

void config_pin(uint64_t bitmap, gpio_mode_t  gpio_mode,
                bool p_upstate, bool p_downstate,
                gpio_int_type_t  interrupt)
{
  gpio_config_t io_config;
  io_config.pin_bit_mask = bitmap;
  io_config.mode = gpio_mode;
  io_config.pull_up_en = p_upstate;
  io_config.pull_down_en = p_downstate;
  io_config.intr_type = interrupt;

  gpio_config(&io_config);
}

#define bitmap_output ((1ULL<<out_1)|(1ULL<<out_2)|(1ULL<<out_3)|(1ULL << out_4))
#define bitmap_input ((1ULL<<in_1)|(1ULL<<in_2)|(1ULL<<in_3)|(1ULL << in_4))

TaskHandle_t ptask = NULL;
/*
    vTaskDelete, vTaskSuspend, vTaskResume,
    is the action we could do to interact with a specific task.
*/

// Generate Queue Task
static QueueHandle_t gpio_queue_event = NULL;

//Queue Task run on RTOS
/*
Task 1: 
Task 2:
Task 3:
Task 4:
*/
static void gpio_task(void* arg)
{

}

void app_main()
{
    //this like void setup on arduino
    config_pin(bitmap_output, GPIO_MODE_OUTPUT, 0, 0, GPIO_INTR_DISABLE);
    config_pin(bitmap_input, GPIO_MODE_INPUT, 0, 0, GPIO_INTR_ANYEDGE);

    xTaskCreate(gpio_task, "GPIO", 1024, NULL, 2, &ptask);
    /*
    Task will run after this config until it cause
    vTaskSuspend or vTaskDelay
    */

    //Main program
    bool status_out1, status_out2, status_out3, status_out4;
    while(1)
    {
        gpio_set_level(out_1, status_out1);
        gpio_set_level(out_1, status_out2);
        gpio_set_level(out_1, status_out3);
        gpio_set_level(out_1, status_out4);

        // Delay for 1 second ==> Run into blocking state
        vTaskDelay(1000/portTICK_PERIOD_MS); 
    }
}
