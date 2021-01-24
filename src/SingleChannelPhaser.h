#ifndef SINGLECHANNELPHASER_H_INCLUDED
#define SINGLECHANNELPHASER_H_INCLUDED

#include <JuceHeader.h>

#include "FBSection.h"
#include "PhaseSection.h"

using Parameters = std::vector<std::unique_ptr<RangedAudioParameter>>;

class SingleChannelPhaser
{
public:
    SingleChannelPhaser (foleys::MagicProcessorState& magicState, String prefix = "");

    static void addParameters (Parameters& params, String prefix = "", float modRange = 0.0f);
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
    void processBlock (const float* input, float* modOut, float* noModOut, const int numSamples);

private:
    std::atomic<float>* fbParam = nullptr;
    std::atomic<float>* modParam = nullptr;
    std::atomic<float>* lfoFreqParam = nullptr;
    std::atomic<float>* lfoDepthParam = nullptr;
    std::atomic<float>* freqMultParam = nullptr;
    std::atomic<float>* skewParam = nullptr;
    std::atomic<float>* stagesParam = nullptr;
    std::atomic<float>* d1Param = nullptr;
    std::atomic<float>* d2Param = nullptr;
    std::atomic<float>* d3Param = nullptr;

    dsp::Oscillator<float> lfo;
    FBSection fbSection;
    PhaseSection phaseSection;

    SmoothedValue<float, ValueSmoothingTypes::Linear> depthSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> fbSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> modSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> skewSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> stagesSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> d1Smooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> d2Smooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> d3Smooth;

    AudioBuffer<float> scopeBuffer;
    foleys::MagicPlotSource* scope = nullptr;

    inline float lightShape (float x, float skewPow) noexcept
    {
        return (std::pow ((x + 1.0f) / 2.0f, skewPow) * 2.0f) - 1.0f;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SingleChannelPhaser)
};

#endif // SINGLECHANNELPHASER_H_INCLUDED
