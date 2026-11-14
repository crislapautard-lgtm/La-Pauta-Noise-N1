#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "suppress", 1 }, "Suppress",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 41.0f));
    return layout;
}

void PluginProcessor::prepareToPlay (double sampleRate, int)
{
    sampleRateHz = sampleRate;
    envelope = 0.0f;
    smoothedGain = 1.0f;
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();
    if (in != out) return false;
    return in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo();
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const float amount = apvts.getRawParameterValue ("suppress")->load();

    const float atk = std::exp (-1.0f / (0.001f * (float) sampleRateHz));   // 1 ms
    const float rel = std::exp (-1.0f / (0.120f * (float) sampleRateHz));   // 120 ms
    const float gainClose = std::exp (-1.0f / (0.040f * (float) sampleRateHz)); // 40 ms
    const float gainOpen  = std::exp (-1.0f / (0.003f * (float) sampleRateHz)); // 3 ms

    // El fader controla el umbral: mas alto = mas supresion
    const float thresholdDb = -60.0f + 0.5f * amount;
    const float ratio = 4.0f;
    const bool active = amount > 0.5f;

    float blockPeak = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        float peak = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            peak = juce::jmax (peak, std::abs (buffer.getReadPointer (ch)[i]));

        blockPeak = juce::jmax (blockPeak, peak);

        envelope = (peak > envelope) ? atk * envelope + (1.0f - atk) * peak
                                     : rel * envelope + (1.0f - rel) * peak;

        float targetGain = 1.0f;
        if (active)
        {
            const float envDb = juce::Decibels::gainToDecibels (envelope, -120.0f);
            if (envDb < thresholdDb)
            {
                const float reductionDb = juce::jmax (-60.0f, (envDb - thresholdDb) * (ratio - 1.0f));
                targetGain = juce::Decibels::decibelsToGain (reductionDb);
            }
        }

        const float coeff = (targetGain < smoothedGain) ? gainClose : gainOpen;
        smoothedGain = coeff * smoothedGain + (1.0f - coeff) * targetGain;

        for (int ch = 0; ch < numChannels; ++ch)
            buffer.getWritePointer (ch)[i] *= smoothedGain;
    }

    inputLevelDb.store (juce::Decibels::gainToDecibels (blockPeak, -100.0f));
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
