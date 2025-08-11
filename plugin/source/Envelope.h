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
    {
        enabled = apvts.getRawParameterValue (name + "_envelope_enabled");
        envelopeAttack = apvts.getRawParameterValue (name + "_envelope_attack");
        envelopeDecay = apvts.getRawParameterValue (name + "_envelope_decay");
        envelopeSustain = apvts.getRawParameterValue (name + "_envelope_sustain");
        envelopeRelease = apvts.getRawParameterValue (name + "_envelope_release");
    }

    double getCoefficient (unsigned long channel, double sampleRate, bool isNoteOn)
    {
        if (! isEnabled())
            return 1.0;

        double attack = getEnvelopeAttack();
        double decay = getEnvelopeDecay();
        double sustain = getEnvelopeSustain();
        double release = getEnvelopeRelease();

        switch (envelopeState[channel])
        {
            case EnvelopeState::Idle:
                if (isNoteOn)
                {
                    envelopeState[channel] = EnvelopeState::Attack;
                }
                envelopeValue[channel] = 0.0;
                break;

            case EnvelopeState::Attack:
                if (! isNoteOn)
                {
                    envelopeState[channel] = EnvelopeState::Release;
                }
                else if (isGreaterThanOrEqualDouble (envelopeValue[channel], 1.0))
                {
                    envelopeState[channel] = EnvelopeState::Decay;
                    envelopeValue[channel] = 1.0;
                }
                else
                {
                    envelopeValue[channel] += (1.0 / (attack * sampleRate));
                }
                break;

            case EnvelopeState::Decay:
                envelopeValue[channel] -= ((envelopeValue[channel] - sustain) / (decay * sampleRate));
                if (isLessThanOrEqualDouble (envelopeValue[channel], sustain))
                {
                    envelopeValue[channel] = sustain;
                    envelopeState[channel] = EnvelopeState::Sustain;
                }
                break;

            case EnvelopeState::Sustain:
                if (! isNoteOn)
                {
                    envelopeState[channel] = EnvelopeState::Release;
                }
                break;

            case EnvelopeState::Release:
                if (isNoteOn)
                {
                    envelopeState[channel] = EnvelopeState::Attack;
                    envelopeValue[channel] = 0.0;
                }
                else
                {
                    if (! isEnabled())
                    {
                        envelopeState[channel] = EnvelopeState::Idle;
                        envelopeValue[channel] = 0.0;
                        return 0.0; // If disabled, return 0 immediately
                    }
                    envelopeValue[channel] -= (envelopeValue[channel] / (release * sampleRate));
                    if (isLessThanOrEqualDouble (envelopeValue[channel], 0.0))
                    {
                        envelopeValue[channel] = 0.0;
                        envelopeState[channel] = EnvelopeState::Idle;
                    }
                }
                break;
        }

        return envelopeValue[channel];
    }

    bool isEnabled() const { return enabled && enabled->load() > 0.5f; }
    double getEnvelopeAttack() const { return envelopeAttack->load(); }
    double getEnvelopeDecay() const { return envelopeDecay->load(); }
    double getEnvelopeSustain() const { return envelopeSustain->load(); }
    double getEnvelopeRelease() const { return envelopeRelease->load(); }

private:
    std::atomic<float>* enabled;
    std::atomic<float>* envelopeAttack;
    std::atomic<float>* envelopeDecay;
    std::atomic<float>* envelopeSustain;
    std::atomic<float>* envelopeRelease;

    std::array<EnvelopeState, 2> envelopeState { EnvelopeState::Idle, EnvelopeState::Idle };
    std::array<double, 2> envelopeValue { 0.0, 0.0 };

    juce::Tolerance<double> tol = juce::Tolerance<double>().withAbsolute (1e-6).withRelative (1e-6);

    bool isGreaterThanOrEqualDouble (double a, double b) const { return (a > b) || juce::approximatelyEqual (a, b, tol); }
    bool isLessThanOrEqualDouble (double a, double b) const { return (a < b) || juce::approximatelyEqual (a, b, tol); }
    bool isEqualDouble (double a, double b) const { return juce::approximatelyEqual (a, b, tol); }
};
