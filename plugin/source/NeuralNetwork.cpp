#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork (int inputSize, int outputSize) : numInputs (inputSize), numOutputs (outputSize)
{
    auto net = torch::nn::Linear (numInputs, numOutputs);
    net->to (torch::kCPU);
    linearLayer = register_module ("linear", net);
}

std::vector<float> NeuralNetwork::forward (const std::vector<float>& input)
{
    std::vector<float> output = { 0.4f, 0.6f };

    return output;
}

void NeuralNetwork::addTrainingData (const std::vector<float>& input, const std::vector<float>& target) {}

void NeuralNetwork::runTraining (int epochs)
{
    for (int i = 0; i < epochs; ++i)
    {
    }
}

torch::Tensor NeuralNetwork::forward (const torch::Tensor input)
{
    return linearLayer->forward (input);
}
