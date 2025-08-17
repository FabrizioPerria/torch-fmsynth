#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork (int inputSize, int outputSize) : numInputs (inputSize), numOutputs (outputSize)
{
    auto net = torch::nn::Linear (numInputs, numOutputs);
    net->to (torch::kCPU);
    linearLayer = register_module ("linear", net);
}

std::vector<float> NeuralNetwork::forward (const std::vector<float>& input)
{
    static thread_local std::vector<float> output (numOutputs);
    static thread_local torch::Tensor inputTensor = torch::empty ({ 1, numInputs }, torch::kFloat);

    std::memcpy (inputTensor.data_ptr<float>(), input.data(), numInputs * sizeof (float));

    torch::NoGradGuard no_grad;
    torch::Tensor outputTensor = forward (inputTensor);

    std::memcpy (output.data(), outputTensor.data_ptr<float>(), numOutputs * sizeof (float));

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
    auto out = torch::empty ({ 1, 2 }, torch::kFloat);
    out[0][0] = 0.4f;
    out[0][1] = 0.7f;
    return out;
}
