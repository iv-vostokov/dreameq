#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <fstream>

class DreamEQAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private ComboBox::Listener    
{
public:
    DreamEQAudioProcessorEditor (DreamEQAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~DreamEQAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
 	void comboBoxChanged(ComboBox*) override;

private:
/*    DreamEQAudioProcessor& audioProcessor;
    juce::Slider cutoffFrequencySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        cutoffFrequencyAttachment;
    juce::Label cutoffFrequencyLabel;
    juce::ToggleButton highpassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        highpassAttachment;
    juce::Label highpassButtonLabel;
*/
	Slider freqSlider;
	Slider qSlider;
	Slider peakGainSlider;
	Slider volumeSlider;
	ComboBox filterChoice;
	ComboBox biquadChoice;

	std::ofstream bufferFile;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
	DreamEQAudioProcessor& processor;

public:
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> freqSliderVal;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> qSliderVal;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> volSliderVal;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> peakGainSliderVal;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> filterChoiceVal;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> biquadChoiceVal;
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DreamEQAudioProcessorEditor)
};