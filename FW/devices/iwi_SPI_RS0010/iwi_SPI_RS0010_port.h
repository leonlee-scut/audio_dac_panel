#ifndef IWI_SPI_RS0010_PORT_H
#define IWI_SPI_RS0010_PORT_H

#include "Driver_GPIO.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t SPI_RS0010_PortInitOutput(ARM_GPIO_Pin_t pin);
void SPI_RS0010_PortWrite(ARM_GPIO_Pin_t pin, uint32_t value);
void SPI_RS0010_PortDelayMs(uint32_t milliseconds);

#ifdef __cplusplus
}
#endif

#endif
