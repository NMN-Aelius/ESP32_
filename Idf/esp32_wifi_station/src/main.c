#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

//information for configuration esp wifi
// #define wf_ssid "Thoai Hanh"
// #define wf_pass "nguyenthoaioanh1973"
#define wf_ssid "TanNguyen"
#define wf_pass "99999999"
#define wf_retry_conn 5
//end config

#ifndef ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD
    #define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK // Chọn một mức mặc định như WPA2
#endif

#ifndef ESP_WIFI_SAE_MODE
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENT // Chọn chế độ SAE mặc định
#endif

#ifndef EXAMPLE_H2E_IDENTIFIER
#define EXAMPLE_H2E_IDENTIFIER "" // Đặt giá trị mặc định nếu không được định nghĩa
#endif

/* DEFINE PARAMETER, this will cause in compile process NOT in running process as normal*/
//===================ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD===================
#if CONFIG_ESP_WPA3_SAE_PWE_HUNT_AND_PECK
/*
HUNT AND PECK is a method with loop, it take again and again until it catch the match value with security req
=> prevent tracking password attack from hacker
=> working on WPA3 network support, no H2E identifier
*/
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""

//==========================ESP_WIFI_SAE_MODE============================
#elif CONFIG_ESP_WPA3_SAE_PWE_HASH_TO_ELEMENT
/*
HASH_TO_ELEMENT (H2E) is a advance method when compare with HASH TO PECK
=> hash function to convert passwor d to private key
=> working on WPA3 network support
*/
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENT
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID

//=======================EXAMPLE_H2E_IDENTIFIER==========================
#elif CONFIG_ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID

#endif
/*=======================================================================================*/
//=======================CONFIG WIFI AUTHENTICAL==========================

/* No need password 
=> every one can connect
*/
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN

/* wired - equivalent - privacy, determine threshold 
=> this old, many of weak point, WIFI_AUTH_WEP
*/
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP

/* protection access (WPA), pre-sharepoint key (PSK), req password for identifing.
=> wpa2 support, WIFI_AUTH_WPA_PSK
*/
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK

/* The upgrade of WPA => WPA2, more strong private method
=> wpa2 support, WIFI_AUTH_WPA2_PSK
*/
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK

/* All device support WPA or WPA2 can access
=> wpa2 support, WIFI_AUTH_WPA_WPA2_PSK
*/
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK

/* The newest method, strong private method
=> wpa3 support only, WIFI_AUTH_WPA3_PSK
*/
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK

/* Extend device support
=> wpa3 or wpa2 support, WIFI_AUTH_WPA2_WPA3_PSK
*/
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK

/* Wired Authentication and Privacy Infrastructure, CHINE make this :@
=> wpa3 or wpa2 support, WIFI_AUTH_WPA2_WPA3_PSK
*/
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif
/* DEFINE PARAMETER, END*/

//freeRTOS for config
static EventGroupHandle_t wf_event_group;
//
#define wf_connected 0x01 //01
#define wf_connect_fail 0x02 //10

//define tag for log
static const char* tag = "wf_station";
static int retry_conn =0;

static void wf_event_handler( void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        if(esp_wifi_connect() == ESP_OK)
        {
            ESP_LOGI(tag, "\n=====CONNECTION START=====\n");
            xEventGroupSetBits(wf_event_group, wf_connected);
        }
        // printf("JUMPED INTO WIFI STATION START EVENT!!");
    }
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if(retry_conn < wf_retry_conn)
        {
            retry_conn++;
            esp_wifi_connect();
            ESP_LOGW(tag, "Retry to connect %s!", wf_ssid);
        }
        else
        {
            ESP_LOGW(tag, "Could't find %s", wf_ssid);
            xEventGroupSetBits(wf_event_group, wf_connect_fail);
        }
    }
    //success connection and return event for got ip
    else if(event_base == WIFI_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ESP_LOGI(tag, "=====CONNECTION GOT IP=====");

        // //GET IP
        // ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        // //IPSTR represent for ip address format
        // //IP2STR struct represent for ip4 from 1 to 4
        // //This (&event->ip_info.ip) fill 3 first argument of struct IP addr, netmask, gateway
        // ESP_LOGI(tag, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        // xEventGroupSetBits(wf_event_group, wf_connected);
        // //reset retry numm conn
        // retry_conn = 0;
    }
}

void wifi_init_sta()
{
    wf_event_group = xEventGroupCreate();

    //wifi station network interface init
    ESP_ERROR_CHECK(esp_netif_init());

    // create default loop for event: allow register level
    // system init default
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // init type for esp32_wifi as type station
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfig = WIFI_INIT_CONFIG_DEFAULT();
    //CHECK init state
    ESP_ERROR_CHECK(esp_wifi_init(&cfig));

    //point to specific handler to modify for further
    esp_event_handler_instance_t instant_anyID;
    esp_event_handler_instance_t instant_gotIP;
    // event handlerr with registerr interaction
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    ESP_EVENT_ANY_ID, //This is for WF_STA Connect/Disconnnect
                                                    &wf_event_handler,
                                                    NULL,
                                                    &instant_anyID)
    );
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    IP_EVENT_STA_GOT_IP, //This is for WF_STA after successful connect and got IP
                                                    &wf_event_handler,
                                                    NULL,
                                                    &instant_gotIP)
    );

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = wf_ssid,
            .password = wf_pass,
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };

    //Config het roi, gio set no len thoi :))
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    //Gan thong so da config da doi
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    //Start
    ESP_ERROR_CHECK(esp_wifi_start());

/*
When access to wifi, ESP need time to connect success so we will wait until bit connected is set
or check log if connected_fail bit is set.
*/
    EventBits_t bits = xEventGroupWaitBits(wf_event_group,
            wf_connected | wf_connect_fail,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);
    // print to monitor
    if(bits & wf_connected)
    {
        ESP_LOGI(tag, "\nwifi_init_station login to SSID:%s password:%s is SUCCESS\n", wf_ssid , wf_pass);
        printf("\n========CONNECT SUCCESSFULLY========\n");
    }
    else if(bits & wf_connect_fail)
    {
        ESP_LOGI(tag, "\nwifi_init_station login to SSID:%s password:%s is FAIL\n", wf_ssid , wf_pass);
        printf("\n========FAIL TO CONNECT========\n");
    }
    else
    {
        ESP_LOGI(tag, "\n========Unexpected Event Appeared========\n");
        printf("\n========Unexpected Event Appeared========\n");
    }

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

    ESP_LOGI(tag, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}