#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include <JuceHeader.h>

class SuperSlider : public juce::Slider
{
public:
    SuperSlider (juce::AudioProcessorValueTreeState* apvts, std::map<juce::String, juce::Slider*>* sliders)
    {
        auto parametersToControl = std::vector<std::string> { "main_modulation_ratio", "main_mod_amplitude" };

        onValueChange = [this, apvts, sliders, parametersToControl]()
        {
            auto range = this->getRange();
            auto knobMin = range.getStart();
            auto knobMax = range.getEnd();

            double torchKnobValue = this->getValue();
            double knobNormalized = (torchKnobValue - knobMin) / (knobMax - knobMin);

            for (auto& s : parametersToControl)
            {
                auto parameter = apvts->getParameter (s);
                auto slider = sliders->at (s);
                double high = slider->getMaxValue();
                double low = slider->getMinValue();
                std::cout << "Setting parameter: " << s << " with normalized value: " << knobNormalized << ", high: " << high
                          << ", low: " << low << std::endl;

                double weight = high - low;
                double bias = low;

                double value = weight * knobNormalized + bias;

                parameter->beginChangeGesture();
                parameter->setValueNotifyingHost (((float) parameter->convertTo0to1 ((float) value)));
                parameter->endChangeGesture();
            }
        };
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SuperSlider)
};
