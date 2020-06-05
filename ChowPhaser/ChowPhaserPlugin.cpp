#include "ChowPhaserPlugin.h"

namespace
{
    const String fbTag = "feedback";
    const String freqTag = "freq";
    const String depthTag = "depth";
    const String lfoFreqTag = "lfo_freq";
    const String lfoDepthTag = "lfo_depth";
}

ChowPhaser::ChowPhaser()
{
    fbParam    = vts.getRawParameterValue (fbTag);
    freqParam  = vts.getRawParameterValue (freqTag);
    depthParam = vts.getRawParameterValue (depthTag);
    lfoFreqParam  = vts.getRawParameterValue (lfoFreqTag);
    lfoDepthParam = vts.getRawParameterValue (lfoDepthTag);

    for (int ch = 0; ch < 2; ++ch)
    {
        oscillator[ch].initialise ([] (float x) { return std::sin (x); });
        lfo[ch].initialise ([] (float x) { return std::sin (x); });
    }
}

void ChowPhaser::addParameters (Parameters& params)
{
    NormalisableRange<float> freqRange (60.0f, 11000.0f);
    freqRange.setSkewForCentre (1000.0f);

    NormalisableRange<float> depthRange (0.0f, 2000.0f);
    depthRange.setSkewForCentre (500.0f);

    NormalisableRange<float> lfoRange (0.0f, 10.0f);
    lfoRange.setSkewForCentre (1.0f);

    params.push_back (std::make_unique<AudioParameterFloat> (fbTag, "Feedback", 0.0f, 0.9f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (freqTag, "Freq", freqRange, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (depthTag, "Depth", depthRange, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (lfoFreqTag, "LFO Freq", lfoRange, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (lfoDepthTag, "LFO Depth", 0.0f, 1.0f, 0.0f));
}

void ChowPhaser::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
    {
        oscillator[ch].prepare ({ sampleRate, (uint32) samplesPerBlock, 1 });
        lfo[ch].prepare ({ sampleRate, (uint32) samplesPerBlock, 1 });
        fbSection[ch].reset (sampleRate);
    }
}

void ChowPhaser::releaseResources()
{

}

void ChowPhaser::processBlock (AudioBuffer<float>& buffer)
{
    auto depth = (1.0f + (12000.0f - *freqParam) / 12000.0f) * *depthParam;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        lfo[ch].setFrequency (*lfoFreqParam);

        auto* x = buffer.getWritePointer (ch);
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            oscillator[ch].setFrequency (*freqParam + lfo[ch].processSample (0.0f) * *lfoDepthParam * *freqParam);
            
            float rVal = oscillator[ch].processSample (0.0f) * depth + (depth + 1.0f);
            fbSection[ch].calcCoefs (rVal, -1.0f * *fbParam);
            x[n] = fbSection[ch].processSample (x[n]) * (0.99f * std::pow (1.0f - *fbParam, 0.5f) + 0.01f);
        }
    }
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
