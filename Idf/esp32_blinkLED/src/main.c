#include<stdio.h>

#include<freertos/FreeRTOS.h>
#include<freertos/task.h>

#include<driver/gpio.h>
#include<esp_log.h>
#include<sdkconfig.h> // this file content value some macros, make sure that define have been 
/*
upon define in sdk config file, for examble in file "sdkconfig.defaults.esp32" have contain this line of code "CONFIG_BLINK_GPIO = 5"
this macros now can use in the same project.
MAKE SURE TO CREATE THE SDK FILE that define "IntLED = 2" with name file is "sdkconfig.defaults.esp32"
*/
#define IntLED 2
#define gpio_led IntLED

static uint8_t ledstate=0;
//ping tag for esp log
static const char* tag = "led";

static void blinkLED(void)
{
  gpio_set_level(IntLED, ledstate);
}

static void configure_led(void)
{
  uint8_t bitmark = (1ULL << IntLED);

  gpio_config_t io_config;
  io_config.pin_bit_mask = bitmark;
  io_config.mode = GPIO_MODE_OUTPUT;
  io_config.pull_up_en = 0;
  io_config.pull_down_en = 0;
  io_config.intr_type = GPIO_INTR_DISABLE;

  gpio_config(&io_config);
  //finish config GPIO pin
  
  /*
  Another way to declare GPIO:
  ESP_LOGI(TAG, "Example configure blink LED!");
  gpio_reset_pin(IntLED);
  gpio_set_direction(IntLED, GPIO_MODE_OUTPUT);
  */
}

//MAIN PROGRAM
void app_main(void)
{
  configure_led();
  while(1)
  {
    // print and check led state
    ESP_LOGI(tag, "LED state; %s", ledstate == true? "ON" : "OFF");
    blinkLED();
    //toggle ledstate for blinking
    ledstate = !ledstate;
    vTaskDelay(1000/ portTICK_PERIOD_MS); // time is have ms as unit
  }
}