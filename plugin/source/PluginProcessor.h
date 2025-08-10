#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class AudioPluginAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateFrequency (double newFrequency);
    void updateAmplitude (double newAmplitude);
    double getFrequency() const
    {
        return frequency;
    }
    double getAmplitude() const
    {
        return amplitude;
    }
    double getEnvelopeAttack() const
    {
        return envelopeAttack;
    }
    double getEnvelopeDecay() const
    {
        return envelopeDecay;
    }
    double getEnvelopeSustain() const
    {
        return envelopeSustain;
    }
    double getEnvelopeRelease() const
    {
        return envelopeRelease;
    }

    void setEnvelopeParameters (double attack, double decay, double sustain, double release);

private:
    enum class EnvelopeState
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };
    double getPhaseIncrement (double frequency, double sampleRate) const;
    void applyEnvelope (double& sample, double& currentEnvelopeValue, EnvelopeState& currentEnvelopeState);

    juce::Tolerance<double> tol = juce::Tolerance<double>().withAbsolute (1e-6).withRelative (1e-6);

    bool isGreaterThanOrEqualDouble (double a, double b) const
    {
        return (a > b) || juce::approximatelyEqual (a, b, tol);
    }
    bool isLessThanOrEqualDouble (double a, double b) const
    {
        return (a < b) || juce::approximatelyEqual (a, b, tol);
    }
    bool isEqualDouble (double a, double b) const
    {
        return juce::approximatelyEqual (a, b, tol);
    }

    int notePlaying;

    double frequency;
    double phase[2];
    double phaseIncrement;
    double amplitude;

    EnvelopeState envelopeState[2];
    double envelopeValue[2];
    double envelopeAttack;
    double envelopeDecay;
    double envelopeSustain;
    double envelopeRelease;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
