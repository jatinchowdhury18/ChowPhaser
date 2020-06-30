#ifndef TANHIIRFILTER_H_INCLUDED
#define TANHIIRFILTER_H_INCLUDED

#include "IIRFilter.h"

template<int order, typename FloatType=float>
class TanhIIRFilter : public IIRFilterN<order, FloatType>
{
public:
    TanhIIRFilter() {}

    template <int N = order>
    inline typename std::enable_if <N == 1, FloatType>::type
    processSampleTanh (FloatType x) noexcept
    {
        FloatType y = z[1] + x * b[0];
        z[order] =  std::tanh (x * b[order] - y * a[order]);
        return y;
    }

    template <int N = order>
    inline typename std::enable_if <N == 2, FloatType>::type
    processSampleTanh (FloatType x) noexcept
    {
        FloatType y = z[1] + x * b[0];
        z[1] = std::tanh (z[2] + x * b[1] - y * a[1]);
        z[order] = std::tanh (x * b[order] - y * a[order]);
        return y;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TanhIIRFilter)
};

#endif // TANHIIRFILTER_H_INCLUDED
