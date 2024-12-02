#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

static i2s_chan_handle_t tx_handle; // Move tx_handle to global scope

void audio_init(uint32_t& sample_rate)
{
    size_t BytesWritten;

    static const i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(
        I2S_NUM_AUTO,
        I2S_ROLE_MASTER
    );

    static const i2s_std_config_t i2s_config = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_16BIT,
            I2S_SLOT_MODE_STEREO
        ),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_27,
            .ws = GPIO_NUM_26,
            .dout = GPIO_NUM_25,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    i2s_new_channel(&chan_cfg, &tx_handle, NULL);
    i2s_channel_init_std_mode(tx_handle, &i2s_config);
    i2s_channel_enable(tx_handle);
}

// Function to handle audio write logic
void to_audio_write(float left_channel, float right_channel)
{
    // Scale float values to 16-bit integer range
    int16_t LeftChannel = static_cast<int16_t>(left_channel * 32767.0f * 0.5f);
    int16_t RightChannel = static_cast<int16_t>(right_channel * 32767.0f * 0.5f);

    // Create a buffer to hold both channel values
    int16_t ChannelBuffer[2];
    ChannelBuffer[0] = LeftChannel;   // Left channel
    ChannelBuffer[1] = RightChannel; // Right channel

    size_t BytesWritten = 0; // Declare BytesWritten
    // Write the buffer to the I2S channel
    i2s_channel_write(tx_handle, ChannelBuffer, sizeof(ChannelBuffer), &BytesWritten, portMAX_DELAY);
}

#endif

