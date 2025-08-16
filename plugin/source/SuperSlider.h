#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include <JuceHeader.h>
#include <torch/nn/modules/linear.h>

class SuperSlider : public juce::Slider
{
public:
    SuperSlider (juce::AudioProcessorValueTreeState* apvts, std::map<juce::String, juce::Slider*>* sliders)
    {
        onValueChange = [this, apvts, sliders]()
        {
            auto range = this->getRange();
            auto knobMin = range.getStart();
            auto knobMax = range.getEnd();

            double torchKnobValue = this->getValue();
            double knobNormalized = (torchKnobValue - knobMin) / (knobMax - knobMin);
            torch::Tensor in = torch::empty ({ 1, 1 }, torch::kFloat);
            in[0][0] = (float) knobNormalized;
            torch::Tensor out = net (in);

            for (unsigned long i = 0; i < parametersToControl.size(); ++i)
            {
                auto s = parametersToControl[i];

                auto parameter = apvts->getParameter (s);
                parameter->beginChangeGesture();
                std::cout << "Setting parameter: " << s << " to value: " << out[0][i].item<float>() << std::endl;
                parameter->setValueNotifyingHost ((float) out[0][i].item<float>());
                parameter->endChangeGesture();

                // auto parameter = apvts->getParameter (s);
                // auto slider = sliders->at (s);
                // double high = slider->getMaxValue();
                // double low = slider->getMinValue();
                //
                // double weight = high - low;
                // double bias = low;
                //
                // double value = weight * knobNormalized + bias;
                //
                // parameter->beginChangeGesture();
                // parameter->setValueNotifyingHost (((float) parameter->convertTo0to1 ((float) value)));
                // parameter->endChangeGesture();
            }
        };
    }

    void addParameterToControl (const std::string& parameterName) { parametersToControl.push_back (parameterName); }

private:
    std::vector<std::string> parametersToControl;
    torch::nn::Linear net { 1, 2 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SuperSlider)
};
