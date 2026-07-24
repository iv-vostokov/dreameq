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

        for (int i = 0; i < bufferSize; i++) {
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
                       )
{
 
    addParameter (new juce::AudioParameterFloat ("gain", "Gain", 0.0f, 1.0f, 1.0f));
    addParameter (new juce::AudioParameterFloat("frequency", "Frequency", 20.0, 20000.0f, 1000.0f));
    addParameter (new juce::AudioParameterFloat("gain_eq", "Gain EQ", -12.0f, 12.0f, 0.0f));
    addParameter (new juce::AudioParameterFloat("q_factor", "Q Factor", 0.7f, 4.0f, 0.0f));

}

DreamEQAudioProcessor::~DreamEQAudioProcessor()
{
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
    
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void DreamEQAudioProcessor::releaseResources()
{
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

  
    juce::AudioProcessorParameter* gainParameter = getParameters()[0];
    float gain = gainParameter->getValue();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
    
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
                             
           channelData[i] *=gain;
        }
        //  applyParametricEQ(&eq, buffer., buffer, sizeof(buffer));

    }
   
   
   
   
   
   
   
        // TODO: your DSP goes here
}

bool DreamEQAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DreamEQAudioProcessor::createEditor()
{
    //return new DreamEQAudioProcessorEditor (*this);
    return new GenericAudioProcessorEditor (*this);
}

void DreamEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

}

void DreamEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DreamEQAudioProcessor();
}