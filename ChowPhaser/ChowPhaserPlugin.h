#ifndef CHOWPHASERPLUGIN_H_INCLUDED
#define CHOWPHASERPLUGIN_H_INCLUDED

#include "JuceHeader.h"
#include "PluginBase.h"

#include "FBSection.h"

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
    std::atomic<float>* freqParam = nullptr;
    std::atomic<float>* depthParam = nullptr;
    std::atomic<float>* lfoFreqParam = nullptr;
    std::atomic<float>* lfoDepthParam = nullptr;

    dsp::Oscillator<float> oscillator[2];
    dsp::Oscillator<float> lfo[2];
    FBSection fbSection[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowPhaser)
};

#endif // CHOWPHASERPLUGIN_H_INCLUDED
