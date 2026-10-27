#include "iwi_SPI_RS0010_port.h"

#include "GPIO_STM32.h"
#include "stm32f4xx_hal.h"

#include <stddef.h>

extern "C" int32_t SPI_RS0010_PortInitOutput(ARM_GPIO_Pin_t pin)
{
  int32_t status = Driver_GPIO0.Setup(pin, NULL);
  if (status != ARM_DRIVER_OK) return status;

  status = Driver_GPIO0.SetOutputMode(pin, ARM_GPIO_PUSH_PULL);
  if (status != ARM_DRIVER_OK) return status;

  status = Driver_GPIO0.SetPullResistor(pin, ARM_GPIO_PULL_NONE);
  if (status != ARM_DRIVER_OK) return status;

  return Driver_GPIO0.SetDirection(pin, ARM_GPIO_OUTPUT);
}

extern "C" void SPI_RS0010_PortWrite(ARM_GPIO_Pin_t pin, uint32_t value)
{
  Driver_GPIO0.SetOutput(pin, value);
}

extern "C" void SPI_RS0010_PortDelayMs(uint32_t milliseconds)
{
  HAL_Delay(milliseconds);
}
