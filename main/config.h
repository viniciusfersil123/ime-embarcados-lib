#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <math.h>
#include "esp_check.h"
#include "driver/dac_continuous.h"

// DAC handle and small staging buffer for efficient DMA writes
static dac_continuous_handle_t s_dac = nullptr;
static uint8_t s_dac_buf[256];
static size_t s_dac_idx = 0;

// Initialize DAC continuous mode on GPIO25 (DAC channel 0) at the given sample rate
void audio_init(uint32_t& sample_rate)
{
    dac_continuous_config_t cfg = {
        .chan_mask = DAC_CHANNEL_MASK_CH0, // GPIO25 on ESP32
        .desc_num = 4,                     // number of DMA descriptors
        .buf_size = 512,                   // bytes per descriptor (total = desc_num * buf_size)
        .freq_hz = sample_rate,            // sample rate
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };

    ESP_ERROR_CHECK(dac_continuous_new_channels(&cfg, &s_dac));
    ESP_ERROR_CHECK(dac_continuous_enable(s_dac));
}

// Convert stereo float [-1.0, 1.0] to mono 8-bit [0,255] and stream to DAC via DMA
void to_audio_write(float left_channel, float right_channel)
{
    // Mix to mono and apply gentle attenuation to avoid clipping
    float mono = 0.5f * (left_channel + right_channel);
    if (mono > 1.0f) mono = 1.0f;
    if (mono < -1.0f) mono = -1.0f;

    // Map [-1,1] -> [0,255]
    float scaled = (mono * 0.5f + 0.5f) * 255.0f;
    uint8_t sample = (scaled < 0.0f) ? 0 : (scaled > 255.0f ? 255 : (uint8_t)lroundf(scaled));

    // Stage sample into buffer; flush when full
    s_dac_buf[s_dac_idx++] = sample;
    if (s_dac_idx >= sizeof(s_dac_buf)) {
        size_t loaded = 0;
        // Block until all bytes are loaded into DMA
        ESP_ERROR_CHECK(dac_continuous_write(s_dac, s_dac_buf, s_dac_idx, &loaded, -1));
        (void)loaded;
        s_dac_idx = 0;
    }
}

#endif

