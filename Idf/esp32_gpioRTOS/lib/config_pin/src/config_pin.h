#ifndef CONFIG_PIN_H
#define CONFIG_PIN_H

#include <stdint.h>
#include "driver/gpio.h"

/*define macros*/
#define CONFIG_GPIO_OUTPUT_0 2
#define CONFIG_GPIO_OUTPUT_1 18
#define CONFIG_GPIO_INPUT_0 4
#define CONFIG_GPIO_INPUT_1 5

#define GPIO_OUTPUT_IO_0 CONFIG_GPIO_OUTPUT_0
#define GPIO_OUTPUT_IO_1 CONFIG_GPIO_OUTPUT_1

#define GPIO_INPUT_IO_0 CONFIG_GPIO_INPUT_0
#define GPIO_INPUT_IO_1 CONFIG_GPIO_INPUT_1

//function in handmade lib
void config_pin(uint64_t bitmap, gpio_mode_t gpio_mode, bool upstate, bool downstate, gpio_int_type_t interrupt);

#endif