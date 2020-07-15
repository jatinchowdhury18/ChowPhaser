#include "ChowPhaserStereo.h"
#include "gui/SliderLink.h"

ChowPhaserStereo::ChowPhaserStereo()
{
    phasers[0] = std::make_unique<SingleChannelPhaser> (magicState, "left_");
    phasers[1] = std::make_unique<SingleChannelPhaser> (magicState, "right_");
}

void ChowPhaserStereo::addParameters (Parameters& params)
{
    SingleChannelPhaser::addParameters (params, "left_");
    SingleChannelPhaser::addParameters (params, "right_");
}

void ChowPhaserStereo::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
        phasers[ch]->prepareToPlay (sampleRate, samplesPerBlock);

    noModBuffer.setSize (1, samplesPerBlock);
}

void ChowPhaserStereo::releaseResources()
{
}

void ChowPhaserStereo::processBlock (AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    noModBuffer.setSize (1, numSamples, false, false, true);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* monoPtr = buffer.getWritePointer (ch);
        auto* noModPtr = noModBuffer.getWritePointer (0);
        auto* yMod = buffer.getWritePointer (ch);

        phasers[ch]->processBlock (monoPtr, yMod, noModPtr, numSamples);
    }
}

AudioProcessorEditor* ChowPhaserStereo::createEditor()
{
    auto builder = std::make_unique<foleys::MagicGUIBuilder> (magicState);

    builder->registerJUCEFactories();
    builder->registerJUCELookAndFeels();
    builder->registerFactory ("SliderLink", &foleys::SliderLink::factory);

    return new foleys::MagicPluginEditor (magicState, BinaryData::stereo_gui_xml, BinaryData::stereo_gui_xmlSize, std::move (builder));
}

// This creates new instances of the plugin...
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowPhaserStereo();
}
