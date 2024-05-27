#include "driver/i2s.h"
#include "config.h"
#include "Phasor.h"
#include "Oscillator.h"

static const uint16_t Volume = 1023;
static uint32_t Value32Bit;
size_t BytesWritten;

Phasor phs;
Oscillator osc;

void audio_callback()
{
    osc.SetFreq(phs.Process() * 2000);
    int16_t OutputValue = (int16_t)(osc.Process() * Volume);
    //Copia os valores para os dois canais
    Value32Bit = (OutputValue << 16) | (OutputValue & 0xffff);
    i2s_write(i2s_num, &Value32Bit, 4, &BytesWritten, portMAX_DELAY);
}

extern "C" void app_main(void)
{
    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin(i2s_num, &pin_config);
    phs.Init(44100,1);
    osc.Init(44100);
    osc.SetWaveform(Oscillator::WAVE_SIN);

    while (1)
    {
        audio_callback();
    }
}
