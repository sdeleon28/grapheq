#include "PluginProcessor.h"
#include "PluginEditor.h"

const juce::StringArray GraphEQProcessor::bandNames = {
    "31 Hz", "62 Hz", "125 Hz", "250 Hz", "500 Hz",
    "1 kHz", "2 kHz", "4 kHz", "8 kHz", "16 kHz"
};

GraphEQProcessor::GraphEQProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    for (size_t i = 0; i < numBands; ++i)
    {
        filtersL[i] = std::make_unique<IIRFilter>();
        filtersR[i] = std::make_unique<IIRFilter>();
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout GraphEQProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    for (int i = 0; i < numBands; ++i)
    {
        auto paramID = juce::String("band") + juce::String(i);
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { paramID, 1 },
            bandNames[i],
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; },
            nullptr));
    }

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "outputGain", 1 },
        "Output Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " dB"; },
        nullptr));

    return { params.begin(), params.end() };
}

void GraphEQProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;

    for (size_t i = 0; i < numBands; ++i)
    {
        filtersL[i]->prepare(spec);
        filtersR[i]->prepare(spec);
    }

    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    outputGain.prepare(spec);

    updateFilters();
}

void GraphEQProcessor::updateFilters()
{
    constexpr float Q = 1.4f;

    for (size_t i = 0; i < numBands; ++i)
    {
        auto paramID = juce::String("band") + juce::String(static_cast<int>(i));
        float gainDB = apvts.getRawParameterValue(paramID)->load();

        auto coefs = IIRCoefs::makePeakFilter(currentSampleRate,
                                               bandFrequencies[i],
                                               Q,
                                               juce::Decibels::decibelsToGain(gainDB));
        *filtersL[i]->coefficients = *coefs;
        *filtersR[i]->coefficients = *coefs;
    }

    float outGainDB = apvts.getRawParameterValue("outputGain")->load();
    outputGain.setGainDecibels(outGainDB);
}

void GraphEQProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateFilters();

    auto numSamples = buffer.getNumSamples();

    // Process left channel
    if (totalNumInputChannels > 0)
    {
        auto* channelData = buffer.getWritePointer(0);
        juce::dsp::AudioBlock<float> blockL(&channelData, 1, static_cast<size_t>(numSamples));
        juce::dsp::ProcessContextReplacing<float> ctxL(blockL);
        for (int i = 0; i < numBands; ++i)
            filtersL[static_cast<size_t>(i)]->process(ctxL);
    }

    // Process right channel
    if (totalNumInputChannels > 1)
    {
        auto* channelData = buffer.getWritePointer(1);
        juce::dsp::AudioBlock<float> blockR(&channelData, 1, static_cast<size_t>(numSamples));
        juce::dsp::ProcessContextReplacing<float> ctxR(blockR);
        for (int i = 0; i < numBands; ++i)
            filtersR[static_cast<size_t>(i)]->process(ctxR);
    }

    // Apply output gain
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    outputGain.process(context);
}

void GraphEQProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GraphEQProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* GraphEQProcessor::createEditor()
{
    return new GraphEQEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GraphEQProcessor();
}
