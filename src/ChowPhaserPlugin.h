#ifndef CHOWPHASERPLUGIN_H_INCLUDED
#define CHOWPHASERPLUGIN_H_INCLUDED

#include <JuceHeader.h>
#include "PluginBase.h"
#include "SingleChannelPhaser.h"
#include "gui/MyLNF.h"

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
    SingleChannelPhaser phaser;

    AudioBuffer<float> monoBuffer;
    AudioBuffer<float> noModBuffer;
    
    MyLNF myLNF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowPhaser)
};

#endif // CHOWPHASERPLUGIN_H_INCLUDED
