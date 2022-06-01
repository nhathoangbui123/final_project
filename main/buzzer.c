#include "buzzer.h"
void buzzer_init(gpio_num_t gpio_num, uint32_t freq) 
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = PIEZO_MODE,
        .timer_num        = PIEZO_TIMER,
        .duty_resolution  = PIEZO_DUTY_RES,
        .freq_hz          = freq,  
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = PIEZO_MODE,
        .channel        = PIEZO_CHANNEL,
        .timer_sel      = PIEZO_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PIEZO_OUTPUT,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}
void buzzer_play(uint32_t freq, uint32_t volume)
{
    buzzer_init(PIEZO_OUTPUT,freq);
    // Set duty(volume)to 50%. ((2 ** 13) - 1) * 50% = 4095
    ESP_ERROR_CHECK(ledc_set_duty(PIEZO_MODE, PIEZO_CHANNEL, volume));
    ESP_ERROR_CHECK(ledc_update_duty(PIEZO_MODE, PIEZO_CHANNEL));
}
void buzzer_stop(void)
{
    ESP_ERROR_CHECK(ledc_set_duty(PIEZO_MODE, PIEZO_CHANNEL, 0));
    ESP_ERROR_CHECK(ledc_update_duty(PIEZO_MODE, PIEZO_CHANNEL));
}