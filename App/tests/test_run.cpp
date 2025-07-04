#include "Alignment_test.h"
#include "Math_test.h"
#include "PCA_test.h"
#include "Parser_test.h"
#include "Portfolio.h"
#include "math/numerical.h"
#include <iostream>

int main(void) {
    timestamp start = std::chrono::floor<std::chrono::seconds>(
        std::chrono::system_clock::now());
    std::cout << "Running tests..." << std::endl;

    Portfolio test = parser_test();
    // std::cout << "Parser test:" << std::endl;
    // for (auto x : test.assets) {
    // 	std::cout << "Coin name: " << x.first.name << std::endl;
    // 	std::cout << "Coin tick: " << x.first.tick << std::endl;
    // 	std::cout << "Coin prices:" << std::endl;
    // 	for (auto y : x.first.hist_data) {
    // 		std::cout << y.high << ' ' << y.low << ' ' << y.open << ' '
    // 				  << y.close << std::endl;
    // 		std::cout << "Coin log return: " << y.log_return << std::endl;
    // 		std::cout << "Coin market cap: " << y.marketcap << std::endl;
    // 		std::cout << "Coin volume: " << y.volume << std::endl;
    // 		std::cout << "Coin timestamp: "
    // 				  << y.timestamp.time_since_epoch().count() << std::endl;
    // 	}
    // }
    // std::cout << "Alignment test:" << std::endl;
    Doubles_Matrix aligned = alignment_test(test, start);
    Doubles_Matrix remainder = remainder_test(test, start);
    //	for (const auto &row : aligned) {
    //		for (const auto &value : row) {
    //			std::cout << value << ' ';
    //		}
    //		std::cout << std::endl;
    //	}
    //	std::cout << aligned.size() << " x " << aligned.at(0).size() <<
    // std::endl;
    for (const auto &row : remainder) {
        for (const auto &value : row) {
            std::cout << value << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << "Math test:" << std::endl;

    std::vector<double> means = mean_test(test, start);

    // std::cout << "Means:" << std::endl;
    //	for (const auto &value : means) {
    //		std::cout << value << ' ';
    //	}
    //	std::cout << std::endl;
    Doubles_Matrix cov = covariance_test(test, start);

    // std::cout << "Covariance:" << std::endl;
    //	for (const auto &row : cov) {
    //		for (const auto &value : row) {
    //			std::cout << value << ' ';
    //		}
    //		std::cout << std::endl;
    //	}

    Doubles_Matrix cholesky = cholesky_test(test, start);
    //  std::cout << "Cholesky:" << std::endl;
    //  for (const auto &row : cholesky) {
    //      for (const auto &value : row) {
    //      std::cout << value << ' ';
    //      }
    //      std::cout << std::endl;
    //      }
    //

    std::cout << "QR decomposition:" << std::endl;
    int info = QR_decomposition_test(test, start);
    if (info != 0) {
        std::cerr << "Error in QR decomposition" << std::endl;
        return info;
    }

    std::cout << "Eigen decomposition:" << std::endl;
    std::vector<std::pair<double, std::vector<double>>> eig_pairs =
        eigen_test(test, start);

    //  for (auto pair : eig_pairs) {
    //      std::cout << "Eigenvalue: " << pair.first << std::endl;
    //      std::cout << "Correspoding eigenvector:" << std::endl;
    //      std::cout << '(';
    //      bool f = false;
    //      for (double x : pair.second) {
    //          if (!f) {
    //              f = true;
    //          } else {
    //              std::cout << ' ';
    //          }
    //          std::cout << x;
    //      }
    //      std::cout << ')' << std::endl;
    //  }

    std::cout << "Monte Carlo test:" << std::endl;
    int simulations = 10000;
    int steps = 100;
    std::vector<Doubles_Matrix> monte_carlo =
        test.monte_carlo(simulations, steps, start);
    //	std::cout << "Monte Carlo:" << std::endl;
    //	for (const auto &simulation : monte_carlo) {
    //		for (const auto &row : simulation) {
    //			for (const auto &value : row) {
    //				std::cout << value << ' ';
    //			}
    //			std::cout << std::endl;
    //			break;
    //		}
    //		std::cout << std::endl;
    //		break;
    //	}

    std::cout << "PCA test:" << std::endl;
    int pca_result = PCA_test(test, start);
    if (pca_result != 0) {
        std::cerr << "PCA test failed with code: " << pca_result << std::endl;
    }

    return 0;
}
