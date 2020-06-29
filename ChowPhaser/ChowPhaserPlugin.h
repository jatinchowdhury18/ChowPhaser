#ifndef CHOWPHASERPLUGIN_H_INCLUDED
#define CHOWPHASERPLUGIN_H_INCLUDED

#include "JuceHeader.h"
#include "PluginBase.h"

#include "FBSection.h"
#include "PhaseSection.h"

class ChowPhaser : public PluginBase<ChowPhaser>
{
public:
    ChowPhaser();

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

    AudioProcessorEditor* createEditor() override;

private:
    std::atomic<float>* fbParam = nullptr;
    std::atomic<float>* modParam = nullptr;
    std::atomic<float>* lfoFreqParam = nullptr;
    std::atomic<float>* lfoDepthParam = nullptr;
    std::atomic<float>* freqMultParam = nullptr;
    std::atomic<float>* skewParam = nullptr;
    std::atomic<float>* stagesParam = nullptr;

    dsp::Oscillator<float> lfo;
    FBSection fbSection;
    PhaseSection phaseSection;

    AudioBuffer<float> monoBuffer;
    AudioBuffer<float> noModBuffer;

    SmoothedValue<float, ValueSmoothingTypes::Linear> depthSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> fbSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> modSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> skewSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> stagesSmooth;

    AudioBuffer<float> scopeBuffer;
    foleys::MagicPlotSource* scope = nullptr;

    inline float lightShape (float x, float skewPow)
    {
        return (std::pow ((x + 1.0f) / 2.0f, skewPow) * 2.0f) - 1.0f;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowPhaser)
};

#endif // CHOWPHASERPLUGIN_H_INCLUDED
