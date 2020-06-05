#include "ChowPhaserPlugin.h"

namespace
{
    const String fbTag = "feedback";
    const String modTag = "mod";
    const String lfoFreqTag = "lfo_freq";
    const String lfoDepthTag = "lfo_depth";
    const String freqMultTag = "freq_mult";
}

ChowPhaser::ChowPhaser()
{
    fbParam  = vts.getRawParameterValue (fbTag);
    modParam = vts.getRawParameterValue (modTag);
    lfoFreqParam  = vts.getRawParameterValue (lfoFreqTag);
    lfoDepthParam = vts.getRawParameterValue (lfoDepthTag);
    freqMultParam = vts.getRawParameterValue (freqMultTag);

    lfo.initialise ([] (float x) { return dsp::FastMathApproximations::sin<float> (x); });
}

void ChowPhaser::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterFloat> (fbTag, "Feedback", 0.0f, 0.95f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (modTag, "Modulation", -1.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (lfoFreqTag, "LFO Freq", 0.0f, 16.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (lfoDepthTag, "LFO Depth", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterBool> (freqMultTag, "Freq. Mult", false));
}

void ChowPhaser::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lfo.prepare ({ sampleRate, (uint32) samplesPerBlock, 1 });
    fbSection.reset (sampleRate);
    phaseSection.reset (sampleRate);

    depthSmooth.reset (sampleRate, 0.05);
    fbSmooth.reset (sampleRate, 0.05);
    modSmooth.reset (sampleRate, 0.05);

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

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        monoBuffer.addFrom (0, 0, buffer.getReadPointer (ch), numSamples, 1.0f / (float) buffer.getNumChannels());

    lfo.setFrequency (*lfoFreqParam * ((bool) *freqMultParam ? 10.0f : 1.0f));
    depthSmooth.setTargetValue (*lfoDepthParam);
    fbSmooth.setTargetValue (*fbParam);
    modSmooth.setTargetValue (std::abs (*modParam));
    const auto modChannel = int (*modParam >= 0.0f);

    auto* monoPtr = monoBuffer.getWritePointer (0);
    auto* noModPtr = noModBuffer.getWritePointer (0);
    auto* yMod = buffer.getWritePointer (modChannel);

    for (int n = 0; n < numSamples; ++n)
    {
        constexpr float maxDepth = 20.0f;
        auto lightVal = (maxDepth + 0.1f) - (lfo.processSample (0.0f) * depthSmooth.getNextValue() * maxDepth);
        auto rVal = 100000.0f * std::pow (lightVal / 0.1f, -0.75f);

        fbSection.calcCoefs (rVal, -1.0f * fbSmooth.getNextValue());
        noModPtr[n] = fbSection.processSample (monoPtr[n]);
        
        phaseSection.calcCoefs (rVal);
        auto modGain = modSmooth.getNextValue();
        yMod[n] = modGain * phaseSection.processSample (noModPtr[n]) + (1.0f - modGain) * noModPtr[n];
    }

    buffer.copyFrom (1 - modChannel, 0, noModBuffer.getReadPointer (0), numSamples);
}

AudioProcessorEditor* ChowPhaser::createEditor()
{
    return new foleys::MagicPluginEditor (magicState, BinaryData::gui_xml, BinaryData::gui_xmlSize);
}

// This creates new instances of the plugin...
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowPhaser();
}
