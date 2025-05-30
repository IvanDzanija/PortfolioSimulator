#ifndef DATETIME_FORMATTING_H
#define DATETIME_FORMATTING_H
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
	timestamp;

inline std::string
timestamp_to_string(const std::chrono::time_point<std::chrono::system_clock,
												  std::chrono::seconds> &tp) {
	std::time_t t = std::chrono::system_clock::to_time_t(tp);

	std::tm tm = *std::gmtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

inline timestamp string_to_timestamp(const std::string &datetime) {
	std::tm tm = {};
	std::istringstream in(datetime);
	in >> std::get_time(&tm, "%Y-%m-%d");

	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	if (!in)
		throw std::runtime_error("Failed to parse datetime");
	std::time_t t = std::mktime(&tm);
	return std::chrono::time_point_cast<std::chrono::seconds>(
		std::chrono::system_clock::from_time_t(t));
}

#endif // DATETIME_FORMATTING_H
