#pragma once

#include "Envelope.h"
#include <JuceHeader.h>

class Signal
{
public:
    Signal (double (*generateFunc) (double))
        : enabled (true), phase { 0.0, 0.0 }, phaseIncrement (0.0), frequency (0.0), amplitude (1.0), generate (generateFunc), envelope()
    {
        envelope.setEnabled (true);
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

        auto envelopeCoefficient = envelope.getCoefficient (channel, sampleRate, isNoteOn);
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
        return envelope;
    }

private:
    bool enabled;

    double phase[2];
    double phaseIncrement;
    double frequency;
    double amplitude;
    // func ptr to generate signal
    double (*generate) (double phase);

    Envelope envelope;
};
