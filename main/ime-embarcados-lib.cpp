#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "config.h"
#include "../DaisySP/Source/daisysp.h"

#define MAX_DELAY static_cast<size_t>(44100 * 0.75f)

static const uint16_t Volume = 1023 * 3;
static uint32_t Value32Bit;
size_t BytesWritten;
using namespace daisysp;

static Oscillator osc, lfo;
static AdEnv env;
static Metro tick;
static DelayLine<float, MAX_DELAY> del;

void audio_callback()
{
    float osc_out, env_out, feedback, del_out, sig_out;
    if (tick.Process())
    {
        float freq = rand() % 400;
        osc.SetFreq(freq + 100.0f);
        env.Trigger();
    }
    env_out = env.Process();
    osc.SetAmp(env_out);
    osc_out = osc.Process();

    // Read from delay line
    del_out = del.Read();
    // Calculate output and feedback
    sig_out = del_out + osc_out;
    feedback = (del_out * 0.75f) + osc_out;

    // Write to the delay
    del.Write(feedback);

    int16_t OutputValue = sig_out * Volume;
    Value32Bit = (OutputValue << 16) | (OutputValue & 0xffff);
    i2s_channel_write(tx_handle, &Value32Bit, 4, &BytesWritten, portMAX_DELAY);
}

extern "C" void app_main(void)
{
    float sample_rate;

    i2s_new_channel(&chan_cfg, &tx_handle, NULL);
    i2s_channel_init_std_mode(tx_handle, &i2s_config);
    i2s_channel_enable(tx_handle);

    float osc_out, env_out, feedback, del_out, sig_out;
    sample_rate = 48000;
    env.Init(sample_rate);
    osc.Init(sample_rate);
    del.Init();

    // Set up Metro to pulse every second
    tick.Init(1.0f, sample_rate);

    // set adenv parameters
    env.SetTime(ADENV_SEG_ATTACK, 0.001);
    env.SetTime(ADENV_SEG_DECAY, 0.50);
    env.SetMin(0.0);
    env.SetMax(0.25);
    env.SetCurve(0); // linear

    // Set parameters for oscillator
    osc.SetWaveform(osc.WAVE_TRI);
    osc.SetFreq(220);
    osc.SetAmp(0.25);

    // Set Delay time to 0.75 seconds
    del.SetDelay(sample_rate * 0.75f);
    while (1)
    {
        audio_callback();
    }
}