#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class DreamEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DreamEQAudioProcessorEditor (DreamEQAudioProcessor&);
    ~DreamEQAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    [[maybe_unused]] DreamEQAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DreamEQAudioProcessorEditor)
};