#pragma once

#include <JuceHeader.h>
#include <torch/nn/module.h>
#include <torch/torch.h>

class NeuralNetwork : public torch::nn::Module
{
public:
    NeuralNetwork (int inputSize, int outputSize);
    std::vector<float> forward (const std::vector<float>& input);
    void addTrainingData (const std::vector<float>& input, const std::vector<float>& target);
    void runTraining (int epochs);

private:
    int numInputs;
    int numOutputs;
    torch::nn::Linear linearLayer { nullptr };
    torch::Tensor forward (const torch::Tensor input);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeuralNetwork)
};
