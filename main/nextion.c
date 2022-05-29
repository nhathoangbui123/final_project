#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "soc/uart_struct.h"
#include "string.h"
#include <stdio.h>
#include "nextion.h"
#include "buzzer.h"

#define TXD_PIN (GPIO_NUM_23)
#define RXD_PIN (GPIO_NUM_22)
#define nUART	(UART_NUM_2)
#define OFF   1
#define ON    0

enum 
{
    UNKNOWN_CMD,
    D1ON,               
    D1OFF,  
    D2ON,               
    D2OFF,
    D3ON,               
    D3OFF,
    D4ON,               
    D4OFF,
    ALL_ON,
    ALL_OFF,
};

char *CMD_STRINGS[] = {
    "  ",                   //UNKNOWN_CMD
    "D1ON",               
    "D1OFF",  
    "D2ON",               
    "D2OFF",
    "D3ON",               
    "D3OFF",
    "D4ON",               
    "D4OFF",
    "ALL_ON",
    "ALL_OFF", 
};

bool Dev1_state = false;
bool Dev2_state = false;
bool Dev3_state = false;
bool Dev4_state = false;

void initNextion() {
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(nUART, &uart_config);
    uart_set_pin(nUART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(nUART, RX_BUFFER * 2, 0, 0, NULL, 0);
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(nUART, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    ESP_LOGI(logName, "Sent %s", data);
    return txBytes;
}

static void tx_task(void* param)
{
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    char *cmd = (char*)malloc(TX_BUFFER);
    param_t data = *((param_t*)param);
    while (1) {

    sprintf(cmd, "Monitor.power_v.txt=\"%f\"\xFF\xFF\xFF",data.power);
    ESP_LOGI("logName", "Monitor.power_v.txt=%f", data.power);
    sendData(TX_TASK_TAG, cmd);	

    sprintf(cmd, "Monitor.freq_v.txt=\"%f\"\xFF\xFF\xFF",data.frequency);
    ESP_LOGI("logName", "Monitor.freq_v.txt=%f", data.frequency);
    sendData(TX_TASK_TAG, cmd);

    sprintf(cmd, "Monitor.vol_v.txt=\"%f\"\xFF\xFF\xFF",data.voltage);
    ESP_LOGI("logName", "Monitor.vol_v.txt=%f", data.voltage);
    sendData(TX_TASK_TAG, cmd);

    sprintf(cmd, "Monitor.amp_v.txt=\"%f\"\xFF\xFF\xFF",data.current);
    ESP_LOGI("logName", "Monitor.amp_v.txt=%f", data.current);
    sendData(TX_TASK_TAG, cmd);	           
    vTaskDelay(10000 / portTICK_PERIOD_MS); //Transmit every 10 seconds
    }
}

static void rx_task()
{
	esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUFFER+1);
    char *dstream = malloc(RX_BUFFER+1);

    while (1) {
        memset(dstream,0,sizeof(malloc(RX_BUFFER+1)));
        const int rxBytes = uart_read_bytes(nUART, data, RX_BUFFER, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            snprintf(dstream, RX_BUFFER+1, "%s", data);
            
        }
        if(ParseCmd(dstream)==0)continue;

        if (Dev1_state && Dev2_state && Dev3_state && Dev4_state) {
          sendData(TX_TASK_TAG, "Control.t9.txt=\"ON\"\xFF\xFF\xFF");	
          sendData(TX_TASK_TAG, "Control.t9.pco=2016\xFF\xFF\xFF");	
          sendData(TX_TASK_TAG, "Control.bt4.val=1\xFF\xFF\xFF");	
        } else if (! (Dev1_state && Dev2_state && Dev3_state && Dev4_state) ) {
          sendData(TX_TASK_TAG, "Control.t9.txt=\"OFF\"\xFF\xFF\xFF");
          sendData(TX_TASK_TAG, "Control.t9.pco=63488\xFF\xFF\xFF");	
          sendData(TX_TASK_TAG, "Control.bt4.val=0\xFF\xFF\xFF");
        }

    }
    
    free(data);
	free(dstream);
}

void nextion_main(param_t param)
{
    initNextion();
	//Set wifi icon on the screen
	  sendData(TX_TASK_TAG, "Monitor.wifi.pic=11\xFF\xFF\xFF");
    vTaskDelay(5 / portTICK_PERIOD_MS);
    sendData(TX_TASK_TAG, "Control.wifi.pic=11\xFF\xFF\xFF");
    vTaskDelay(5 / portTICK_PERIOD_MS);
	
	//create the asynchronous send and receive tasks 
    xTaskCreate(&rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(&tx_task, "uart_tx_task", 1024*2, (void *)&param, configMAX_PRIORITIES-1, NULL);
}

#define __NUMBER_OF_CMD_STRINGS (sizeof(CMD_STRINGS) / sizeof(*CMD_STRINGS))
int GetCmd(char *text)
{
	int i = 0;
	for (i = 0; i < __NUMBER_OF_CMD_STRINGS; i++) {
		if (strcmp(text, CMD_STRINGS[i]) == 0) {
			return i;
		}
	}
	return UNKNOWN_CMD;
}
int ParseCmd(char *text)
{
	int cmd_index = GetCmd(text);
    switch (cmd_index) {
      case D1ON:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd D1ON");
        gpio_set_level(DEVICE_1, ON);
        Dev1_state = true;
        return 0;
      break;
      case D1OFF:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd D1OFF");
        gpio_set_level(DEVICE_1, OFF);
        Dev1_state = false;
        return 0;
      break;
      case D2ON:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd D2ON");
        gpio_set_level(DEVICE_2, ON);
        Dev2_state = true;
        return 0;
      break;
      case D2OFF:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd D2OFF");
        gpio_set_level(DEVICE_2, OFF);
        Dev2_state = false;
        return 0;
      break;
      case D3ON:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd D3ON");
        gpio_set_level(DEVICE_3, ON);
        Dev3_state = true;
        return 0;
      break;
      case D3OFF:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd D3OFF");
        gpio_set_level(DEVICE_3, OFF);
        Dev3_state = false;
        return 0;
      break;
      case D4ON:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd D4ON");
        gpio_set_level(DEVICE_4, ON);
        Dev4_state = true;
        return 0;
      break;
      case D4OFF:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd D4OFF");
        gpio_set_level(DEVICE_4, OFF);
        Dev4_state = false;
        return 0;
      break;
      case ALL_ON:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd ALL_ON");
        gpio_set_level(DEVICE_1, ON);
        gpio_set_level(DEVICE_2, ON);
        gpio_set_level(DEVICE_3, ON);
        gpio_set_level(DEVICE_4, ON);
        Dev1_state = true;
        Dev2_state = true;
        Dev3_state = true;
        Dev4_state = true;
        return 0;
      break;
      case ALL_OFF:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd ALL_OFF");
        gpio_set_level(DEVICE_1, OFF);
        gpio_set_level(DEVICE_2, OFF);
        gpio_set_level(DEVICE_3, OFF);
        gpio_set_level(DEVICE_4, OFF);
        Dev1_state = false;
        Dev2_state = false;
        Dev3_state = false;
        Dev4_state = false;
        return 0;
      break;
      case UNKNOWN_CMD:
        ESP_LOGI("CMD_HMI","\nUART RX: cmd UNKNOWN_CMD");
      return 1;
      break;
	}
	return 0;
}