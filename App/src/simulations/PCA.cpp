#include "Portfolio.h"
#include "math/numerical.h"

double Portfolio::PCA(
    timestamp start, int num_components,
    std::vector<std::pair<double, std::vector<double>>> &components) {
    if (num_components == -1) {
        num_components = assets.size();
        components.clear();
        components.resize(
            num_components,
            std::make_pair(0.0, std::vector<double>(num_components)));
    } else if (num_components < -1) {
        std::cerr << "Number of components must be positive!" << std::endl;
        return -1;
    } else if (num_components > static_cast<int>(assets.size())) {
        std::cerr << "Number of components exceeds the number of assets!"
                  << std::endl;
        return -1;
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
        throw std::runtime_error(
            "Eigen decomposition failed! The covariance matrix might not be "
            "symmetric or positive-definite.");
    }

    double total_variance = 0.0;
    double variance_explained = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double current_eigenvalue = eig.at(i).first;
        total_variance += current_eigenvalue;
        if (i < num_components) {
            components.at(i) = eig.at(i);
            variance_explained += current_eigenvalue;
        }
    }

    return variance_explained / total_variance;
}
