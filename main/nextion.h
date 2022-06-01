static const int RX_BUFFER = 1024;
static const int TX_BUFFER = 256;
#include "demo_config.h"

#define DEVICE_1    GPIO_NUM_26
#define DEVICE_2    GPIO_NUM_27
#define DEVICE_3    GPIO_NUM_32
#define DEVICE_4    GPIO_NUM_33
#define DEVICE_ALL  GPIO_NUM_25

static const char *TX_TASK_TAG = "TX_TASK";
static const char *RX_TASK_TAG = "RX_TASK";
static const char *ESP_SOFT_RESET = "espreset";

void nextion_main(param_t param);
int sendData(const char* logName, const char* data);
void initNextion();
int ParseCmd(char *text);
int GetCmd(char *text);

