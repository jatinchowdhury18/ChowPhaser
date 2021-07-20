#ifndef CHOWPHASERPLUGIN_H_INCLUDED
#define CHOWPHASERPLUGIN_H_INCLUDED

#include "PluginBase.h"
#include "SingleChannelPhaser.h"
#include "gui/MyLNF.h"
#include <JuceHeader.h>

class ChowPhaser : public PluginBase<ChowPhaser>
{
public:
    ChowPhaser();

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processAudioBlock (AudioBuffer<float>& buffer) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    AudioProcessorEditor* createEditor() override;

private:
    SingleChannelPhaser phaser;

    AudioBuffer<float> monoBuffer;
    AudioBuffer<float> noModBuffer;

    MyLNF myLNF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowPhaser)
};

#endif // CHOWPHASERPLUGIN_H_INCLUDED
