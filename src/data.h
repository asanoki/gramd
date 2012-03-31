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
#include <libgram/faststring.h>
#include <boost/unordered_map.hpp>

namespace data {

bool setUtf8InternalLocale();

int loadNGrams(std::string filename,
		boost::unordered_map<libgram::FastString<wchar_t>, double> &result);

bool loadQuery(std::wstringstream &data, libgram::Query<wchar_t> &query);

/*
bool saveCache(std::string filename,
		boost::unordered_map<libgram::FastString<wchar_t>, double> &container);

bool loadCache(std::string filename,
		boost::unordered_map<libgram::FastString<wchar_t>, double> &container);
*/

}

#endif /* DATA_H_ */
