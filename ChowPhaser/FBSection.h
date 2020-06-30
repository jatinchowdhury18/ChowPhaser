#ifndef FBSECTION_H_INCLUDED
#define FBSECTION_H_INCLUDED

#include "TanhIIRFilter.h"
#include "IIRFilter.h"

class FBSection : public TanhIIRFilter<2>
{
public:
    FBSection() {}

    void reset (double sampleRate)
    {
        fs = (float) sampleRate;
        IIRFilterN<2>::reset();
    }

    inline void calcCoefs (float R, float fbAmt)
    {
        // jassert (R > 0.0f);

        // component values
        // R = jmax (1.0f, R);
        constexpr float C = (float) 15e-9;
        const float RC = R*C;

        // analog coefs
        const float b0s = RC * RC;
        const float b1s = -2.0f * RC;
        const float b2s = 1.0f;
        const float a0s = b0s * (1.0f + fbAmt);
        const float a1s = -b1s * (1.0f - fbAmt);
        const float a2s = 1.0f + fbAmt;

        // frequency warping
        const float wc = calcPoleFreq (a0s, a1s, a2s);
        const auto K = wc == 0.0f ? 2.0f * fs : wc / dsp::FastMathApproximations::tan<float> (wc / (2.0f * fs));
        const auto KSq = K * K;

        // bilinear transform
        const float a0 = a0s * KSq + a1s * K + a2s;
        a[0] = a0 / a0;
        a[1] = 2.0f * (a2s - a0s * KSq) / a0;
        a[2] = (a0s * KSq - a1s * K + a2s) / a0;
        b[0] = (b0s * KSq + b1s * K + b2s) / a0;
        b[1] = 2.0f * (b2s - b0s * KSq) / a0;
        b[2] = (b0s * KSq - b1s * K + b2s) / a0;
    }

    static inline float calcPoleFreq (float a, float b, float c)
    {
        auto radicand = b*b - 4.0f*a*c;
        if (radicand >= 0.0f)
            return 0.0f;

        return std::sqrt (-radicand) / (2.0f * a);
    }

private:
    float fs = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FBSection)
};

#endif // FBSECTION_H_INCLUDED
