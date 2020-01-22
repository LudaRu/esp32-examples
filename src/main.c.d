// https://github.com/nkolban/esp32-snippets/blob/bf8649ba5df3b154866a814014fd97c027e76263/wifi/fragments/access-point.c
// https://github.com/nkolban/esp32-snippets/blob/master/wifi/fragments/access-point.c
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <lwip/sockets.h>
#include <nvs_flash.h>
#include <stdio.h>
# include <string.h>
#include "driver/ledc.h"


//----------
//  Servos
//----------

const int frequency = 50; // Hz
const int pulse_neutral = 1500; // us
const int bit_resolution = 15;

const int servo1_pin = 16;


int get_duty(int pulse) {
    double pulse_100_percent = 1e6 / frequency;
    double duty_100_percent = 1 << bit_resolution;

    double progress = ((double)pulse) / pulse_100_percent;
    double duty = progress * duty_100_percent;

    return (int)duty;
}


void set_up_servo(int channel, int pin) {
    // configure channel
    ledc_channel_config_t channel_config = {
        .channel = channel,
        .duty = get_duty(pulse_neutral),
        .gpio_num = pin,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&channel_config);
}


void set_up_servos() {
    // configure timer
    ledc_timer_config_t timer_config = {
        .bit_num = bit_resolution,
        .freq_hz = frequency,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&timer_config);

    // configure servos
    set_up_servo(0, servo1_pin);
}


void set_pulse(int channel, int pulse) {
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, get_duty(pulse));
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}


//--------
//  WiFi
//--------

const int port = 5555;

esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
    return ESP_OK;
}


void wifi_start_access_point() {
    wifi_config_t wifi_config = {
        .ap = {
            .ssid="_ ESP32 narfa",
            .ssid_len=0,
            .password="arduinoNOT",
            .channel=6,
            .authmode=WIFI_AUTH_WPA2_PSK,
            .ssid_hidden=0,
            .max_connection=4,
            .beacon_interval=100
        }
    };

    tcpip_adapter_init();
    esp_event_loop_init(wifi_event_handler, NULL);
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();
}


//-----------
//  Sockets
//-----------

bool handle_messages(int sock) {
    // remember to return status
    bool ok = true;

    // read message
    int message[1];
    int size_read;
    size_read = recvfrom(sock, message, 1 * 4, 0, NULL, NULL);
    if (size_read <= 0) {
        return !ok;
    }

    // read values
    int pulse1 = message[0];

    // print
    bool debug = true;
    if (debug) {
        printf("%d: %d\n", 0, pulse1);
        fflush(stdout);
        // return ok;
    }

    // move servos
    set_pulse(0, pulse1);
    return ok;
}


void start_socket_server() {
    // create and listen on the socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port)
    };
    bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));

    // handle messages
    while (1) {
        handle_messages(sock);
    }
}

//--------
//  Main
//--------

void app_main() {
    // nvs_flash_init();

    // set_up_servos();
    // wifi_start_access_point();
    // start_socket_server();

    // vTaskDelete(NULL);
}