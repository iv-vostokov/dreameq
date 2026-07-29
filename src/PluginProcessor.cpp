#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


ParametricEQ eq;

#define SAMPLE_RATE 44100
#define M_PI 3.14159265358979323846

void applyParametricEQ(ParametricEQ *eq, float *inputBuffer, float *outputBuffer, int bufferSize)
    {
        float A = powf(10, eq->gain / 40); // Db to Koeff 
        float omega = 2 * M_PI * eq->frequency / SAMPLE_RATE;
        float alpha = sin(omega) / (2 * eq->Q);
    
        float a0 = 1 + alpha / A;
        float a1 = -2 * cos(omega);
        float a2 = 1 - alpha / A;
        float b0 = (1 + alpha * A) / a0;
        float b1 = -2 * cos(omega) / a0;
        float b2 = (1 - alpha * A) / a0;

        // init Pre values
        static float x1 = 0, x2 = 0, y1 = 0, y2 = 0;

        for (int i = 0; i < bufferSize; i++)
        {
            float x0 = inputBuffer[i];
            // calc Post values
            outputBuffer[i] = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;

            // update Pre values
            x2 = x1;
            x1 = x0;
            y2 = y1;
            y1 = outputBuffer[i];
        }
}




DreamEQAudioProcessor::DreamEQAudioProcessor()
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
	parameters(*this, nullptr, "PARAMETER", createParameterLayout())
 /*   parameters(*this,nullptr,juce::Identifier("Dream_EQ_Plugin"),
                {
                    std::make_unique<juce::AudioParameterFloat>(
                    "cutoff_frequency", 
                    "Cutoff Frequency",
                    juce::NormalisableRange{20.f, 
                                            20000.f, 
                                            0.1f, 
                                            0.2f, 
                                            false},
                    500.f),
                    
                    std::make_unique<juce::AudioParameterBool>(
                    "highpass", 
                    "Highpass", 
                    false)
                }) 
*/
{
                    //cutoffFrequencyParameter = parameters.getRawParameterValue("cutoff_frequency");
                    //highpassParameter = parameters.getRawParameterValue("highpass");
}


                    

DreamEQAudioProcessor::~DreamEQAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout DreamEQAudioProcessor::createParameterLayout()
{
	std::vector <std::unique_ptr<RangedAudioParameter>> params;

	auto freqParam = std::make_unique<AudioParameterFloat>(FREQ_ID, FREQ_NAME, 20.0f, 20000.0f, 100.0f);
	auto qParam = std::make_unique<AudioParameterFloat>(Q_ID, Q_NAME, 0.001f, 1.0f, 0.1f);
	auto volumeParam = std::make_unique<AudioParameterFloat>(VOLUME_ID, VOLUME_NAME, -48.0f, 15.0f, 0.0f);
	auto peakGainParam = std::make_unique<AudioParameterFloat>(PEAKGAIN_ID, PEAKGAIN_NAME, -48.0f, 30.0f, 0.0f);
	auto filterChoiceParam = std::make_unique<AudioParameterFloat>(CHOICE_ID, CHOICE_NAME, NormalisableRange<float>(1, 4, 1), 1);
	auto biquadChoiceParam = std::make_unique<AudioParameterFloat>(BIQCHOICE_ID, BIQCHOICE_NAME, NormalisableRange<float>(1, 7, 1), 1);

	params.push_back(std::move(freqParam));
	params.push_back(std::move(qParam));
	params.push_back(std::move(volumeParam));
	params.push_back(std::move(peakGainParam));
	params.push_back(std::move(filterChoiceParam));
	params.push_back(std::move(biquadChoiceParam));

	return { params.begin(), params.end() };
}



const juce::String DreamEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DreamEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DreamEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DreamEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DreamEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DreamEQAudioProcessor::getNumPrograms()
{
    return 1;
}

int DreamEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DreamEQAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String DreamEQAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void DreamEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void DreamEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // set filters params
    
    eq.frequency = 888.0f; 
    eq.gain = 6.0f;        
    eq.Q = 1.0f;     
    
    LHPfilter.setSamplingRate(static_cast<float>(sampleRate));
    
    for (int channel = 0; channel < kChannels; channel++)
	{
		Dfilter[channel].prepareToPlay(sampleRate, samplesPerBlock);
	//	biQ[channel].setBiquad(bq_type_lowpass, 0.5, 0.707, 0);
	}
    
    //juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void DreamEQAudioProcessor::releaseResources()
{
   // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DreamEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
   #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
   #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
      &&layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
   #endif
}
#endif

void DreamEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    float sliderFreqValue = ((*parameters.getRawParameterValue(FREQ_ID) / 20000) * 0.49);
	float sliderFrequencyValue = *parameters.getRawParameterValue(FREQ_ID);
	float sliderQValue = *parameters.getRawParameterValue(Q_ID);
	float sliderVolumeValue = *parameters.getRawParameterValue(VOLUME_ID);
	float sliderPeakGainValue = *parameters.getRawParameterValue(PEAKGAIN_ID);
    
       
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

  
  //  juce::AudioProcessorParameter* gainParameter = getParameters()[0];
  //  float gain = gainParameter->getValue();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // in data
        const float* inputData = buffer.getReadPointer(channel);
    	
        float* outputData = buffer.getWritePointer(channel);
        
        // place samples into buff
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            //get current value from read pointer
			float inputSample = inputData[sample];                 
            
            switch ((int)filterChoice) 
            {
			    case FIRHiPass:
    				outputData[sample] = Dfilter[channel].simpleFIRHiPass(inputSample, sliderFreqValue);
				    break;

			    case IIRLowPass:
    				outputData[sample] = Dfilter[channel].simpleIIRLowPass(inputSample, sliderFreqValue);
				    break;

			    case BiquadFilter:
				    Dfilter[channel].setBiquad((int)biquadChoice, sliderFrequencyValue, sliderQValue, sliderPeakGainValue);
				    outputData[sample] = Dfilter[channel].processBiquad(inputSample);
				    //Dfilter[channel].setButterworth(sliderFrequencyValue);
				    //outputData[channel] = Dfilter[channel].processButterworth(inputSample);
				    break;
			    case Bilinear:
				    Dfilter[channel].setButterworth((int)biquadChoice, sliderFrequencyValue, sliderQValue, sliderPeakGainValue);
				    outputData[sample] = Dfilter[channel].processButterworth(inputSample);
			}  
            outputData[sample] = outputData[sample] * Decibels::decibelsToGain(sliderVolumeValue);
            
            ///channelData[i] *=gain;
        }
        //  applyParametricEQ(&eq, buffer., buffer, sizeof(buffer));

    }
  /*--------------------------LHP_filter_block----------------------------------  */  
 /*   // retrieve and set the parameter values
    const auto cutoffFrequency = cutoffFrequencyParameter->load();
    // in C++, std::atomic<T> to T conversion is equivalent to a load
    const auto highpass = *highpassParameter < 0.5f ? false : true;
    LHPfilter.setCutoffFrequency(cutoffFrequency);
    LHPfilter.setHighpass(highpass);

    // perform filtering
    LHPfilter.processBlock(buffer, midiMessages);
   
 */
    // TODO: Add additional DSP code here
}

bool DreamEQAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DreamEQAudioProcessor::createEditor()
{
    return new DreamEQAudioProcessorEditor (*this,parameters);
    //return new GenericAudioProcessorEditor (*this);
}

void DreamEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<XmlElement> xml(parameters.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void DreamEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<XmlElement> theParams(getXmlFromBinary(data, sizeInBytes));
	if (theParams != nullptr)
	{
		if (theParams->hasTagName(parameters.state.getType()))
		{
			parameters.state = ValueTree::fromXml(*theParams);
		}
	}
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DreamEQAudioProcessor();
}