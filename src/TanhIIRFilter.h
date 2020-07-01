#ifndef TANHIIRFILTER_H_INCLUDED
#define TANHIIRFILTER_H_INCLUDED

#include "IIRFilter.h"

template<int order, typename FloatType=float>
class TanhIIRFilter
{
public:
    TanhIIRFilter() {}

    virtual void reset()
    {
        std::fill (z, &z[order+1], 0.0f);
    }

    template <int N = order>
    inline typename std::enable_if <N == 2, FloatType>::type
    processSampleTanh (FloatType x, float d1, float d2, float d3) noexcept
    {
        FloatType y = z[1] + x * b[0];
        auto yDrive = drive (y, d3);
        z[1] = drive (z[2] + x * b[1] - yDrive * a[1], d1);
        z[order] = drive (x * b[order] - yDrive * a[order], d2);
        return y;
    }

    inline FloatType drive (FloatType x, FloatType drive) noexcept
    {
        return std::tanh (x * drive) / drive;
    }

protected:
    FloatType a[order+1];
    FloatType b[order+1];
    FloatType z[order+1];

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TanhIIRFilter)
};

#endif // TANHIIRFILTER_H_INCLUDED
