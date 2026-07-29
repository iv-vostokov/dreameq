// LowpassHighpassFilter.h
#pragma once
#include <vector>
#include "JuceHeader.h"


enum filter_types{
  ft_lowpass = 1,
	ft_highpass,
	ft_bandpass,
    ft_notch,
	ft_peak,
	ft_lowshelf,
	ft_highshelf
};

class LowpassHighpassFilter {
public:
    // setters
    void setHighpass(bool highpass);
    void setCutoffFrequency(float cutoffFrequency);
    void setSamplingRate(float samplingRate);

    // Does not necessarily need to use JUCE's audio buffer
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

private:
    bool highpass;
    float cutoffFrequency;
    float samplingRate;

    // allpass filters' buffers: 1 sample per channel
    std::vector<float> dnBuffer;
};

class DreamFilter
{
public:
	  DreamFilter();
	  ~DreamFilter();

	  void prepareToPlay(double sampleRate, int samplesPerBlock);

	  float simpleFIRHiPass(float currentSample, float sliderValue);
	  float simpleIIRHiPass(float currentSample, float sliderValue);
	  float simpleIIRLowPass(float currentSample, float sliderValue);

	  void setBiquad(int type, double Freq, double Q, double peakGain);
	  float processBiquad(float in);

	  void setButterworth(int type, double Freq, double Q, double peakGain);
	  float processButterworth(float in);
	  
    //Static values, set up in prepare to play.
	  int bufferLength;
	  int sampleRate;
	  double* sn;
	  double* cs;

protected:
	  void calcBiquad(void);
	  void calcButterworth(void);

	  int filtertype;  
	  //Coefficients
	  float a0, a1, a2, b1, b2 , c0, c1, c2, d1, d2;
	  double frequency, Q, peakGain;
	  double z1, z2;
    // filter state memory
	  // y[n] (post-process)
	  double delayedSampleY1;
	  double delayedSampleY2;
	  // x[n] (pre-process)
	  double delayedSampleX1;
	  double delayedSampleX2;
};

inline float DreamFilter::processBiquad(float in)
{
	  double out = in * a0 + z1;
	  z1 = in * a1 + z2 - b1 * out;
	  z2 = in * a2 - b2 * out;
	  return out;
}

inline float DreamFilter::processButterworth(float in)
{
	  double V = pow(10, fabs(peakGain) / 20.0);
	  double out = in * a0 + delayedSampleX1 * a1 + delayedSampleX2 * a2 - delayedSampleY1 * b1 - delayedSampleY2 * b2;
	  delayedSampleX2 = delayedSampleX1;
	  delayedSampleY2 = delayedSampleY1;
	  delayedSampleX1 = in;
	  delayedSampleY1 = out;
	  return out;
}
