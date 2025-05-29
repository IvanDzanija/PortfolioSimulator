#include "PCA_test.h"
#include <chrono>
#include <numeric>

int PCA_test(Portfolio &portfolio, timestamp start) {
    int num_components = -1; // Default to all components
    std::vector<std::pair<double, std::vector<double>>> components;

    auto s = std::chrono::high_resolution_clock::now();
    double total_variance = 0.0;
    double variance_explained = 0.0;
    int info = portfolio.PCA(start, num_components, components, total_variance,
                             variance_explained);
    if (info != 0) {
        std::cerr << "PCA analysis failed with error code: " << info
                  << std::endl;
        return info;
    }

    auto e = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(e - s);

    std::cout << duration.count() << " ms" << std::endl;
    std::cout << "Variance explained by PCA: " << variance_explained * 100.0
              << "%" << std::endl;

    double variance = 0.0;
    for (size_t i = 0; i < components.size(); ++i) {
        variance += components[i].first;
        std::cout << "Variance explained by the first " << i + 1
                  << " components: " << variance / total_variance * 100.0 << "%"
                  << std::endl;
    }
    return 0;
}
