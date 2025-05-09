#include "CSV_parser.h"
#include "Portfolio.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(void) {
	CSV_Parser parser;

	std::string folder_path = "../datasets/";

	for (const auto &entry : fs::directory_iterator(folder_path)) {
		if (entry.is_regular_file()) {
			std::cout << "Parsing file: " << entry.path().filename()
					  << std::endl;

			Cryptocurrency crypto =
				parser.fastReadCryptoCSV(fs::absolute(entry.path()).string());
			Portfolio test;
			test.add_asset(crypto, 1);

			for (auto x : test.assets) {
				std::cout << "Coin name: " << x.first.name << std::endl;
				std::cout << "Coin tick: " << x.first.tick << std::endl;
				std::cout << "Coin prices:" << std::endl;
				for (auto y : x.first.hist_data) {
					std::cout << y.high << ' ' << y.low << ' ' << y.open << ' '
							  << y.close << ' ' << y.log_return << std::endl;
					std::cout << "Coin volume: " << y.volume << std::endl;
					std::cout << "Coin timestamp: "
							  << y.timestamp.time_since_epoch().count()
							  << std::endl;
				}
			}
		}
	}

	return 0;
}
