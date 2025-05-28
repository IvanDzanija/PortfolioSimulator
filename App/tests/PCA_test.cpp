#include "PCA_test.h"
#include <chrono>
#include <numeric>

int PCA_test(Portfolio &portfolio, timestamp start) {
    int num_components = -1; // Default to all components
    std::vector<std::pair<double, std::vector<double>>> components;

    auto s = std::chrono::high_resolution_clock::now();
    double variance_explained =
        portfolio.PCA(start, num_components, components);

    auto e = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(e - s);

    std::cout << duration.count() << " ms" << std::endl;
    std::cout << "Variance explained by PCA: " << variance_explained * 100.0
              << "%" << std::endl;

    double total_variance = std::accumulate(
        components.begin(), components.end(), 0.0,
        [](double sum, const std::pair<double, std::vector<double>> &p) {
            return sum + p.first;
        });
    double variance = 0.0;
    for (size_t i = 0; i < components.size(); ++i) {
        variance += components[i].first;
        std::cout << "Variance explained by the first " << i + 1
                  << " components: " << variance / total_variance * 100.0 << "%"
                  << std::endl;
    }
    return 0;
}
