#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "config.h"
#include "../DaisySP/Source/daisysp.h"

using namespace daisysp;
Oscillator osc;
Phasor phasor;
uint32_t sr = 48000;
void audio_callback()
{
    float osc_out = osc.Process();
    osc.SetFreq(phasor.Process()*10000);
    to_audio_write(osc_out, osc_out);
}


extern "C" void app_main(void)
{
    osc.Init(sr);
    osc.SetWaveform(osc.WAVE_SIN);
    osc.SetFreq(220);
    osc.SetAmp(1);
    phasor.Init(sr);
    phasor.SetFreq(0.5f);
    audio_init(sr);

    while (1)
    {        
        audio_callback();
    }
}

