#pragma once

#include <JuceHeader.h>

enum class EnvelopeState
{
    Idle,
    Attack,
    Decay,
    Sustain,
    Release
};

class Envelope
{
public:
    Envelope()
        : enabled (true)
        , envelopeState { EnvelopeState::Idle, EnvelopeState::Idle }
        , envelopeValue { 0.0, 0.0 }
        , envelopeAttack (0.01)
        , envelopeDecay (0.1)
        , envelopeSustain (0.7)
        , envelopeRelease (0.2)
    {
    }

    void setEnvelopeParameters (double attack, double decay, double sustain, double release)
    {
        envelopeAttack = attack;
        envelopeDecay = decay;
        envelopeSustain = sustain;
        envelopeRelease = release;
    }

    double getCoefficient (int channel, double sampleRate, bool isNoteOn)
    {
        if (! enabled)
            return 1.0;

        switch (envelopeState[channel])
        {
            case EnvelopeState::Idle:
                if (isNoteOn)
                {
                    envelopeState[channel] = EnvelopeState::Attack;
                    DBG ("Idle -> Attack");
                }
                envelopeValue[channel] = 0.0;
                break;

            case EnvelopeState::Attack:
                if (! isNoteOn)
                {
                    envelopeState[channel] = EnvelopeState::Release;
                    DBG ("Attack -> Release");
                }
                else if (isGreaterThanOrEqualDouble (envelopeValue[channel], 1.0))
                {
                    envelopeState[channel] = EnvelopeState::Decay;
                    DBG ("Attack -> Decay");
                    envelopeValue[channel] = 1.0;
                }
                else
                {
                    envelopeValue[channel] += (1.0 / (envelopeAttack * sampleRate));
                }
                break;

            case EnvelopeState::Decay:
                envelopeValue[channel] -= ((envelopeValue[channel] - envelopeSustain) / (envelopeDecay * sampleRate));
                if (isLessThanOrEqualDouble (envelopeValue[channel], envelopeSustain))
                {
                    envelopeValue[channel] = envelopeSustain;
                    envelopeState[channel] = EnvelopeState::Sustain;
                    DBG ("Decay -> Sustain");
                }
                break;

            case EnvelopeState::Sustain:
                if (! isNoteOn)
                {
                    envelopeState[channel] = EnvelopeState::Release;
                    DBG ("Sustain -> Release");
                }
                break;

            case EnvelopeState::Release:
                envelopeValue[channel] -= (envelopeValue[channel] / (envelopeRelease * sampleRate));
                if (isLessThanOrEqualDouble (envelopeValue[channel], 0.0))
                {
                    envelopeValue[channel] = 0.0;
                    envelopeState[channel] = EnvelopeState::Idle;
                    DBG ("Release -> Idle");
                }
                break;
        }

        return envelopeValue[channel];
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

    void setEnabled (bool newState)
    {
        enabled = newState;
        if (! enabled)
        {
            envelopeState[0] = EnvelopeState::Idle;
            envelopeState[1] = EnvelopeState::Idle;
            envelopeValue[0] = 0.0;
            envelopeValue[1] = 0.0;
        }
    }

    bool isEnabled() const
    {
        return enabled;
    }

private:
    bool enabled;
    EnvelopeState envelopeState[2];
    double envelopeValue[2];
    double envelopeAttack;
    double envelopeDecay;
    double envelopeSustain;
    double envelopeRelease;

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
};
