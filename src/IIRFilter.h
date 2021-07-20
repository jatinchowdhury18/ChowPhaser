#ifndef IIRFILTER_H_INCLUDED
#define IIRFILTER_H_INCLUDED

#include "JuceHeader.h"

/** IIR filter of arbirtary order */
template <int order, typename FloatType = float>
class IIRFilterN
{
public:
    IIRFilterN() {}

    virtual void reset()
    {
        std::fill (z, &z[order + 1], 0.0f);
    }

    template <int N = order>
    inline typename std::enable_if<N == 1, FloatType>::type
        processSample (FloatType x) noexcept
    {
        FloatType y = z[1] + x * b[0];
        z[order] = x * b[order] - y * a[order];
        return y;
    }

    template <int N = order>
    inline typename std::enable_if<N == 2, FloatType>::type
        processSample (FloatType x) noexcept
    {
        FloatType y = z[1] + x * b[0];
        z[1] = z[2] + x * b[1] - y * a[1];
        z[order] = x * b[order] - y * a[order];
        return y;
    }

    /** Uses Transposed Direct Form II */
    template <int N = order>
    inline typename std::enable_if<(N > 2), FloatType>::type
        processSample (FloatType x) noexcept
    {
        FloatType y = z[1] + x * b[0];

        for (int i = 1; i < order; ++i)
            z[i] = z[i + 1] + x * b[i] - y * a[i];

        z[order] = x * b[order] - y * a[order];

        return y;
    }

    virtual void processBlock (FloatType* block, const int numSamples) noexcept
    {
        for (int n = 0; n < numSamples; ++n)
            block[n] = processSample (block[n]);
    }

    /** Set coefficients to new values */
    virtual void setCoefs (const FloatType (&newB)[order + 1], const FloatType (&newA)[order + 1])
    {
        std::copy (newB, &newB[order + 1], b);
        std::copy (newA, &newA[order + 1], a);
    }

protected:
    FloatType a[order + 1];
    FloatType b[order + 1];
    FloatType z[order + 1];

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRFilterN)
};

#endif // IIRFILTER_H_INCLUDED
