#pragma once

#include "PluginProcessor.h"

class GraphEQEditor : public juce::AudioProcessorEditor
{
public:
    explicit GraphEQEditor(GraphEQProcessor&);
    ~GraphEQEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    GraphEQProcessor& processorRef;

    std::array<juce::Slider, GraphEQProcessor::numBands> bandSliders;
    std::array<juce::Label, GraphEQProcessor::numBands> bandLabels;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>,
               GraphEQProcessor::numBands> bandAttachments;

    juce::Slider outputGainSlider;
    juce::Label outputGainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphEQEditor)
};
