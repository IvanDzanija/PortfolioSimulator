#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class CSVParser {
  public:
	// Method to parse a CSV file and return the data as a vector of vectors of
	// strings

	static std::vector<std::vector<std::string>>
	fastReadCustomCsv(const std::string &filename) {
		// Get file size for preallocation
		std::filesystem::path p(filename);
		size_t fileSize = std::filesystem::file_size(p);

		// Open file
		std::ifstream file(filename, std::ios::in | std::ios::binary);
		if (!file) {
			throw std::runtime_error("Could not open file: " + filename);
		}

		// Count lines quickly (faster than getline in a loop)
		size_t lineCount = 0;
		char buffer[8192]; // Use a buffer for faster reading
		while (file) {
			file.read(buffer, sizeof(buffer));
			size_t count = file.gcount();
			for (size_t i = 0; i < count; ++i) {
				if (buffer[i] == '\n') {
					++lineCount;
				}
			}
		}

		// Reset file position
		file.clear();
		file.seekg(0, std::ios::beg);

		// Preallocate the main vector
		std::vector<std::vector<std::string>> data;
		data.reserve(lineCount);

		// Expected number of columns (replace with your known value)
		const size_t expectedColumns = 10; // Adjust based on your data

		std::string line;
		while (std::getline(file, line)) {
			std::vector<std::string> row;
			row.reserve(expectedColumns);

			size_t start = 0;
			size_t end = 0;

			// Fast parsing by finding delimiters and using string_view-like
			// approach
			while ((end = line.find(',', start)) != std::string::npos) {
				row.push_back(line.substr(start, end - start));
				start = end + 1;
			}

			// Add the last field
			row.push_back(line.substr(start));

			data.push_back(std::move(row));
		}

		return data;
	}

	int main() {
		auto data = fastReadCustomCsv("yourfile.csv");
		std::cout << "Read " << data.size() << " rows" << std::endl;
		return 0;
	}
	static std::vector<std::vector<std::string>>
	parse(const std::string &filename) {
		std::vector<std::vector<std::string>> data;
		std::ifstream file(filename);
		std::string line;

		while (std::getline(file, line)) {
			std::vector<std::string> row;
			std::stringstream lineStream(line);
			std::string cell;

			while (std::getline(lineStream, cell, ',')) {
				row.push_back(cell);
			}

			data.push_back(row);
		}

		return data;
	}
};

#endif // CSV_PARSER_H
