#pragma once

#include <JuceHeader.h>

class Signal
{
public:
    Signal (double (*generateFunc) (double))
        : phase { 0.0, 0.0 }, phaseIncrement (0.0), frequency (0.0), amplitude (1.0), generate (generateFunc)
    {
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

    double getSample (int channel)
    {
        double sample = generate (phase[channel]);
        phase[channel] += phaseIncrement;

        // if (phase[channel] >= 1.0)
        //     phase[channel] -= 1.0; // Wrap phase to [0, 1)
        return sample * amplitude;
    }

private:
    double phase[2];
    double phaseIncrement;
    double frequency;
    double amplitude;
    // func ptr to generate signal
    double (*generate) (double phase);
};
