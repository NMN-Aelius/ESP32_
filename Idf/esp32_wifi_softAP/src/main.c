#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"


#define wifi_ssid "ESP32"
#define wifi_password "minhnhut10"
#define wifi_channel 1
#define wifi_sat_conn 5 // this is max station support

static const char *tag = "wifi_softAP";

//event handler
static void wifi_handler(void* arg, esp_event_base_t eve_base,
                        int32_t eve_id, void* eve_data)
{
    if(eve_id == WIFI_EVENT_AP_STACONNECTED) //macro in struct have value 14
    {
        wifi_event_ap_staconnected_t *eve = (wifi_event_ap_staconnected_t *) eve_data;
        ESP_LOGI(tag, "station "MACSTR" join, AID=%d",
                MAC2STR(eve->mac), eve->aid);
    }
    
    else if (eve_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* eve = (wifi_event_ap_stadisconnected_t*) eve_data;
        ESP_LOGI(tag, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(eve->mac), eve->aid, eve->reason);
    }
}

//wifi init softAP
void wifi_init_softAP(void)
{
    // create pool for esp wifi connection
    ESP_ERROR_CHECK(esp_netif_init());
    
    // create default loop for event: allow register level
    // system init default
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // init type for esp32_wifi as type softAP
    esp_netif_create_default_wifi_ap();

    // config wifi connection
    // attempt all default can set for esp32 wifi
    wifi_init_config_t cfig = WIFI_INIT_CONFIG_DEFAULT();
    // check init state
    ESP_ERROR_CHECK(esp_wifi_init(&cfig)); // this arg is pointer so we put address on that

    // event handlerr with registerr interaction
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    ESP_EVENT_ANY_ID,
                                                    &wifi_handler,
                                                    NULL,
                                                    NULL));

    // config for some type of boardcast security method
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = wifi_ssid,
            .ssid_len = strlen(wifi_ssid),
            .channel = wifi_channel,
            .password = wifi_password,
            .max_connection = wifi_sat_conn,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            //only wpa3 have h2e method
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
        },
    };

    if (strlen(wifi_password) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    // Finish config information

    // Set mode after done every config infor
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    // Input config parameter into mode
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    // Start ESP32 wifi
    ESP_ERROR_CHECK(esp_wifi_start());

    // print to monitor
    ESP_LOGI(tag, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             wifi_ssid , wifi_password, wifi_channel);

}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(tag, "ESP_WIFI_MODE_AP");
    wifi_init_softAP();
}