#include "PluginProcessor.h"
#include "PluginEditor.h"

DreamEQAudioProcessorEditor::DreamEQAudioProcessorEditor (DreamEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setResizable (true, true);
    setResizeLimits (400, 300, 7680, 4320); // usable minimum up to 8K displays

    setSize (900, 650);
}

DreamEQAudioProcessorEditor::~DreamEQAudioProcessorEditor()
{
}

void DreamEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (18.0f));
    g.drawFittedText ("DreamEQ Plugin", getLocalBounds(), juce::Justification::centred, 1);
}

void DreamEQAudioProcessorEditor::resized()
{
}