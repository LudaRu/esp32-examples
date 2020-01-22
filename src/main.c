/* ADC1 Example */
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF 1100 //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 64  //Multisampling

void app_main(void)
{ 
  //Check if Two Point or Vref are burned into eFuse
  // check_efuse();

  //Configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11 );

  //Continuously sample ADC1
  while (1)
  {
    printf("%d\n", adc1_get_raw(ADC1_CHANNEL_7));
  }
}