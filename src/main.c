#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "esp_err.h"
#include "esp_attr.h"

#define TCPOS_MAIN_STACK_SIZE 4096
#define TCPOS_MAIN_PRIO 20

#include "tcpos.h"

void InitTimer(void);

void app_main(void)
{
    // Initialize TinyCoPos 
    TcposInit();

    // Init timer
    InitTimer();

    // Start TinyCoPoOS on core 1
    xTaskCreatePinnedToCore((TaskFunction_t)TcposLoop, "TcposLoop", TCPOS_MAIN_STACK_SIZE, NULL, TCPOS_MAIN_PRIO, NULL, 1);

    while (1) {
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