#include <stdio.h>
#include <torch/torch.h>
#include <iostream>
using namespace std;

int main() {
    if (torch::cuda::is_available()) {
        // GPU disponible, créez ou déplacez des tenseurs sur le GPU
        auto tensor = torch::rand({5, 3}, torch::device(torch::kCUDA));
        cout << "Tensor on GPU!";
    } else {
        // GPU non disponible, fallback sur le CPU
        auto tensor = torch::rand({5, 3}, torch::device(torch::kCPU));
        cout << "Tensor on GPU";
    }



    return 0;
}