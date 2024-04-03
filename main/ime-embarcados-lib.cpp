#include "driver/i2s.h"
#include <math.h>
#include "config.h"
#include "Phasor.h"
#include "Oscillator.h"
#include "Metro.h"
#include "Adsr.h"
#include "driver/adc.h"

static const uint16_t Volume = 1023;
static uint32_t Value32Bit;
size_t BytesWritten;
// TODO: Atualizar driver i2s
// TODO: Atualizar driver adc
// TODO: implementar FREERTOS
// TODO:"Limpar" valores ADC

Phasor phs;
Oscillator osc;
Metro clock;
Adsr env;
bool gate;
int counter = 0;
int adc_raw;
float adc_normalized;

int multisample(int samples)
{
    int sum = 0;
    for (int i = 0; i < samples; i++)
    {
        sum += adc1_get_raw(ADC1_CHANNEL_4);
    }
    return sum / samples;
}

float normalize(int raw, int max_raw)
{
    return (float)raw / (float)max_raw;
}

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
    env.SetAttackTime(0.05);
    env.SetDecayTime(0.1);
    env.SetSustainLevel(1);
    env.SetReleaseTime(0.5);
    clock.Init(1, 44100);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);

    while (1)
    {
        audio_callback();
        if (counter < 1000)
        {
            // vTaskDelay(100 / portTICK_PERIOD_MS);

            // printf("Normalized: %.3f\n", adc_normalized);
            counter++;
        }
        else
        {
            adc_raw = multisample(128);
            adc_normalized = normalize(adc_raw, 4095);
            counter = 0;
        }
        osc.SetFreq(int(880 * adc_normalized));
    }
}
