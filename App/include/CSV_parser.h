#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "Cryptocurrency.h"
#include <filesystem>
#include <fstream>

class CSV_Parser {
  public:
    Cryptocurrency fastReadCryptoCSV(const std::string &filename) {
        // Get file size for preallocation
        std::filesystem::path p(filename);
        size_t fileSize = std::filesystem::file_size(p);

        // Estimate lines
        const size_t avgLineLength = 128;
        size_t estimatedLines = fileSize / avgLineLength;

        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        // Preallocate the vector with our estimate
        Cryptocurrency data;
        data.hist_data.reserve(estimatedLines);

        // Buffer for efficient reading
        const size_t bufferSize = 16384; // 16KB buffer
        char *buffer = new char[bufferSize];
        file.rdbuf()->pubsetbuf(buffer, bufferSize);

        std::string line;
        bool first_line = true, first_reading = true;
        while (std::getline(file, line)) {
            if (first_line) {
                // Skip the header line
                first_line = false;
                continue;
            }

            double high, low, open, close, volume, marketcap;
            timestamp timestamp;

            // Fast parsing with manual index tracking
            size_t pos = 0;
            size_t nextComma = 0;

            // Process Name and Tick on the first reading only
            if (first_reading) {
                first_reading = false;

                // Dummy (column 0)
                nextComma = line.find(',', pos);
                pos = nextComma + 1;

                // Parse Name (column 1)
                nextComma = line.find(',', pos);
                data.name = line.substr(pos, nextComma - pos);
                pos = nextComma + 1;

                // Parse Tick (column 2)
                nextComma = line.find(',', pos);
                data.tick = line.substr(pos, nextComma - pos);
                pos = nextComma + 1;
            } else {
                // Dummy (column 0)
                nextComma = line.find(',', pos);
                pos = nextComma + 1;

                // Parse Name (column 1)
                nextComma = line.find(',', pos);
                pos = nextComma + 1;

                // Parse Tick (column 2)
                nextComma = line.find(',', pos);
                pos = nextComma + 1;
            }

            // Parse DateTime (column 3)
            nextComma = line.find(',', pos);
            try {
                timestamp =
                    string_to_timestamp(line.substr(pos, nextComma - pos));
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            pos = nextComma + 1;

            // Parse High (column 5)
            nextComma = line.find(',', pos);
            high = std::stod(line.substr(pos, nextComma - pos));
            pos = nextComma + 1;

            // Parse Low (column 6)
            nextComma = line.find(',', pos);
            low = std::stod(line.substr(pos, nextComma - pos));
            pos = nextComma + 1;

            // Parse Open (column 7)
            nextComma = line.find(',', pos);
            open = std::stod(line.substr(pos, nextComma - pos));
            pos = nextComma + 1;

            // Parse Close (column 8)
            nextComma = line.find(',', pos);
            close = std::stod(line.substr(pos, nextComma - pos));
            pos = nextComma + 1;

            // Parse Volume (column 9)
            nextComma = line.find(',', pos);
            volume = std::stod(line.substr(pos, nextComma - pos));
            pos = nextComma + 1;

            // Parse Market Cap (column 10, last column)
            marketcap = std::stod(line.substr(pos));

            data.hist_data.emplace_back(open, high, low, close, volume,
                                        marketcap, timestamp);
        }
        file.close();
        delete[] buffer;
        return data;
    }
};

#endif // CSV_PARSER_H
