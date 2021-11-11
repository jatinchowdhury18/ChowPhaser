#ifndef CHOWPHASERPLUGIN_H_INCLUDED
#define CHOWPHASERPLUGIN_H_INCLUDED

#include "PluginBase.h"
#include "SingleChannelPhaser.h"
#include "gui/MyLNF.h"
#include <JuceHeader.h>

class ChowPhaserStereo : public PluginBase<ChowPhaserStereo>
{
public:
    ChowPhaserStereo();

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    AudioProcessorEditor* createEditor() override;

private:
    std::unique_ptr<SingleChannelPhaser> phasers[2];
    AudioBuffer<float> noModBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowPhaserStereo)
};

#endif // CHOWPHASERPLUGIN_H_INCLUDED
