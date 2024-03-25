#include "driver/i2s.h"
#include <math.h>
#include "config.h"
#include "Phasor.h"
#include "Oscillator.h"

static const uint16_t Volume = 1023;
static uint32_t Value32Bit;
size_t BytesWritten;
// TODO: Atualizar driver i2s

Phasor phs;
Phasor phs2;
Phasor phs3;
Oscillator osc;
Oscillator osc2;
Oscillator osc3;

void audio_callback()
{
    int16_t OutputValue = (int16_t)(osc.Process() * Volume) + (int16_t)(osc2.Process() * Volume) + (int16_t)(osc3.Process() * Volume);
    OutputValue = OutputValue / 3;
    Value32Bit = (OutputValue << 16) | (OutputValue & 0xffff);
    i2s_write(i2s_num, &Value32Bit, 4, &BytesWritten, portMAX_DELAY);
}

extern "C" void app_main(void)
{
    // Generate the sine wave before entering the loop
    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin(i2s_num, &pin_config);
    phs.Init(44100, 0.0105);
    phs2.Init(44100, 0.01);
    phs3.Init(44100, 0.01025);
    osc.Init(44100);
    osc2.Init(44100);
    osc3.Init(44100);
    osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    osc2.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    osc3.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);

    while (1)
    {
        // Set the frequency (you can change this dynamically as needed)
        osc.SetFreq(phs.Process() * 440);
        osc2.SetFreq((phs2.Process() * 440)*2);
        osc3.SetFreq((phs3.Process() * 440)*3);
        audio_callback();
    }
}
