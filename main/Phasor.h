#ifndef PHASOR_H
#define PHASOR_H

class Phasor
{
public:
    Phasor(){};
    ~Phasor(){};

    inline void Init(float sample_rate, float freq)
    {
        sample_rate_ = sample_rate;
        SetFreq(freq);
    }

    void SetFreq(float freq);

    float Process();

private:
    float freq_;
    float sample_rate_, inc_, phs_;
};

#endif
