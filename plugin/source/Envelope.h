#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
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
    Envelope (juce::String name, juce::AudioProcessorValueTreeState& apvts)
        : envelopeState { EnvelopeState::Idle, EnvelopeState::Idle }, envelopeValue { 0.0, 0.0 }
    {
        enabled = dynamic_cast<juce::AudioParameterBool*> (apvts.getParameter (name + "_envelope_enabled"));
        envelopeAttack = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (name + "_envelope_attack"));
        envelopeDecay = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (name + "_envelope_decay"));
        envelopeSustain = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (name + "_envelope_sustain"));
        envelopeRelease = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (name + "_envelope_release"));
    }

    void setEnvelopeParameters (double attack, double decay, double sustain, double release)
    {
        *envelopeAttack = (float) attack;
        *envelopeDecay = (float) decay;
        *envelopeSustain = (float) sustain;
        *envelopeRelease = (float) release;
    }

    double getCoefficient (int channel, double sampleRate, bool isNoteOn)
    {
        if (! isEnabled())
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
                    envelopeValue[channel] += (1.0 / (*envelopeAttack * sampleRate));
                }
                break;

            case EnvelopeState::Decay:
                envelopeValue[channel] -= ((envelopeValue[channel] - *envelopeSustain) / (*envelopeDecay * sampleRate));
                if (isLessThanOrEqualDouble (envelopeValue[channel], *envelopeSustain))
                {
                    envelopeValue[channel] = *envelopeSustain;
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
                if (isNoteOn)
                {
                    envelopeState[channel] = EnvelopeState::Attack;
                    DBG ("Release -> Attack");
                    envelopeValue[channel] = 0.0;
                }
                else
                {
                    if (! isEnabled())
                    {
                        envelopeState[channel] = EnvelopeState::Idle;
                        DBG ("Release -> Idle");
                        envelopeValue[channel] = 0.0;
                        return 0.0; // If disabled, return 0 immediately
                    }
                    envelopeValue[channel] -= (envelopeValue[channel] / (*envelopeRelease * sampleRate));
                    if (isLessThanOrEqualDouble (envelopeValue[channel], 0.0))
                    {
                        envelopeValue[channel] = 0.0;
                        envelopeState[channel] = EnvelopeState::Idle;
                        DBG ("Release -> Idle");
                    }
                }
                break;
        }

        return envelopeValue[channel];
    }

    double getEnvelopeAttack() const
    {
        return *envelopeAttack;
    }
    double getEnvelopeDecay() const
    {
        return *envelopeDecay;
    }
    double getEnvelopeSustain() const
    {
        return *envelopeSustain;
    }
    double getEnvelopeRelease() const
    {
        return *envelopeRelease;
    }

    juce::AudioParameterBool* getEnabledParameter() const
    {
        return enabled;
    }
    juce::AudioParameterFloat* getEnvelopeAttackParameter() const
    {
        return envelopeAttack;
    }
    juce::AudioParameterFloat* getEnvelopeDecayParameter() const
    {
        return envelopeDecay;
    }
    juce::AudioParameterFloat* getEnvelopeSustainParameter() const
    {
        return envelopeSustain;
    }
    juce::AudioParameterFloat* getEnvelopeReleaseParameter() const
    {
        return envelopeRelease;
    }

    void setEnabled (bool newState)
    {
        if (enabled == nullptr)
            return;

        *enabled = newState;
        if (! isEnabled())
        {
            envelopeState[0] = EnvelopeState::Idle;
            envelopeState[1] = EnvelopeState::Idle;
            envelopeValue[0] = 0.0;
            envelopeValue[1] = 0.0;
        }
    }

    bool isEnabled() const
    {
        if (enabled == nullptr)
            return false;
        return *enabled;
    }

private:
    juce::AudioParameterBool* enabled;
    juce::AudioParameterFloat* envelopeAttack;
    juce::AudioParameterFloat* envelopeDecay;
    juce::AudioParameterFloat* envelopeSustain;
    juce::AudioParameterFloat* envelopeRelease;

    EnvelopeState envelopeState[2];
    double envelopeValue[2];

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
