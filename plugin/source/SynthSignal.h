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
        , envelope (nullptr)
        , mod (nullptr)
    {
    }

    void enableModulation (bool newState, double (*generateFunc) (double))
    {
        if (newState)
        {
            mod = std::make_unique<Signal> (generateFunc);
        }
        else
        {
            mod.reset();
        }
    }
    void enableEnvelope (bool newState)
    {
        if (newState)
        {
            envelope = std::make_unique<Envelope>();
        }
        else
        {
            envelope.reset();
        }
    }

    void updateFrequency (double newFrequency, double sampleRate)
    {
        frequency = newFrequency;
        phaseIncrement = getPhaseIncrement (sampleRate);
    }

    void updateAmplitude (double newAmplitude)
    {
        amplitude = newAmplitude;
    }

    double getAmplitude() const
    {
        return amplitude;
    }

    double getPhaseIncrement (double sampleRate)
    {
        return (2.0 * juce::MathConstants<double>::pi * frequency) / sampleRate;
    }

    double getSample (int channel, double sampleRate, bool isNoteOn)
    {
        if (! enabled)
            return 0.0;
        double sample = generate (phase[channel]);
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

    bool isEnabled() const
    {
        return enabled;
    }

    Envelope& getEnvelope()
    {
        return *envelope;
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
};
