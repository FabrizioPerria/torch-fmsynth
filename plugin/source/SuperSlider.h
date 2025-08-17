#pragma once

#include "NeuralNetwork.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include <JuceHeader.h>
#include <torch/nn/modules/linear.h>

class SuperSlider : public juce::Slider
{
public:
    SuperSlider (juce::AudioProcessorValueTreeState* apvts, std::map<juce::String, juce::Slider*>* sliders)
    {
        setTrainingMode (apvts->getRawParameterValue ("main_training_mode")->load() > 0.5f);

        onValueChange = [this, apvts, sliders]()
        {
            if (trainingMode)
                return;
            auto range = this->getRange();
            auto knobMin = range.getStart();
            auto knobMax = range.getEnd();

            double torchKnobValue = this->getValue();
            double knobNormalized = (torchKnobValue - knobMin) / (knobMax - knobMin);

            std::vector<float> input = { (float) knobNormalized };
            auto out = net.forward (input);

            for (unsigned long i = 0; i < parametersToControl.size(); ++i)
            {
                auto s = parametersToControl[i];

                auto parameter = apvts->getParameter (s);
                parameter->beginChangeGesture();
                parameter->setValueNotifyingHost (out[i]);
                parameter->endChangeGesture();
            }
        };
    }

    void addParameterToControl (const std::string& parameterName) { parametersToControl.push_back (parameterName); }

    void setTrainingMode (bool mode)
    {
        trainingMode = mode;
        setColour (juce::Slider::ColourIds::thumbColourId, trainingMode ? juce::Colours::red : juce::Colours::blue);
    }

private:
    std::vector<std::string> parametersToControl;
    NeuralNetwork net { 1, 2 };
    bool trainingMode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SuperSlider)
};
