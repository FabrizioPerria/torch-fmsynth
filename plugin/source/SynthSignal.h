#pragma once

#include "Envelope.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include <JuceHeader.h>

class Signal
{
public:
    Signal (double (*generateFunc) (double), double appSampleRate, juce::String signalName, AudioProcessorValueTreeState& state)
        : apvts (state)
        , name (signalName)
        , phase { 0.0, 0.0 }
        , phaseIncrement (0.0)
        , frequency (0.0)
        , sampleRate (appSampleRate)
        , generate (generateFunc)
        , envelope (std::make_unique<Envelope> (name, state))
        , mod (nullptr)
    {
        enabled = dynamic_cast<juce::AudioParameterBool*> (apvts.getParameter (name + "_enabled"));
        amplitude = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (name + "_amplitude"));
        modRatio = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (name + "_modulation_ratio"));
    }

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
        mod = std::make_unique<Signal> (generateFunc, sampleRate, name + "_mod", apvts);
        mod->getEnvelope().setEnabled (false);
    }

    void setGenerateFunction (double (*generateFunc) (double))
    {
        generate = generateFunc;
    }

    void updateFrequency (double newFrequency)
    {
        frequency = newFrequency;

        if (mod && mod->isEnabled())
        {
            auto modFrequency = *modRatio * newFrequency;
            mod->updateFrequency (modFrequency);
        }
    }

    void updateAmplitude (double newAmplitude)
    {
        *amplitude = (float) newAmplitude;
    }

    float getAmplitude() const
    {
        return *amplitude;
    }

    double getPhaseIncrement (double currentFrequency)
    {
        return (2.0 * juce::MathConstants<double>::pi * currentFrequency) / sampleRate;
    }

    double getSample (int channel, bool isNoteOn)
    {
        if (! *enabled || generate == nullptr)
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
        return sample * *amplitude * envelopeCoefficient;
    }

    void setEnabled (bool newState)
    {
        *enabled = newState;
        if (! *enabled)
        {
            phase[0] = 0.0;
            phase[1] = 0.0;
        }
    }

    void setModulationRatio (double newRatio)
    {
        *modRatio = (float) newRatio;
        updateFrequency (frequency);
    }

    float getModulationRatio() const
    {
        return *modRatio;
    }

    juce::AudioParameterBool* getEnabledParameter() const
    {
        return enabled;
    }

    juce::AudioParameterFloat* getAmplitudeParameter() const
    {
        return amplitude;
    }

    juce::AudioParameterFloat* getModulationRatioParameter() const
    {
        return modRatio;
    }

    bool isEnabled() const
    {
        return *enabled;
    }

    Envelope& getEnvelope()
    {
        return *envelope;
    }

    Signal& getModulation()
    {
        return *mod;
    }

private:
    AudioProcessorValueTreeState& apvts;
    juce::String name;

    juce::AudioParameterBool* enabled;
    juce::AudioParameterFloat* amplitude;
    juce::AudioParameterFloat* modRatio;

    double phase[2];
    double phaseIncrement;
    double frequency;
    double sampleRate;

    double (*generate) (double phase);

    std::unique_ptr<Envelope> envelope;
    std::unique_ptr<Signal> mod;
};
