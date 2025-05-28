#include "Math_test.h"
#include "math/numerical.h"

std::vector<double> mean_test(Portfolio &test, timestamp start) {
    // Test the mean function
    int info = test.calculate_aligned_metrics(start);
    if (info != 0) {
        std::cerr << "Error calculating metrics" << std::endl;
        return {};
    }
    return test.aligned_means;
}

Doubles_Matrix covariance_test(Portfolio &test, timestamp start) {
    // Test the covariance function
    Doubles_Matrix aligned = test.aligned_log_returns(start);
    Doubles_Matrix cov = test.calculate_covariance(start);
    return cov;
}

Doubles_Matrix cholesky_test(Portfolio &test, timestamp start) {
    // Test the Cholesky decomposition function
    Doubles_Matrix cov = test.calculate_covariance(start);
    Doubles_Matrix cholesky(cov.size(), std::vector<double>(cov.size(), 0.0));
    math::cholesky_decomposition(cov, cholesky);
    return cholesky;
}

int QR_decomposition_test(Portfolio &test, timestamp start) {
    Doubles_Matrix cov = test.calculate_covariance(start);
    Doubles_Matrix Q, R;
    int info = math::QR_decomposition(cov, Q, R);
    if (info != 0) {
        std::cerr << "Error in QR decomposition" << std::endl;
        return info;
    }
    //    std::cout << "Q matrix:" << std::endl;
    //    for (auto &row : Q) {
    //        for (auto &val : row) {
    //            std::cout << val << " ";
    //        }
    //        std::cout << std::endl;
    //    }
    //    std::cout << "R matrix:" << std::endl;
    //    for (auto &row : R) {
    //        for (auto &val : row) {
    //            std::cout << val << " ";
    //        }
    //        std::cout << std::endl;
    //    }
    //
    //    std::cout << "A matrix:" << std::endl;
    //    for (auto &row : cov) {
    //        for (auto &val : row) {
    //            std::cout << val << " ";
    //        }
    //        std::cout << std::endl;
    //    }
    //    std::cout << "Q * R:" << std::endl;
    //    Doubles_Matrix QR = math::matrix_multiply(Q, R);
    //    for (auto &row : QR) {
    //        for (auto &val : row) {
    //            std::cout << val << " ";
    //        }
    //        std::cout << std::endl;
    //    }

    return 0;
}

std::vector<std::pair<double, std::vector<double>>>
eigen_test(Portfolio &test, timestamp start) {
    // Doubles_Matrix cov = test.calculate_covariance(start);
    Doubles_Matrix cov = {{6, 5.5, -1}, {5.5, 1, -2}, {-1, -2, -3}};
    size_t n = cov.size();

    std::vector<std::pair<double, std::vector<double>>> eig_pairs(
        n,
        std::pair<double, std::vector<double>>(1, std::vector<double>(n, 0)));

    int info = math::eigen_pairs(cov, eig_pairs);
    if (info != 0) {
        std::cerr << "Error in eigen decomposition" << std::endl;
        return {};
    }
    return eig_pairs;
}
