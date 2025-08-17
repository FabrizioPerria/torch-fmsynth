#pragma once

#include "Envelope.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include <JuceHeader.h>

class Signal : private juce::AudioProcessorValueTreeState::Listener
{
public:
    Signal (std::function<double (double)> generateFunc, double appSampleRate, juce::String signalName, AudioProcessorValueTreeState& state)
        : name (signalName), sampleRate (appSampleRate), envelope (std::make_unique<Envelope> (name, state)), apvts (state)
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

    void enableModulation (std::function<double (double)> generateFunc)
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
        auto* param = apvts.getParameter (name + "_enabled");
        param->beginChangeGesture();
        param->setValueNotifyingHost (newState ? 1.0f : 0.0f);
        param->endChangeGesture();
        if (! isEnabled())
        {
            phase.fill (0.0);
        }
    }

    void updateAmplitude (double newAmplitude)
    {
        auto* param = apvts.getParameter (name + "_amplitude");
        param->beginChangeGesture();
        param->setValueNotifyingHost ((float) newAmplitude);
        param->endChangeGesture();
    }

    static constexpr double twoPi = 2.0 * juce::MathConstants<double>::pi;
    double getPhaseIncrement (double currentFrequency) { return (twoPi * currentFrequency) / sampleRate; }

    void setModulationRatio (double newRatio)
    {
        auto* param = apvts.getParameter (name + "_modulation_ratio");
        param->beginChangeGesture();
        param->setValueNotifyingHost ((float) newRatio);
        param->endChangeGesture();
        updateFrequency (frequency);
    }

    bool isEnabled() const { return enabled->load() > 0.5f; }

    float getAmplitude() const { return amplitude->load(); }

    float getModulationRatio() const { return modRatio->load(); }

    inline Envelope& getEnvelope() { return *envelope; }

    inline Signal& getModulation() { return *mod; }

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override
    {
        // This method is called when a parameter changes.
        // We can ignore the newValue here because we already have the current value stored in the member variables.
        // However, we can use it to trigger any necessary updates or recalculations
        // based on the parameter change.
        juce::ignoreUnused (newValue);

        // We only react to changes in the modulation ratio parameter because we need to recalculate the frequency
        // when it changes. Other parameters are handled through their respective setters.
        if (parameterID == name + "_modulation_ratio")
        {
            updateFrequency (frequency);
        }
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

    AudioProcessorValueTreeState& apvts;
};
