/*
 * i2c.h
 *
 * Created: 1/9/2021 09:06:33
 *  Author: Wenceslao
 */ 


#ifndef I2C_H_
#define I2C_H_


#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

#define F_CPU 16000000L
#define BAUDRATE 9600
#define ADDRESS 0x3C

#define DEBUG_LOG			0
#define SUCCESS				0

#define TW_SCL_PIN			PORT21
#define TW_SDA_PIN			PORT20

#define TW_SLA_W(ADDRESS)		((ADDRESS << 1) | TW_WRITE)
#define TW_SLA_R(ADDRESS)		((ADDRESS << 1) | TW_READ)
#define TW_READ_ACK			1
#define TW_READ_NACK		0

typedef uint16_t ret_code_t;

typedef enum {
	TW_FREQ_100K,
	TW_FREQ_250K,
	TW_FREQ_400K
} twi_freq_mode_t;

void tw_init(twi_freq_mode_t twi_freq, bool pullup_en);
ret_code_t tw_master_transmit(uint8_t slave_addr, uint8_t* p_data, uint8_t len, bool repeat_start);
ret_code_t tw_master_receive(uint8_t slave_addr, uint8_t* p_data, uint8_t len);



#endif /* I2C_H_ */