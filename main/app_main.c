/* tls-mutual-auth example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "pzem.h"
#include "demo_config.h"
int aws_iot_demo_main( int argc, char ** argv, param_t param);

static const char *TAG = "MQTT_EXAMPLE";
param_t param;
/*
 * Prototypes for the demos that can be started from this project.  Note the
 * MQTT demo is not actually started until the network is already.
 */
static void pzem_task(void *arg){
    uart_data_t uart_data= {
        .uart_port = UART_NUM_1,
        .tx_io_num = GPIO_NUM_4,
        .rx_io_num = GPIO_NUM_5,
    };
    PZEM004Tv30_Init(&uart_data,PZEM_DEFAULT_ADDR);
    bool pzem004_ajustado = false;
	const uint8_t addr = 0x42;

    while (1) {
    	if (!pzem004_ajustado)
    	{
    		ESP_LOGI("TAG_PZEM004T", "Set address to 0x42");
    		setAddress(addr);
    		if (addr == getAddress())
    		{
				ESP_LOGI("TAG_PZEM004T", "New Address %#.2x", getAddress());
				ESP_LOGI("TAG_PZEM004T", "Reset Energy");
				resetEnergy();
				pzem004_ajustado = true;
    		}
    		else
    		{
    			ESP_LOGI("TAG_PZEM004T", "Error to set New Address");
    			vTaskDelay( 1000 / portTICK_PERIOD_MS);
    		}
    	}

    	//Ensure that mqtt is connected
		if(pzem004_ajustado) {
			power_meansuare_t * mensures = meansures();
            ESP_LOGI("TAG_PZEM004T","voltage %f",mensures->voltage);
            ESP_LOGI("TAG_PZEM004T","current %f",mensures->current);
            ESP_LOGI("TAG_PZEM004T","power %f",mensures->power);
            ESP_LOGI("TAG_PZEM004T","energy %f",mensures->energy);
            ESP_LOGI("TAG_PZEM004T","frequency %f",mensures->frequency);
            ESP_LOGI("TAG_PZEM004T","pf %f",mensures->pf);
            param.voltage=mensures->voltage;
            param.current=mensures->current;
            param.power=mensures->power;
            param.energy=mensures->energy;
            param.frequency=mensures->frequency;
            param.pf=mensures->pf;
			vTaskDelay( 1000 / portTICK_PERIOD_MS );
		}
		else
		{
			taskYIELD();
		}
	}
}
bool run_pzem(){
    xTaskCreate(&pzem_task, "pzem_task", 9216, NULL, 1, NULL);
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
    return true;
}
void app_main()
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    
    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    if(run_pzem()){
        aws_iot_demo_main(0,NULL, param);
    }
}
