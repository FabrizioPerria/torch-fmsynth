#include <iostream>
#include <torch/torch.h>

int main()
{
    // Create a tensor filled with zeros
    torch::Tensor tensor = torch::rand ({ 2, 3 });

    // Print the tensor
    std::cout << "Tensor: " << tensor << std::endl;

    return 0;
}
