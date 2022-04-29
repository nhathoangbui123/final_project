#ifndef PZEM004TV30_H
#define PZEM004TV30_H

#include "sdkconfig.h"
#include "string.h"
#include <stdint.h>
#include "driver/uart.h"

#define PZEM_DEFAULT_ADDR    0xF8

typedef struct
{
	uart_port_t uart_port;
	int tx_io_num;
	int rx_io_num;
} uart_data_t;

static const int RX_BUF_SIZE = 1024;

typedef struct {
    float voltage;
    float current;
    float power;
    float energy;
    float frequency;
    float pf;
    uint16_t alarms;
} power_meansuare_t; // Measured values

    void PZEM004Tv30_Init(uart_data_t *uart_data, uint8_t addr);

    power_meansuare_t* meansures();
    float voltage();
    float current();
    float power();
    float energy();
    float frequency();
    float pf();


    bool setAddress(uint8_t addr);
    uint8_t getAddress();

    bool setPowerAlarm(uint16_t watts);
    bool getPowerAlarm();

    bool resetEnergy();

    // void search();

    void init(uint8_t addr); // Init common to all constructors

    bool updateValues();    // Get most up to date values from device registers and cache them
    uint16_t recieve(uint8_t *resp, uint16_t len); // Receive len bytes into a buffer

    bool sendCmd8(uint8_t cmd, uint16_t rAddr, uint16_t val, bool check, uint16_t slave_addr); // Send 8 byte command

    void setCRC(uint8_t *buf, uint16_t len);           // Set the CRC for a buffer
    bool checkCRC(const uint8_t *buf, uint16_t len);   // Check CRC of buffer

    uint16_t CRC16(const uint8_t *data, uint16_t len); // Calculate CRC of buffer

#endif // PZEM004T_H
