#include "driver/i2s.h"
#include "config.h"
#include "../DaisySP/Source/daisysp.h"

static const uint16_t Volume = 1023;
static uint32_t Value32Bit;
size_t BytesWritten;

static daisysp::Oscillator osc, lfo;


void audio_callback()
{
    osc.SetFreq(440 + lfo.Process() * 100);
    int16_t OutputValue = (int16_t)(osc.Process() * Volume);
    Value32Bit = (OutputValue << 16) | (OutputValue & 0xffff);
    i2s_write(i2s_num, &Value32Bit, 4, &BytesWritten, portMAX_DELAY);
}

extern "C" void app_main(void)
{
    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin(i2s_num, &pin_config);
    osc.Init(44100);
    lfo.Init(44100);
    lfo.SetFreq(0.5);
    osc.SetFreq(440);
    osc.SetWaveform(daisysp::Oscillator::WAVE_POLYBLEP_SAW);

    while (1)
    {
        audio_callback();
    }
}
