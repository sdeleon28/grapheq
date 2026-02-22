#include "PluginEditor.h"

GraphEQEditor::GraphEQEditor(GraphEQProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    for (int i = 0; i < GraphEQProcessor::numBands; ++i)
    {
        auto& slider = bandSliders[static_cast<size_t>(i)];
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(slider);

        auto paramID = juce::String("band") + juce::String(i);
        bandAttachments[static_cast<size_t>(i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processorRef.apvts, paramID, slider);

        auto& label = bandLabels[static_cast<size_t>(i)];
        label.setText(GraphEQProcessor::bandNames[i], juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::Font(juce::FontOptions(12.0f)));
        addAndMakeVisible(label);
    }

    outputGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(outputGainSlider);

    outputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.apvts, "outputGain", outputGainSlider);

    outputGainLabel.setText("Out", juce::dontSendNotification);
    outputGainLabel.setJustificationType(juce::Justification::centred);
    outputGainLabel.setFont(juce::Font(juce::FontOptions(12.0f)));
    addAndMakeVisible(outputGainLabel);

    setSize(660, 400);
}

void GraphEQEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));

    g.setColour(juce::Colour(0xff16213e));
    auto headerArea = getLocalBounds().removeFromTop(35);
    g.fillRect(headerArea);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(juce::FontOptions(18.0f).withStyle("Bold")));
    g.drawText("GraphEQ", headerArea, juce::Justification::centred);
}

void GraphEQEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(40);
    area.reduce(10, 10);

    auto labelHeight = 20;
    auto labelArea = area.removeFromBottom(labelHeight);
    auto sliderArea = area;

    int totalSlots = GraphEQProcessor::numBands + 1; // +1 for output gain
    int slotWidth = area.getWidth() / totalSlots;

    for (int i = 0; i < GraphEQProcessor::numBands; ++i)
    {
        auto sliderBounds = sliderArea.removeFromLeft(slotWidth);
        bandSliders[static_cast<size_t>(i)].setBounds(sliderBounds);

        auto labelBounds = labelArea.removeFromLeft(slotWidth);
        bandLabels[static_cast<size_t>(i)].setBounds(labelBounds);
    }

    // Output gain in remaining space
    auto outSliderBounds = sliderArea.removeFromLeft(slotWidth);
    outputGainSlider.setBounds(outSliderBounds);

    auto outLabelBounds = labelArea.removeFromLeft(slotWidth);
    outputGainLabel.setBounds(outLabelBounds);
}
