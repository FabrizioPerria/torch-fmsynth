#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include <JuceHeader.h>

class SuperSlider : public juce::Slider
{
public:
    SuperSlider (juce::AudioProcessorValueTreeState& apvts) : juce::Slider()
    {
        modulationRatio = apvts.getParameter ("main_modulation_ratio");
        auto modulationRatioRange = apvts.getParameterRange ("main_modulation_ratio");
        modulationDepth = apvts.getParameter ("main_mod_amplitude");
        auto modulationDepthRange = apvts.getParameterRange ("main_mod_amplitude");

        onValueChange = [this, modulationRatioRange, modulationDepthRange]()
        {
            auto value = (float) getValue();

            // Small slider values decrease the ratio and increase the depth
            if (modulationRatio != nullptr)
            {
                auto newRatio = 1.0 - (value / 10.0);
                modulationRatio->beginChangeGesture();
                modulationRatio->setValueNotifyingHost (newRatio);
                modulationRatio->endChangeGesture();
            }
            if (modulationDepth != nullptr)
            {
                auto newDepth = value / 10.0;
                modulationDepth->beginChangeGesture();
                modulationDepth->setValueNotifyingHost (newDepth);
                modulationDepth->endChangeGesture();
            }
        };
    }

private:
    juce::RangedAudioParameter* modulationRatio = nullptr;
    juce::RangedAudioParameter* modulationDepth = nullptr;
};
