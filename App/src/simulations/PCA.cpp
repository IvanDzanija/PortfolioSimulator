#include "Portfolio.h"
#include "math/numerical.h"
#include <numeric>

int Portfolio::PCA(
    timestamp start, int num_components,
    std::vector<std::pair<double, std::vector<double>>> &components,
    double &total_variance, double &variance_explained) {
    std::cout << "Starting PCA analysis..." << std::endl;
    if (num_components == -1) {
        num_components = _assets.size();
        components.clear();
        components.resize(
            num_components,
            std::make_pair(0.0, std::vector<double>(num_components)));
    } else if (num_components < -1) {
        std::cerr << "Number of components must be positive!" << std::endl;
        return -1;
    } else if (num_components > static_cast<int>(_assets.size())) {
        std::cerr << "Number of components exceeds the number of assets!"
                  << std::endl;
        return -1;
    } else {
        components.clear();
        components.resize(
            num_components,
            std::make_pair(0.0, std::vector<double>(_assets.size())));
    }
    Doubles_Matrix covariance = calculate_covariance(start);

    if (covariance.empty()) {
        std::cerr << "Starting date mismatch. Some assets probably didn't "
                     "exist as of set starting date!"
                  << std::endl;
        return -1;
    }

    size_t n = covariance.size(); // number of assets
    if (n == 0) {
        std::cerr << "Covariance matrix is empty!" << std::endl;
        return -1;
    }

    // Calculate eigenvalues and eigenvectors

    std::vector<std::pair<double, std::vector<double>>> eig(
        n, std::pair<double, std::vector<double>>(1, std::vector<double>(n)));
    if (math::eigen_pairs(covariance, eig)) {
        std::cerr << "Eigen pairs calculation failed!" << std::endl;
        return -1;
    }
    std::sort(eig.begin(), eig.end(), std::greater<>());
    total_variance = std::accumulate(
        eig.begin(), eig.end(), 0.0,
        [](double sum, const std::pair<double, std::vector<double>> &p) {
            return sum + p.first;
        });

    for (size_t i = 0; i < n; ++i) {
        double current_eigenvalue = eig.at(i).first;
        if (i < num_components) {
            components.at(i) = eig.at(i);
            variance_explained += current_eigenvalue;
        }
    }
    return 0;
}
