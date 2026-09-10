#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "led_strip.h"
#include "led_strip_types.h"

#define LED_STRIP_GPIO  48
#define LED_STRIP_LED_COUNT 1

#define TCPOS_MAIN_STACK_SIZE 4096
#define TCPOS_MAIN_PRIO 20

#include "tcpos.h"
#include "dataQueue.h"
#include "modbus.h"
#include "modbus_stats.h"
#include "monitor.h"

void InitTimer(void);
void RGBLed(uint8_t red, uint8_t blue, uint8_t green);

void app_main(void)
{
    // Initialize TinyCoPos 
    TcposInit();

    // Init timer
    InitTimer();

    // Application specific initializations
    ModbusInit();

    // Start TinyCoPoOS on core 1
    xTaskCreatePinnedToCore((TaskFunction_t)TcposLoop, "TcposLoop", TCPOS_MAIN_STACK_SIZE, NULL, TCPOS_MAIN_PRIO, NULL, 1);

    while (1) {
        for (int i = 0; i < 2; i++)
        {
            for (int s = 0; s < 50; s++)
            {
                // Red
                uint8_t led_red = MonitorReceivedData() ? 50 : 0;
                uint8_t led_green = MonitorTransmittedData() ? 50 : 0;
                uint8_t led_blue = i == 0 ? 50 : 0;
                RGBLed(led_red, led_green, led_blue);

                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        printf("Received %ld, transmitted %ld, tasks %ld\n", ModbusBytesReceived(), ModbusBytesTransmitted(), TcposTasksExecuted());
    }
}

// Init timer

static bool IRAM_ATTR gptimer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    tcpos_timer_tick++;
    return true;
}

void InitTimer(void)
{
    gptimer_handle_t gptimer = NULL;

    ESP_ERROR_CHECK(gptimer_new_timer(&(gptimer_config_t){
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = 1000000,
        }, &gptimer));

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &(gptimer_event_callbacks_t){
            .on_alarm = gptimer_cb
        }, NULL));

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &(gptimer_alarm_config_t){
        .alarm_count = 1000,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    }));

    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

// LED strip init

void RGBLed(uint8_t red, uint8_t blue, uint8_t green)
{
    static led_strip_handle_t led_strip = NULL;
    if (led_strip == NULL)
    {
        ESP_ERROR_CHECK(led_strip_new_rmt_device(
            &(led_strip_config_t){
                .strip_gpio_num = LED_STRIP_GPIO,
                .max_leds = LED_STRIP_LED_COUNT,
                .led_model = LED_MODEL_WS2812,
                .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
            },
            &(led_strip_rmt_config_t){
                .resolution_hz = 10 * 1000 * 1000, // 10MHz
                .flags.with_dma = false,
            }, 
            &led_strip));
    }
    led_strip_set_pixel(led_strip, 0, red, green, blue);
    led_strip_refresh(led_strip);
}