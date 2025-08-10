#pragma once

#include "Envelope.h"
#include <JuceHeader.h>

class Signal
{
public:
    Signal (double (*generateFunc) (double))
        : enabled (true)
        , phase { 0.0, 0.0 }
        , phaseIncrement (0.0)
        , frequency (0.0)
        , amplitude (1.0)
        , generate (generateFunc)
        , envelope (std::make_unique<Envelope>())
        , mod (nullptr)
    {
    }

    void enableModulation (double (*generateFunc) (double))
    {
        mod = std::make_unique<Signal> (generateFunc);
        mod->getEnvelope().setEnabled (false); // Disable envelope for modulation signal
    }

    void setGenerateFunction (double (*generateFunc) (double))
    {
        generate = generateFunc;
    }

    void updateFrequency (double newFrequency, double sampleRate)
    {
        frequency = newFrequency;

        if (mod && mod->isEnabled())
        {
            auto modFrequency = modIndex * newFrequency;
            mod->updateFrequency (modFrequency, sampleRate);
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

    double getPhaseIncrement (double currentFrequency, double sampleRate)
    {
        return (2.0 * juce::MathConstants<double>::pi * currentFrequency) / sampleRate;
    }

    double getSample (int channel, double sampleRate, bool isNoteOn)
    {
        if (! enabled || generate == nullptr)
        {
            return 0.0;
        }

        double sample = generate (phase[channel]);

        auto modSample = 0.0;
        if (mod && mod->isEnabled())
        {
            modSample = mod->getSample (channel, sampleRate, true);
        }
        phaseIncrement = getPhaseIncrement (frequency, sampleRate);
        phase[channel] += phaseIncrement + modSample * modIndex;

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

    double (*generate) (double phase);

    std::unique_ptr<Envelope> envelope;
    std::unique_ptr<Signal> mod;

    double modIndex = 0.5;
};
