#pragma once
#include "filter.h"
#include <JuceHeader.h>


#define FREQ_ID "freq"
#define FREQ_NAME "Freq"
#define Q_ID "q"
#define Q_NAME "Q"
#define VOLUME_ID "volume"
#define VOLUME_NAME "Volume"
#define CHOICE_ID "choice"
#define CHOICE_NAME "Choice"
#define BIQCHOICE_ID "biqchoice"
#define BIQCHOICE_NAME "BiqChoice"
#define PEAKGAIN_ID "peakGain"
#define PEAKGAIN_NAME "PeakGain"

typedef struct 
{
    float frequency;   // in Hz
    float gain;        // in dB
    float Q;            
} ParametricEQ;

enum
{
	FIRHiPass = 1,
	IIRLowPass,
	BiquadFilter,
	Bilinear
};
class DreamEQAudioProcessor : public juce::AudioProcessor
{
public:
    DreamEQAudioProcessor();
    ~DreamEQAudioProcessor() override;


    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState parameters;
	AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

	AudioPlayHead* playHead;
	bool playing;
	
	int filterChoice;
	int biquadChoice;

private:
    // our plugin's parameters
  //  juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* cutoffFrequencyParameter = nullptr;
    std::atomic<float>* highpassParameter = nullptr;

    static const int kChannels = 2;
	DreamFilter Dfilter[kChannels];
	//Biquad biQ[kChannels];
    
    // the filter implemented in listings 1-3
    LowpassHighpassFilter LHPfilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DreamEQAudioProcessor)
};


