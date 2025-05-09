#include "Alignment_test.h"
#include "Math_test.h"
#include "Parser_test.h"
#include "Portfolio.h"
#include <iostream>

int main(void) {
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
	Doubles_Matrix aligned = alignment_test(test);

	//	for (const auto &row : aligned) {
	//		for (const auto &value : row) {
	//			std::cout << value << ' ';
	//		}
	//		std::cout << std::endl;
	//	}
	//	std::cout << aligned.size() << " x " << aligned.at(0).size() <<
	// std::endl;

	std::cout << "Math test:" << std::endl;
	std::vector<double> means = mean_test(test);

	std::cout << "Means:" << std::endl;
	for (const auto &value : means) {
		std::cout << value << ' ';
	}
	std::cout << std::endl;
	return 0;
	Doubles_Matrix cov = covariance_test(test);

	std::cout << "Covariance:" << std::endl;
	for (const auto &row : cov) {
		for (const auto &value : row) {
			std::cout << value << ' ';
		}
		std::cout << std::endl;
	}

	return 0;
}
