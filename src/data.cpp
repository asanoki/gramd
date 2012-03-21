/*
 * tools.cpp
 *
 *  Created on: Mar 16, 2012
 *      Author: asanoki
 */

#include <fstream>
#include <sstream>
#include <cassert>
#include <boost/progress.hpp>
#include <boost/shared_ptr.hpp>

#include "data.h"
#include "encoding.h"

namespace data {

int loadNGrams(std::string filename,
		boost::unordered_map<std::wstring, double> &result, bool quiet = false) {
	// Loading n-grams from a file
	std::wifstream data(filename.c_str());
	data.imbue(*encoding::getUtf8Locale());
	if (!data.good())
		return -1;
	data.seekg(0, std::ios::end);
	int input_size = data.tellg();
	data.seekg(0, std::ios::beg);
	boost::progress_display show_progress(input_size);
	boost::shared_ptr<boost::progress_timer> timer;
	if (!quiet)
		timer.reset(new boost::progress_timer());
	std::wstring line;
	std::wstring line_gram;
	int line_value;
	unsigned int n = 0;
	data >> n;
	if (n <= 0) {
		return -2;
	}
	int line_no = 0;
	int before_pos = 0;
	while (!data.eof()) {
		line_no++;
		std::getline(data, line);
		if (!quiet) {
			if (line_no % 10000 == 0) {
				show_progress += ((int) data.tellg() - before_pos);
				before_pos = data.tellg();
			}
		}
		if (line.length() < n)
			continue;
		line_gram = line.substr(0, n);
		std::wstringstream stream(
				line.substr(n + 1, line.length() - n - 1).c_str());
		stream >> line_value;
		result[line_gram] = line_value;
	}
	if (!quiet) {
		show_progress.operator +=(input_size - show_progress.count());
	}
	return n;
}

bool loadQuery(std::wstringstream &data, libgram::Query<wchar_t> &query) {
	// Loading query from a file
	if (!data.good())
		return false;
	std::wstring line_character;
	double line_probability;
	std::vector<wchar_t> labels;
	std::vector<double> emission_probabilities;
	std::wstring line;
	bool data_started = false;
	while (std::getline(data, line)) {
		if (line.length() < 3) {
			// Empty line
			if (labels.size() > 0) {
				libgram::QuerySection<wchar_t> query_section(
						emission_probabilities, labels);
				query.addSection(query_section);
				labels.clear();
				emission_probabilities.clear();
			} else {
				if (data_started) {
					// End of data
					break;
				} else {
					// Ignore empty lines in the beginning
				}
			}
		} else {
			std::wstringstream stream(line);
			data_started++;
			stream >> line_character;
			stream >> line_probability;
			// std::wcout << "Char: " << line_character << ", P: " << line_probability << std::endl;
			assert(line_character.length() > 0);
			labels.push_back(line_character[0]);
			emission_probabilities.push_back(line_probability);
		}
	}
	return true;
}

bool saveCache(std::string filename,
		boost::unordered_map<std::wstring, double> &container) {
	std::ofstream data(filename.c_str(), std::ios::binary);
	if (!data.good())
		return false;
	for (boost::unordered_map<std::wstring, double>::iterator it =
			container.begin(); it != container.end(); it++) {
		unsigned short string_size = it->first.length();
		data.write((char *) &string_size, sizeof(unsigned short));
		data.write(
				(char *) (it->first.c_str()), it->first.length() * sizeof(wchar_t));
		data.write((char *) &it->second, sizeof(double));
	}
	data.close();
	return true;
}

bool loadCache(std::string filename,
		boost::unordered_map<std::wstring, double> &container) {
	std::ifstream data(filename.c_str(), std::ios::binary);
	if (!data.good())
		return false;
	wchar_t *buffer[65536 + 1];while
(	!data.eof()) {
		unsigned short string_size;
		std::wstring key;
		double value;
		data.read((char *)&string_size, sizeof(string_size));
		buffer[string_size] = 0;
		key = std::wstring((wchar_t *)buffer);
		data.read((char *)&value, sizeof(double));
		container[key] = value;
	}
	data.close();
	return true;
}

}
