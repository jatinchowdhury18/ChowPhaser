#ifndef PHASESECTION_H_INCLUDED
#define PHASESECTION_H_INCLUDED

#include "JuceHeader.h"

class PhaseSection
{
public:
    PhaseSection() {}

    void reset (double sampleRate)
    {
        for (int stage = 0; stage < numStages; ++stage)
        {
            for (int n = 0; n <= order; ++n)
                z[stage][n] = 0.0f;
        }

        fs = (float) sampleRate;
    }

    inline float processSample (float x)
    {
        for (int stage = 0; stage < numStages; stage++)
            x = processStage (x, stage);

        return x;
    }

    inline float processStage (float x, int stage)
    {
        float y = z[stage][1] + x * b[0];
        z[stage][order] = x * b[order] - y * a[order];

        return y;
    }

    void calcCoefs (float R)
    {
        // component values
        // R = jmax (1.0f, R);
        constexpr float C = (float) 25e-9;
        const float RC = R*C;

        // analog coefs
        const float b0s = RC;
        const float b1s = -1.0f;
        const float a0s = b0s;
        const float a1s = 1.0f;

        // bilinear transform
        const auto K = 2.0f * fs;
        const auto a0 = a0s * K + a1s;
        b[0] = ( b0s * K + b1s) / a0;
        b[1] = (-b0s * K + b1s) / a0;
        a[0] = 1.0f;
        a[1] = (-a0s * K + a1s) / a0;
    }

    static inline float calcPoleFreq (float a, float b, float c)
    {
        auto radicand = b*b - 4.0f*a*c;
        if (radicand >= 0.0f)
            return 0.0f;

        return std::sqrt (-radicand) / (2.0f * a);
    }

private:
    enum
    {
        order = 1,
        numStages = 8,
    };

    float a[order+1] = {1.0f, 0.0f};
    float b[order+1] = {1.0f, 0.0f};
    float z[numStages][order+1];

    float fs = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseSection)
};

#endif // PHASESECTION_H_INCLUDED
