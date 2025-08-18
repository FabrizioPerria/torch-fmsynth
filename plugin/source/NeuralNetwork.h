#pragma once

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
    torch::nn::Softmax softmaxLayer { nullptr };
    torch::Tensor forward (const torch::Tensor input);

    std::vector<torch::Tensor> trainingInputs;
    std::vector<torch::Tensor> trainingTargets;

    std::unique_ptr<torch::optim::SGD> optimizer { nullptr };
};
