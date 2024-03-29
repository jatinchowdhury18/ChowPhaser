#include "ChowPhaserPlugin.h"
#include "gui/InfoComp.h"
#include "gui/LightMeter.h"
#include "gui/TitleComp.h"
#include "gui/TooltipComp.h"

ChowPhaser::ChowPhaser() : phaser (magicState)
{
}

void ChowPhaser::addParameters (Parameters& params)
{
    SingleChannelPhaser::addParameters (params, "", -1.0f);
}

void ChowPhaser::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    phaser.prepareToPlay (sampleRate, samplesPerBlock);

    monoBuffer.setSize (1, samplesPerBlock);
    noModBuffer.setSize (1, samplesPerBlock);
}

void ChowPhaser::releaseResources()
{
}

void ChowPhaser::processBlock (AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    monoBuffer.setSize (1, numSamples, false, false, true);
    noModBuffer.setSize (1, numSamples, false, false, true);
    monoBuffer.clear();

    // sum to mono
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        monoBuffer.addFrom (0, 0, buffer.getReadPointer (ch), numSamples, 1.0f / (float) buffer.getNumChannels());

    auto* modParam = vts.getRawParameterValue ("mod");
    const auto modChannel = int (*modParam >= 0.0f);
    auto* monoPtr = monoBuffer.getWritePointer (0);
    auto* noModPtr = noModBuffer.getWritePointer (0);
    auto* yMod = buffer.getWritePointer (modChannel);

    phaser.processBlock (monoPtr, yMod, noModPtr, numSamples);

    buffer.copyFrom (1 - modChannel, 0, noModBuffer.getReadPointer (0), numSamples);
}

bool ChowPhaser::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // only supports stereo
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // input and output layout must be the same
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

AudioProcessorEditor* ChowPhaser::createEditor()
{
    auto builder = std::make_unique<foleys::MagicGUIBuilder> (magicState);
    builder->registerJUCEFactories();
    builder->registerJUCELookAndFeels();

    builder->registerLookAndFeel ("MyLNF", std::make_unique<MyLNF>());
    builder->registerFactory ("TooltipComp", &TooltipItem::factory);
    builder->registerFactory ("InfoComp", &InfoItem::factory);
    builder->registerFactory ("TitleComp", &TitleItem::factory);

    return new foleys::MagicPluginEditor (magicState, BinaryData::gui_xml, BinaryData::gui_xmlSize, std::move (builder));
}

// This creates new instances of the plugin...
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowPhaser();
}
