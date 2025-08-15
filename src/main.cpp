#include <iostream>
#include <random>
#include <torch/torch.h>

std::vector<std::pair<float, float>> getLine (float bias, float weight, float startX, float endX, float count)
{
    float y { 0 };
    std::vector<std::pair<float, float>> line;
    float dX = (endX - startX) / count;
    for (float x = startX; x < endX; x += dX)
    {
        y = bias + weight * x;
        line.push_back (std::make_pair (x, y));
    }
    return line;
}

void testLine()
{
    float bias = 0.0f;
    float weight = 0.0f;
    float startX = 0.0f;
    float endX = 10.0f;
    float count = 10.0f;

    auto line = getLine (bias, weight, startX, endX, count);
    assert (line.size() == count);
    assert (std::all_of (line.begin(), line.end(), [] (const auto& point) { return point.second == 0.0f; }));

    weight = 1.0f;
    line = getLine (bias, weight, startX, endX, count);
    assert (line.size() == count);
    assert (std::all_of (line.begin(), line.end(), [] (const auto& point) { return point.second == point.first; }));

    bias = 1.0f;
    line = getLine (bias, weight, startX, endX, count);
    assert (line.size() == count);
    assert (std::all_of (line.begin(), line.end(), [] (const auto& point) { return point.second == 1.0f + point.first; }));

    weight = -2.0f;
    line = getLine (bias, weight, startX, endX, count);
    assert (line.size() == count);
    assert (std::all_of (line.begin(), line.end(), [] (const auto& point) { return point.second == 1.0f - 2 * point.first; }));

    std::cout << "All tests passed!" << std::endl;
}

void addNoiseToLine (std::vector<std::pair<float, float>>& line, float lowThreshold, float highThreshold)
{
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution (lowThreshold, highThreshold);
    auto rand = std::bind (distribution, generator);
    std::for_each (line.begin(), line.end(), [&rand] (auto& point) { point.second += rand(); });
}

void showNoisyLine()
{
    float bias = 1.0f;
    float weight = 2.0f;
    float startX = 0.0f;
    float endX = 10.0f;
    float count = 10.0f;
    auto line = getLine (bias, weight, startX, endX, count);
    addNoiseToLine (line, -0.5f, 0.5f);
    for (const auto& point : line)
    {
        std::cout << "x: " << point.first << ", y: " << point.second << std::endl;
    }
}

int main()
{
    testLine();
    showNoisyLine();
    // Create a tensor filled with zeros
    torch::Tensor tensor = torch::rand ({ 2, 3 });

    // Print the tensor
    std::cout << "Tensor: " << tensor << std::endl;

    return 0;
}
