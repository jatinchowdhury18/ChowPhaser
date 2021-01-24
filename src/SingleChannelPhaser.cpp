#include "SingleChannelPhaser.h"
#include "gui/LightMeter.h"

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
} // namespace

SingleChannelPhaser::SingleChannelPhaser (foleys::MagicProcessorState& magicState, String prefix)
{
    auto& vts = magicState.getValueTreeState();

    fbParam = vts.getRawParameterValue (prefix + fbTag);
    modParam = vts.getRawParameterValue (prefix + modTag);
    lfoFreqParam = vts.getRawParameterValue (prefix + lfoFreqTag);
    lfoDepthParam = vts.getRawParameterValue (prefix + lfoDepthTag);
    freqMultParam = vts.getRawParameterValue (prefix + freqMultTag);
    skewParam = vts.getRawParameterValue (prefix + skewTag);
    stagesParam = vts.getRawParameterValue (prefix + stagesTag);
    d1Param = vts.getRawParameterValue (prefix + d1Tag);
    d2Param = vts.getRawParameterValue (prefix + d2Tag);
    d3Param = vts.getRawParameterValue (prefix + d3Tag);

    lfo.initialise ([] (float x)
                    { return dsp::FastMathApproximations::sin<float> (x); });

    scope = magicState.createAndAddObject<LightMeter> (prefix + "light");
    magicState.addBackgroundProcessing (scope);
}

void SingleChannelPhaser::addParameters (Parameters& params, String prefix, float modRange)
{
    NormalisableRange<float> stagesRange (1.0f, 50.0f);
    stagesRange.setSkewForCentre (12.0f);

    NormalisableRange<float> freqRange (0.0f, 16.0f);
    freqRange.setSkewForCentre (4.0f);

    params.push_back (std::make_unique<AudioParameterFloat> (prefix + fbTag, "Feedback", 0.0f, 0.95f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (prefix + modTag, "Modulation", modRange, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (prefix + lfoFreqTag, "LFO Freq", freqRange, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (prefix + lfoDepthTag, "LFO Depth", 0.0f, 0.95f, 0.0f));
    params.push_back (std::make_unique<AudioParameterBool> (prefix + freqMultTag, "Freq. Mult", false));
    params.push_back (std::make_unique<AudioParameterFloat> (prefix + skewTag, "Skew", -3.0f, 3.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (prefix + stagesTag, "Stages", stagesRange, 8.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (prefix + d1Tag, "D1", 0.1f, 5.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (prefix + d2Tag, "D2", 0.1f, 5.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (prefix + d3Tag, "D3", 0.1f, 5.0f, 1.0f));
}

void SingleChannelPhaser::prepareToPlay (double sampleRate, int samplesPerBlock)
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

    scopeBuffer.setSize (1, samplesPerBlock);
    scope->prepareToPlay (sampleRate, samplesPerBlock);
}

void SingleChannelPhaser::releaseResources()
{
}

void SingleChannelPhaser::processBlock (const float* input, float* modOut, float* noModOut, const int numSamples)
{
    scopeBuffer.setSize (1, numSamples, false, false, true);
    scopeBuffer.clear();

    bool freqMult = (bool) *freqMultParam;
    lfo.setFrequency (*lfoFreqParam * (freqMult ? 10.0f : 1.0f));
    depthSmooth.setTargetValue (freqMult ? lfoDepthParam->load() * 0.8f : lfoDepthParam->load());
    fbSmooth.setTargetValue (*fbParam);
    modSmooth.setTargetValue (std::abs (*modParam));
    skewSmooth.setTargetValue (std::pow (2.0f, *skewParam));
    stagesSmooth.setTargetValue (*stagesParam);
    d1Smooth.setTargetValue (*d1Param);
    d2Smooth.setTargetValue (*d2Param);
    d3Smooth.setTargetValue (*d3Param);

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
        noModOut[n] = fbSection.processSampleTanh (input[n], d1Smooth.getNextValue(), d2Smooth.getNextValue(), d3Smooth.getNextValue());

        phaseSection.calcCoefs (rVal);
        auto modGain = modSmooth.getNextValue();
        modOut[n] = modGain * phaseSection.processSample (noModOut[n], stagesSmooth.getNextValue()) + (1.0f - modGain) * noModOut[n];
    }

    scope->pushSamples (scopeBuffer);
}
