#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork (int inputSize, int outputSize) : numInputs (inputSize), numOutputs (outputSize)
{
    auto net = torch::nn::Linear (numInputs, numOutputs);
    net->to (torch::kCPU);
    linearLayer = register_module ("linear", net);

    softmaxLayer = register_module ("softmax", torch::nn::Softmax (1));
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

void NeuralNetwork::addTrainingData (const std::vector<float>& input, const std::vector<float>& target)
{
    assert (input.size() == numInputs);
    assert (target.size() == numOutputs);
    torch::Tensor inT = torch::from_blob (const_cast<float*> (input.data()), numInputs, torch::kFloat).clone();
    torch::Tensor targetT = torch::from_blob (const_cast<float*> (target.data()), numOutputs, torch::kFloat).clone();
    std::cout << targetT << std::endl << inT << std::endl;
    trainingInputs.push_back (inT);
    trainingTargets.push_back (targetT);
}

void NeuralNetwork::runTraining (int epochs)
{
    for (int i = 0; i < epochs; ++i)
    {
    }
}

torch::Tensor NeuralNetwork::forward (const torch::Tensor input)
{
    auto out = linearLayer (input);
    out = softmaxLayer (out);
    return out;
}
