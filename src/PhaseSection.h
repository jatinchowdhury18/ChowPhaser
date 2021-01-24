#ifndef PHASESECTION_H_INCLUDED
#define PHASESECTION_H_INCLUDED

#include "JuceHeader.h"

class PhaseSection
{
public:
    PhaseSection() {}

    void reset (double sampleRate)
    {
        std::fill (z, &z[maxNumStages], 0.0f);
        fs = (float) sampleRate;
    }

    inline float processSample (float x, float numStages)
    {
        // process integer stages
        for (int stage = 0; stage < (int) numStages; stage++)
            x = processStage (x, stage);

        // process fractional stage
        float stageFrac = numStages - (int) numStages;
        x = stageFrac * processStage (x, (int) numStages) + (1.0f - stageFrac) * x;

        return x;
    }

    inline float processStage (float x, int stage)
    {
        float y = z[stage] + x * b[0];
        z[stage] = x * b[order] - y * a[order];

        return y;
    }

    void calcCoefs (float R)
    {
        // component values
        // R = jmax (1.0f, R);
        constexpr float C = (float) 25e-9;
        const float RC = R * C;

        // analog coefs
        const float b0s = RC;
        const float b1s = -1.0f;
        const float a0s = b0s;
        const float a1s = 1.0f;

        // bilinear transform
        const auto K = 2.0f * fs;
        const auto a0 = a0s * K + a1s;
        b[0] = (b0s * K + b1s) / a0;
        b[1] = (-b0s * K + b1s) / a0;
        a[0] = 1.0f;
        a[1] = (-a0s * K + a1s) / a0;
    }

private:
    enum
    {
        order = 1,
        maxNumStages = 52, // must be 2 greater than the parameter allows
    };

    float a[order + 1] = { 1.0f, 0.0f };
    float b[order + 1] = { 1.0f, 0.0f };
    float z[maxNumStages];

    float fs = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseSection)
};

#endif // PHASESECTION_H_INCLUDED
