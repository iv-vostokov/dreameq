#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <fstream>



DreamEQAudioProcessorEditor::DreamEQAudioProcessorEditor (DreamEQAudioProcessor& p,juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processor(p) 
{
 /*   addAndMakeVisible(cutoffFrequencySlider);
    cutoffFrequencySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    cutoffFrequencyAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "cutoff_frequency", cutoffFrequencySlider));

    addAndMakeVisible(cutoffFrequencyLabel);
    cutoffFrequencyLabel.setText("Cutoff Frequency", juce::dontSendNotification);

    addAndMakeVisible(highpassButton);
    highpassAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(vts, "highpass", highpassButton));

    addAndMakeVisible(highpassButtonLabel);
    highpassButtonLabel.setText("Highpass", juce::dontSendNotification);

    setSize(200, 400);
*/
	freqSliderVal = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, FREQ_ID, freqSlider);

	freqSlider.setSliderStyle(Slider::LinearHorizontal);
	freqSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 20);
	freqSlider.setRange(20.0f, 20000.0f);
	addAndMakeVisible(&freqSlider);

	qSliderVal = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, Q_ID, qSlider);

	qSlider.setSliderStyle(Slider::LinearHorizontal);
	qSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 20);
	qSlider.setRange(0.001f, 1.0f);
	addAndMakeVisible(&qSlider);

	volSliderVal = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, VOLUME_ID, volumeSlider);

	volumeSlider.setSliderStyle(Slider::LinearHorizontal);
	volumeSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 20);
	volumeSlider.setRange(-48.0f, 15.0f);
	addAndMakeVisible(&volumeSlider);

	peakGainSliderVal = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, PEAKGAIN_ID, peakGainSlider);

	peakGainSlider.setSliderStyle(Slider::LinearHorizontal);
	peakGainSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 20);
	peakGainSlider.setRange(-48.0f, 30.0f);
	addAndMakeVisible(&peakGainSlider);

	filterChoice.addItem("FIRHiPass", 1);
	filterChoice.addItem("IIRLowPass", 2);
	filterChoice.addItem("Biquad", 3);
	//filterChoice.addItem("Bilinear", 4);
	filterChoice.setSelectedId(1);
	filterChoice.addListener(this);
	addAndMakeVisible(&filterChoice);

	filterChoiceVal = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, CHOICE_ID, filterChoice);

	biquadChoice.addItem("Low Pass", 1);
	biquadChoice.addItem("High Pass", 2);
	//biquadChoice.addItem("Band Pass", 3);
	//biquadChoice.addItem("Notch", 4);
	biquadChoice.addItem("Peak", 5);
	biquadChoice.addItem("Low Shelf", 6);
	biquadChoice.addItem("High Shelf", 7);
	biquadChoice.setSelectedId(1);
	biquadChoice.addListener(this);
	addAndMakeVisible(&biquadChoice);

	biquadChoiceVal = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, BIQCHOICE_ID, biquadChoice);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500,450);
}

DreamEQAudioProcessorEditor::~DreamEQAudioProcessorEditor()
{
}

void DreamEQAudioProcessorEditor::comboBoxChanged(ComboBox* box)
{
	processor.filterChoice = filterChoice.getSelectedIdAsValue().getValue();
	processor.biquadChoice = biquadChoice.getSelectedIdAsValue().getValue();
}

void DreamEQAudioProcessorEditor::paint (juce::Graphics& g)
{

/*
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (18.0f));
    g.drawFittedText ("DreamEQ Plugin", getLocalBounds(), juce::Justification::centred, 1);
*/
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

	g.setColour (Colours::white);
    g.setFont (15.0f);

    g.drawText("Filter Type:", 120, 5, 80, 50, Justification::centred, false);
	g.drawText("Frequency", (getWidth() / 2) - 200, (((getHeight() / 4) * 1) - 30), 200, 50, Justification::centred, false);	
	g.drawText("Volume", (getWidth() / 2) - 200, (((getHeight() / 4) * 3) - 130), 200, 50, Justification::centred, false);

	if (processor.filterChoice == 3 )
	{
		g.setColour (Colours::white);
    	g.setFont (15.0f);
		biquadChoice.setVisible(true);
	    qSlider.setVisible(true);
		g.drawText("Q", (getWidth() / 2) - 200, (((getHeight() / 4) * 2) - 80), 200, 50, Justification::centred, false);
		g.drawText("Biquad Type:", 110, 40, 80, 50, Justification::centred, false);
		if (processor.biquadChoice == 5 )
		{
			peakGainSlider.setVisible(true);
		    g.drawText("Peak Gain", (getWidth() / 2) - 200, (((getHeight() / 4) * 4) - 180), 200, 50, Justification::centred, false);
		}
	}
	else
	{
	    g.setColour (Colours::darkgrey);
        g.setFont (15.0f);
		g.drawText("Biquad Type:", 110, 40, 80, 50, Justification::centred, false);
		g.drawText("Q", (getWidth() / 2) - 200, (((getHeight() / 4) * 2) - 80), 200, 50, Justification::centred, false);
	    g.drawText("Peak Gain", (getWidth() / 2) - 200, (((getHeight() / 4) * 4) - 180), 200, 50, Justification::centred, false);
		peakGainSlider.setVisible(false);
		biquadChoice.setVisible(false);
	    qSlider.setVisible(false);
	}
	
	

}


void DreamEQAudioProcessorEditor::resized() {
/*
    cutoffFrequencySlider.setBounds({15, 35, 100, 300});
  cutoffFrequencyLabel.setBounds({cutoffFrequencySlider.getX() + 30,
                                  cutoffFrequencySlider.getY() - 30, 
                                  200, 50});
  highpassButton.setBounds(
      {cutoffFrequencySlider.getX(),
       cutoffFrequencySlider.getY() + 
        cutoffFrequencySlider.getHeight() + 15,
       30, 50});
  highpassButtonLabel.setBounds(
      {cutoffFrequencySlider.getX() + highpassButton.getWidth() + 15,
       highpassButton.getY(),
       cutoffFrequencySlider.getWidth() - highpassButton.getWidth(),
       highpassButton.getHeight()});
*/
   // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	filterChoice.setBounds(200, 15, 250, 30);
	biquadChoice.setBounds(200, 50, 250, 30);
	freqSlider.setBounds((getWidth() / 2) - 60, (((getHeight() / 4) * 1) - 20), 200, 50);
	qSlider.setBounds((getWidth() / 2) - 60, (((getHeight() / 4) * 2) - 70), 200, 50);
	volumeSlider.setBounds((getWidth() / 2) - 60, (((getHeight() / 4) * 3)- 120), 200, 50);
	peakGainSlider.setBounds((getWidth() / 2) - 60, (((getHeight() / 4) * 4) - 170), 200, 50);
}