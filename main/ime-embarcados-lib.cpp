#include "driver/i2s.h"
#include <math.h>
#include "config.h"
#include "Phasor.h"
#include "Oscillator.h"
#include "Metro.h"

static const uint16_t Volume = 1023;
static uint32_t Value32Bit;
size_t BytesWritten;
// TODO: Atualizar driver i2s

Phasor phs;
Oscillator osc;
Metro clock;

void audio_callback()
{
    uint8_t tic;
    float freq;
    tic = clock.Process();
    if (tic)
    {
        freq = rand() % 500;
        osc.SetFreq(freq);
    }

    int16_t OutputValue = (int16_t)(osc.Process() * Volume);
    Value32Bit = (OutputValue << 16) | (OutputValue & 0xffff);
    i2s_write(i2s_num, &Value32Bit, 4, &BytesWritten, portMAX_DELAY);
}

extern "C" void app_main(void)
{
    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin(i2s_num, &pin_config);
    phs.Init(44100, 0.0105);
    osc.Init(44100);
    osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    clock.Init(10, 44100);

    while (1)
    {
        audio_callback();
    }
}
