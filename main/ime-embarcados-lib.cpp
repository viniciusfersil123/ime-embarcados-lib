#include "driver/i2s.h"
#include <math.h>
#include "config.h"
#include "Phasor.h"
#include "Oscillator.h"
#include "Metro.h"
#include "Adsr.h"

static const uint16_t Volume = 1023;
static uint32_t Value32Bit;
size_t BytesWritten;
// TODO: Atualizar driver i2s

Phasor phs;
Oscillator osc;
Metro clock;
Adsr env;
bool gate;

void audio_callback()
{
    float env_out;
    if (clock.Process())
    {
        gate = !gate;
    }

    env_out = env.Process(gate);
    osc.SetAmp(env_out);

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
    env.Init(44100);
    env.SetAttackTime(0.3);
    env.SetDecayTime(0.1);
    env.SetSustainLevel(1);
    env.SetReleaseTime(0.5);
    clock.Init(1, 44100);

    while (1)
    {
        audio_callback();
    }
}
