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

    dsp::Oscillator<float> lfo;
    FBSection fbSection;
    PhaseSection phaseSection;

    AudioBuffer<float> monoBuffer;
    AudioBuffer<float> noModBuffer;

    SmoothedValue<float, ValueSmoothingTypes::Linear> depthSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> fbSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> modSmooth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowPhaser)
};

#endif // CHOWPHASERPLUGIN_H_INCLUDED
