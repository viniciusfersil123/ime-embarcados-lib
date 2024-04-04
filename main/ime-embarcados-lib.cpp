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

float expMap[4096];

Phasor phs;
Oscillator osc;
Oscillator lfo;
Metro clock;
Adsr env;
bool gate;
int counter = 0;
int adc_raw;
float adc_normalized;
float amp = 0.5;
float base_freq = 220.0;
float lfo_freq = 1.0;

int multisample(int samples, adc1_channel_t channel)
{
    int sum = 0;
    for (int i = 0; i < samples; i++)
    {
        sum += adc1_get_raw(channel);
    }
    return sum / samples;
}

float normalize(int raw, int max_raw)
{
    return (float)raw / (float)max_raw;
}

void audio_callback()
{
    /* {
        float env_out;
        if (clock.Process())
        {
            gate = !gate;
        } */

    /*     env_out = env.Process(gate);
        osc.SetAmp(env_out); */
    osc.SetAmp(amp);

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
    lfo.Init(44100);
    lfo.SetWaveform(Oscillator::WAVE_SIN);
    lfo.SetFreq(lfo_freq);
    lfo.SetAmp(0.5);
    env.Init(44100);
    env.SetAttackTime(0.05);
    env.SetDecayTime(0.1);
    env.SetSustainLevel(1);
    env.SetReleaseTime(0.5);
    clock.Init(1, 44100);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);

    // draws a exponential curve that goes from 0 to 1
    for (int i = 0; i < 4095; i++)
    {
        expMap[i] = pow(2, (i - 4095) / 512.0);
    }

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
            adc_raw = multisample(100, ADC1_CHANNEL_5);
            adc_normalized = normalize(adc_raw, 4095);
            // convert base freq to log scale
            base_freq = 30 + 9000 * (expMap[(int)(adc_normalized * 4090)]);
            // printf("Normalized: %.3f\n", base_freq);
            amp = normalize(multisample(100, ADC1_CHANNEL_4), 4095);
            counter = 0;
        }
        osc.SetFreq(base_freq);
        osc.SetAmp(amp);
    }
}
