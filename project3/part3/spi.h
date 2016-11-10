/*
 * spi.h
 *
 *  Created on: Nov 9, 2016
 *      Author: dhth3498
 */

#ifndef INCLUDES_SPI_H_
#define INCLUDES_SPI_H_

#include "MKL25Z4.h"

void SPI0_init(void);
uint8_t SPI0_write(unsigned char data);



#endif /* INCLUDES_SPI_H_ */
