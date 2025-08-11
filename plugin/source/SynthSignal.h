#pragma once

#include "Envelope.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include <JuceHeader.h>

class Signal : private juce::AudioProcessorValueTreeState::Listener
{
public:
    Signal (std::function<double (double)> generateFunc, double appSampleRate, juce::String signalName, AudioProcessorValueTreeState& state)
        : apvts (state), name (signalName), sampleRate (appSampleRate), envelope (std::make_unique<Envelope> (name, state))
    {
        setGenerateFunction (generateFunc);

        enabled = apvts.getRawParameterValue (name + "_enabled");
        amplitude = apvts.getRawParameterValue (name + "_amplitude");
        modRatio = apvts.getRawParameterValue (name + "_modulation_ratio");

        apvts.addParameterListener (name + "_modulation_ratio", this);
    }

    ~Signal() override { apvts.removeParameterListener (name + "_modulation_ratio", this); }

    void setSampleRate (double newSampleRate)
    {
        sampleRate = newSampleRate;
        if (mod)
        {
            mod->setSampleRate (newSampleRate);
        }
    }

    void enableModulation (double (*generateFunc) (double))
    {
        mod = std::make_unique<Signal> (std::move (generateFunc), sampleRate, name + "_mod", apvts);
    }

    void setGenerateFunction (std::function<double (double)> generateFunc) { generate = std::move (generateFunc); }

    void updateFrequency (double newFrequency)
    {
        frequency = newFrequency;

        if (mod && mod->isEnabled())
        {
            auto modFrequency = getModulationRatio() * newFrequency;
            mod->updateFrequency (modFrequency);
        }
    }

    double getSample (unsigned long channel, bool isNoteOn)
    {
        if (! isEnabled() || generate == nullptr)
        {
            return 0.0;
        }

        auto modSample = 0.0;
        if (mod && mod->isEnabled())
        {
            modSample = mod->getSample (channel, true) * mod->getAmplitude();
        }

        double sample = generate (phase[channel] + modSample);
        phaseIncrement = getPhaseIncrement (frequency);
        phase[channel] += phaseIncrement;

        auto envelopeCoefficient = isNoteOn ? 1.0 : 0.0;
        if (envelope && envelope->isEnabled())
        {
            envelopeCoefficient = envelope->getCoefficient (channel, sampleRate, isNoteOn);
        }
        return sample * getAmplitude() * envelopeCoefficient;
    }

    void setEnabled (bool newState)
    {
        *enabled = newState;
        if (! isEnabled())
        {
            phase.fill (0.0);
        }
    }

    void updateAmplitude (double newAmplitude) { *amplitude = (float) newAmplitude; }

    double getPhaseIncrement (double currentFrequency) { return (2.0 * juce::MathConstants<double>::pi * currentFrequency) / sampleRate; }

    void setModulationRatio (double newRatio)
    {
        *modRatio = (float) newRatio;
        updateFrequency (frequency);
    }

    bool isEnabled() const { return enabled->load() > 0.5f; }

    float getAmplitude() const { return amplitude->load(); }

    float getModulationRatio() const { return modRatio->load(); }

    inline Envelope& getEnvelope() { return *envelope; }

    inline Signal& getModulation() { return *mod; }

private:
    AudioProcessorValueTreeState& apvts;

    void parameterChanged (const juce::String& parameterID, float newValue) override
    {
        if (parameterID == name + "_modulation_ratio")
            updateFrequency (frequency);
    }

    juce::String name;

    std::atomic<float>* enabled;
    std::atomic<float>* amplitude;
    std::atomic<float>* modRatio;

    std::array<double, 2> phase { 0.0, 0.0 };
    double phaseIncrement { 0.0 };
    double frequency { 440.0 };
    double sampleRate;

    std::function<double (double)> generate;

    std::unique_ptr<Envelope> envelope;
    std::unique_ptr<Signal> mod { nullptr };
};
