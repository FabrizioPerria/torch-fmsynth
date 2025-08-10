#pragma once

#include "Envelope.h"
#include <JuceHeader.h>

class Signal
{
public:
    Signal (const Signal&) = delete;
    Signal (Signal&&) = default;
    Signal& operator= (const Signal&) = delete;
    Signal& operator= (Signal&&) = default;
    Signal (double (*generateFunc) (double), double appSampleRate)
        : enabled (true)
        , phase { 0.0, 0.0 }
        , phaseIncrement (0.0)
        , frequency (0.0)
        , amplitude (1.0)
        , sampleRate (appSampleRate)
        , generate (generateFunc)
        , envelope (std::make_unique<Envelope>())
        , mod (nullptr)
    {
    }

    void setSampleRate (double newSampleRate)
    {
        sampleRate = newSampleRate;
    }

    void enableModulation (double (*generateFunc) (double))
    {
        mod = std::make_unique<Signal> (generateFunc, sampleRate);
        mod->getEnvelope().setEnabled (false); // Disable envelope for modulation signal
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
            auto modFrequency = modRatio * newFrequency;
            mod->updateFrequency (modFrequency);
        }
    }

    void updateAmplitude (double newAmplitude)
    {
        amplitude = newAmplitude;
    }

    double getAmplitude() const
    {
        return amplitude;
    }

    double getPhaseIncrement (double currentFrequency)
    {
        return (2.0 * juce::MathConstants<double>::pi * currentFrequency) / sampleRate;
    }

    double getSample (int channel, bool isNoteOn)
    {
        if (! enabled || generate == nullptr)
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
        return sample * amplitude * envelopeCoefficient;
    }

    void setEnabled (bool newState)
    {
        enabled = newState;
        if (! enabled)
        {
            phase[0] = 0.0;
            phase[1] = 0.0;
        }
    }

    void setModulationRatio (double newRatio)
    {
        modRatio = newRatio;
        updateFrequency (frequency);
    }

    double getModulationRatio() const
    {
        return modRatio;
    }

    bool isEnabled() const
    {
        return enabled;
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
    bool enabled;

    double phase[2];
    double phaseIncrement;
    double frequency;
    double amplitude;
    double sampleRate;

    double (*generate) (double phase);

    std::unique_ptr<Envelope> envelope;
    std::unique_ptr<Signal> mod;

    double modRatio = 0.5;
};
