#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "sdkconfig.h"
/**
* LEDC (LED Controller) fade example — примеры с затуханием и зажиганием
*
* Пример использования библиотеки "driver/ledc.h" для работы с ШИМ.
*/

/*
* Об этом примере
*
* 1. Начинаем с инициализацией светодиодов:
* a. Вначале настраиваем "таймер" (timer), для этого указываем частоту и разрешение ШИМ.
* b. Потом настраиваем каналы со светодиодами и связываем их с "таймером".
*
* 2. Потом для инициализации запускаем стартовую функцию, чтобы использовать "АПИ" (API).
*
* 3. Можно так же изменять свечение светодиодов практически без задержки.
*
* 4. В этом примере используются выходы GPIO18/19/4/5 для питания светодиодов,
* и изменения будут происходить циклически.
*
* 5. GPIO18/19 из высокоскоросной канальной группы.
* GPIO4/5 из низкоканальной группы.
*/
// Указатели для высокоскоросной канальной группы.
#define LEDC_HS_TIMER LEDC_TIMER_0
#define LEDC_HS_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO (14)
#define LEDC_HS_CH0_CHANNEL LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO (19)
#define LEDC_HS_CH1_CHANNEL LEDC_CHANNEL_1
// Указатели для низкоканальной канальной группы.
#define LEDC_LS_TIMER LEDC_TIMER_1
#define LEDC_LS_MODE LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH2_GPIO (4)
#define LEDC_LS_CH2_CHANNEL LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO (5)
#define LEDC_LS_CH3_CHANNEL LEDC_CHANNEL_3
// Сколько светодиодов (каналов) будет использоваться в примере
#define LEDC_TEST_CH_NUM (4)
// Максимальная яркость светодиодов в этом примере
#define LEDC_TEST_DUTY (2000)
// Время в миллисекундах в течении которого будет происходить зажигание и затухание
#define LEDC_TEST_FADE_TIME (500)
void app_main()
{
  /*
* Подготавливаем и настраиваем таймер,
* который мы будем использовать в этом примере.
*/
  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_13_BIT, // Разрешение ШИМ-сигнала (resolution of PWM duty)
      .freq_hz = 5000,                      // Частота ШИМ-сигнла
      .speed_mode = LEDC_HS_MODE,           // Режим таймера
      .timer_num = LEDC_HS_TIMER            // Индекс таймера
  };
  // Сохраняем конфигурацию Таймера 0 (timer0) для выскоскоросных каналов
  ledc_timer_config(&ledc_timer);
  // Сохраняем конфигурацию Таймера 1 (timer1) для низкоскоростных каналов
  ledc_timer.speed_mode = LEDC_LS_MODE;
  ledc_timer.timer_num = LEDC_LS_TIMER;
  ledc_timer_config(&ledc_timer);
  /*  
* Подготавливаем отдельные настройки для каждого канала, подключённому к светодиоду:
* — номер канала (channel)
* — выходной сигнал (duty), по умолчанию 0
* — номер GPIO, к которому подключён светодиод (gpio_num)
* — скоростной режим (speed_mode) — выскоскоростные или низкоскоротные
* — индекс таймера, к которому подсоединяется канал (timer_sel)
* Заметка:
* При использовании несколькими каналами одного таймера,
* частота и bit_num этих каналов будут одинаковыми.
*/
  ledc_channel_config_t ledc_channel = {
      .channel = LEDC_HS_CH0_CHANNEL,
      .duty = 0,
      .gpio_num = LEDC_HS_CH0_GPIO,
      .speed_mode = LEDC_HS_MODE,
      .hpoint = 0,
      .timer_sel = LEDC_HS_TIMER};
  ledc_channel_config(&ledc_channel);

  // инициализация службы
  ledc_fade_func_install(0);
  // основной цикл
  while (1)
  {
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 0);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);

    ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, LEDC_TEST_DUTY, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);
    vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);

    ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);
    vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);
  }
}