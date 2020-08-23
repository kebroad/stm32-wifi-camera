/**************************************************************************//**
 * Copyright 2018, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include <stdbool.h>
#include "sl_wfx_host_pin.h"

static void MX_SPI1_Init(void);

SPI_HandleTypeDef hspi1;

sl_status_t sl_wfx_host_init_bus( void )
{
  /* Init SPI interface */
  MX_SPI1_Init();
  return SL_STATUS_OK;
}

sl_status_t sl_wfx_host_deinit_bus( void )
{
  /* Deinit SPI interface */
  HAL_SPI_MspDeInit(&hspi1);
  return SL_STATUS_OK;
}

sl_status_t sl_wfx_host_spi_cs_assert()
{
  HAL_GPIO_WritePin(SL_WFX_CS_PORT_SPI, SL_WFX_CS_GPIO_SPI, GPIO_PIN_RESET);
  return SL_STATUS_OK;
}

sl_status_t sl_wfx_host_spi_cs_deassert()
{
  HAL_GPIO_WritePin(SL_WFX_CS_PORT_SPI, SL_WFX_CS_GPIO_SPI, GPIO_PIN_SET);
  return SL_STATUS_OK;
}

sl_status_t sl_wfx_host_spi_transfer_no_cs_assert(sl_wfx_host_bus_transfer_type_t type,
                                                  uint8_t *header,
                                                  uint16_t header_length,
                                                  uint8_t *buffer,
                                                  uint16_t buffer_length)
{
  sl_status_t    result  = SL_STATUS_FAIL;
  const bool     is_read = ( type == SL_WFX_BUS_READ );
  
  /* send the 2-byte header without DMA */
  HAL_SPI_Transmit(&hspi1, header, header_length, 1000);
  if(is_read)
  {
    if(HAL_SPI_Receive(&hspi1, buffer, buffer_length, 50) == HAL_OK) result = SL_STATUS_OK;
  }else{
    if(HAL_SPI_Transmit(&hspi1, buffer, buffer_length, 50) == HAL_OK) result = SL_STATUS_OK;
  }
  
  return result;
}

sl_status_t sl_wfx_host_enable_platform_interrupt( void )
{
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  return SL_STATUS_OK;
}

sl_status_t sl_wfx_host_disable_platform_interrupt( void )
{
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
  return SL_STATUS_OK;
}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}
