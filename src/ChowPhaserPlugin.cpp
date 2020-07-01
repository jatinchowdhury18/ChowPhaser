#include "ChowPhaserPlugin.h"
#include "LightMeter.h"

namespace
{
    const String fbTag = "feedback";
    const String modTag = "mod";
    const String lfoFreqTag = "lfo_freq";
    const String lfoDepthTag = "lfo_depth";
    const String freqMultTag = "freq_mult";
    const String skewTag = "skew";
    const String stagesTag = "stages";
    const String d1Tag = "d1";
    const String d2Tag = "d2";
    const String d3Tag = "d3";
}

ChowPhaser::ChowPhaser()
{
    fbParam  = vts.getRawParameterValue (fbTag);
    modParam = vts.getRawParameterValue (modTag);
    lfoFreqParam  = vts.getRawParameterValue (lfoFreqTag);
    lfoDepthParam = vts.getRawParameterValue (lfoDepthTag);
    freqMultParam = vts.getRawParameterValue (freqMultTag);
    skewParam = vts.getRawParameterValue (skewTag);
    stagesParam = vts.getRawParameterValue (stagesTag);
    d1Param = vts.getRawParameterValue (d1Tag);
    d2Param = vts.getRawParameterValue (d2Tag);
    d3Param = vts.getRawParameterValue (d3Tag);

    lfo.initialise ([] (float x) { return dsp::FastMathApproximations::sin<float> (x); });

    scope = magicState.createAndAddObject<LightMeter> ("light");
    magicState.addBackgroundProcessing (scope);
}

void ChowPhaser::addParameters (Parameters& params)
{
    NormalisableRange<float> stagesRange (1.0f, 50.0f);
    stagesRange.setSkewForCentre (12.0f);

    params.push_back (std::make_unique<AudioParameterFloat> (fbTag, "Feedback", 0.0f, 0.95f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (modTag, "Modulation", -1.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (lfoFreqTag, "LFO Freq", 0.0f, 16.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (lfoDepthTag, "LFO Depth", 0.0f, 0.95f, 0.0f));
    params.push_back (std::make_unique<AudioParameterBool> (freqMultTag, "Freq. Mult", false));
    params.push_back (std::make_unique<AudioParameterFloat> (skewTag, "Skew", -3.0f, 3.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (stagesTag, "Stages", stagesRange, 8.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (d1Tag, "D1", 0.1f, 5.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (d2Tag, "D2", 0.1f, 5.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (d3Tag, "D3", 0.1f, 5.0f, 1.0f));
}

void ChowPhaser::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lfo.prepare ({ sampleRate, (uint32) samplesPerBlock, 1 });
    fbSection.reset (sampleRate);
    phaseSection.reset (sampleRate);

    depthSmooth.reset (sampleRate, 0.05);
    fbSmooth.reset (sampleRate, 0.05);
    modSmooth.reset (sampleRate, 0.05);
    skewSmooth.reset (sampleRate, 0.05);
    stagesSmooth.reset (sampleRate, 0.05);
    d1Smooth.reset (sampleRate, 0.05f);
    d2Smooth.reset (sampleRate, 0.05f);
    d3Smooth.reset (sampleRate, 0.05f);

    monoBuffer.setSize (1, samplesPerBlock);
    noModBuffer.setSize (1, samplesPerBlock);

    scopeBuffer.setSize (1, samplesPerBlock);
    scope->prepareToPlay (sampleRate, samplesPerBlock);
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

    scopeBuffer.setSize (1, numSamples, false, false, true);
    scopeBuffer.clear();

    // sum to mono
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        monoBuffer.addFrom (0, 0, buffer.getReadPointer (ch), numSamples, 1.0f / (float) buffer.getNumChannels());

    bool freqMult = (bool) *freqMultParam;
    lfo.setFrequency (*lfoFreqParam * (freqMult ? 10.0f : 1.0f));
    depthSmooth.setTargetValue (freqMult ? lfoDepthParam->load() * 0.8f : lfoDepthParam->load());
    fbSmooth.setTargetValue (*fbParam);
    modSmooth.setTargetValue (std::abs (*modParam));
    const auto modChannel = int (*modParam >= 0.0f);
    skewSmooth.setTargetValue (std::pow (2.0f, *skewParam));
    stagesSmooth.setTargetValue (*stagesParam);
    d1Smooth.setTargetValue (*d1Param);
    d2Smooth.setTargetValue (*d2Param);
    d3Smooth.setTargetValue (*d3Param);

    auto* monoPtr = monoBuffer.getWritePointer (0);
    auto* noModPtr = noModBuffer.getWritePointer (0);
    auto* yMod = buffer.getWritePointer (modChannel);
    auto* scopePtr = scopeBuffer.getWritePointer (0);

    for (int n = 0; n < numSamples; ++n)
    {
        constexpr float maxDepth = 20.0f;
        auto lfoVal = lfo.processSample (0.0f) * depthSmooth.getNextValue();
        auto lfoShapeVal = lightShape (lfoVal, skewSmooth.getNextValue());
        scopePtr[n] = lfoShapeVal;
        auto lightVal = (maxDepth + 0.1f) - (lfoShapeVal * maxDepth);
        auto rVal = 100000.0f * std::pow (lightVal / 0.1f, -0.75f);

        fbSection.calcCoefs (rVal, -1.0f * fbSmooth.getNextValue());
        noModPtr[n] = fbSection.processSampleTanh (monoPtr[n], d1Smooth.getNextValue(),
                                                   d2Smooth.getNextValue(), d3Smooth.getNextValue());
        
        phaseSection.calcCoefs (rVal);
        auto modGain = modSmooth.getNextValue();
        yMod[n] = modGain * phaseSection.processSample (noModPtr[n], stagesSmooth.getNextValue()) + (1.0f - modGain) * noModPtr[n];
    }

    buffer.copyFrom (1 - modChannel, 0, noModBuffer.getReadPointer (0), numSamples);

    scope->pushSamples (scopeBuffer);
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
