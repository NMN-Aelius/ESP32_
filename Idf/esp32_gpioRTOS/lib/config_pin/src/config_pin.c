#include "config_pin.h"

/*function for implementing*/
void config_pin(uint64_t bitmap, gpio_mode_t  gpio_mode, bool upstate, bool downstate, gpio_int_type_t  interrupt)
{
  gpio_config_t io_config;
  io_config.pin_bit_mask = bitmap;
  io_config.mode = gpio_mode;
  io_config.pull_up_en = upstate;
  io_config.pull_down_en = downstate;
  io_config.intr_type = interrupt;

  gpio_config(&io_config);
}