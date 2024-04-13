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
// TODO:"Limpar" valores ADC (IMPLEMENTAR LINE)

float expMap[4096];

Phasor phs;
Oscillator osc;
Oscillator lfo;
Metro clock;
Adsr env;
bool gate;
int counter = 0;
int button_raw;
int raw_out;
int adc1_raw;
int adc2_raw;
int adc3_raw;
float adc1_normalized;
float adc2_normalized;
float adc3_normalized;
float amp = 0.5;
float base_freq = 220.0;
float lfo_freq = 1.0;
float lfo_amp = 0.5;
bool button_state = false;
float adc_reading = 0;
float target = 400;

int multisample(int samples, adc1_channel_t channel)
{
    int sum = 0;
    for (int i = 0; i < samples; i++)
    {
        sum += adc1_get_raw(channel);
    }
    return sum / samples;
}

int multisample2(int samples, adc2_channel_t channel)
{
    int sum = 0;
    for (int i = 0; i < samples; i++)
    {
        sum += adc2_get_raw(channel, ADC_WIDTH_BIT_12, &raw_out);
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

    env_out = env.Process(gate);
    osc.SetAmp(env_out * amp);

    if (raw_out < 2000)
    {
        gate = true;
    }
    else
    {
        gate = false;
    }

    osc.SetFreq(adc_reading);
    // lfo.SetFreq(lfo_freq * 20);
    // lfo.SetAmp(lfo_amp);
    // lfo.Process();
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
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_12);
    adc2_config_channel_atten(ADC2_CHANNEL_5, ADC_ATTEN_DB_12);

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
            counter++;
        }
        else
        {
            adc1_raw = multisample(24, ADC1_CHANNEL_0);
            adc1_normalized = normalize(adc1_raw, 4095);
            adc2_raw = multisample(24, ADC1_CHANNEL_3);
            adc2_normalized = normalize(adc2_raw, 4095);
            adc3_raw = multisample(128, ADC1_CHANNEL_7);
            adc3_normalized = normalize(adc3_raw, 4095);
            base_freq = 30 + (adc3_normalized * 9000);
            amp = adc1_normalized;
            env.SetReleaseTime(0.01 + (adc2_normalized * 2));
            button_raw = multisample2(32, ADC2_CHANNEL_5);
            adc_reading = lerp(adc_reading, adc3_raw, 0.5);

            printf("Normalized: %d\n", int(adc_reading));
            //  printf("Gate: %d\n", gate);
            //   adc2_raw = multisample(32, ADC1_CHANNEL_4);
            //   adc2_normalized = normalize(adc2_raw, 4095);
            //   lfo_amp = (expMap[(int)(adc2_normalized * 4090)]);

            counter = 0;
        }
    }
}
