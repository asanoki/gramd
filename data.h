/*
 * tools.h
 *
 *  Created on: Mar 16, 2012
 *      Author: asanoki
 */

#ifndef DATA_H_
#define DATA_H_

#include <iostream>
#include <libgram/query.h>
#include <boost/unordered_map.hpp>

namespace data {

bool setUtf8InternalLocale();

int loadNGrams(std::string filename,
		boost::unordered_map<std::wstring, double> &result, bool quiet);

bool loadQuery(std::wstringstream &data, libgram::Query<wchar_t> &query);

bool saveCache(std::string filename,
		boost::unordered_map<std::wstring, double> &container);

bool loadCache(std::string filename,
		boost::unordered_map<std::wstring, double> &container);

}

#endif /* DATA_H_ */
