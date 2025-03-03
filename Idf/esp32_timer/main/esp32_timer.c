#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "version.h"

// Define tag for LOG
const char* tag_timer = "TAG_TIMER_NAME";
const char* tag_mmc = "TAG_HEAP";
const char* tag_dominion = "TAG_DOMINION";

// Định nghĩa struct với typedef
typedef struct {
    int age;
    char name[20];
} argument_config;

// Callback function
void t1_callback_fcn(void* argu) {
    argument_config* ptrcallback = (argument_config*) argu;
    ESP_LOGI(tag_timer, "Trong argument-config: Age = %d, Name = %s", ptrcallback->age, ptrcallback->name);
}

// Global variable for hold periodic timer
esp_timer_handle_t periodic_timer;
// Global variable for hold one-shot timer
esp_timer_handle_t one_timer;

// Function to configurer timer
int config_timer(void* ptr_arg, char* ptr_name, esp_timer_handle_t* timer_handle, esp_timer_create_args_t timer) {
    esp_timer_create_args_t cfg_timer = {
        .callback = t1_callback_fcn,
        .dispatch_method = ESP_TIMER_TASK,
        .arg = ptr_arg,
        .name = ptr_name
    };
    
    timer = cfg_timer;

    return esp_timer_create(&timer, timer_handle);
}

// app_main
void app_main(void) {
    //mastery of code 
    ESP_LOGI(tag_dominion, "THIS IS NHUT, VERSION %d.", version);

    // allocation for newptr to handle data type in struct.
    argument_config* newptr = (argument_config*)malloc(sizeof(argument_config));
    argument_config* newptr1 = (argument_config*)malloc(sizeof(argument_config));
    if (newptr == NULL || newptr1 == NULL) {
        ESP_LOGE(tag_mmc, "Failed to allocate memory!");
        return;
    }

    // assign new value
    newptr->age = 10;
    newptr1->age = 11;
    //due to it a string, so must be add by command strcpy instead of normal assign
    strcpy(newptr->name, "nhut");
    strcpy(newptr1->name, "nhut2");

    // create periodic esp_timer
    esp_timer_create_args_t timer_per;
    esp_err_t err = config_timer(newptr, newptr->name, &periodic_timer, timer_per);
    if (err != ESP_OK) {
        ESP_LOGE(tag_timer, "Timer PERIODIC fail!");
        free(newptr);
        return;
    }

    // timer start
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 500000));
    ESP_LOGI(tag_timer, "Timer PERIODIC created and started successfully!");

    // create one-shot esp_timer
    esp_timer_create_args_t timer_one;
    esp_err_t err1 = config_timer(newptr1, newptr1->name, &one_timer, timer_one);
    if (err1 != ESP_OK) {
        ESP_LOGE(tag_timer, "Timer ONE SHOT fail!");
        free(newptr1);
        return;
    }

    // timer start
    ESP_ERROR_CHECK(esp_timer_start_once(one_timer, 3000000));
    ESP_LOGI(tag_timer, "Timer ONE SHOT created and started successfully!");
}
