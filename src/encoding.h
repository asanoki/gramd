/*
 * encoding.h
 *
 *  Created on: Mar 19, 2012
 *      Author: asanoki
 */

#ifndef ENCODING_H_
#define ENCODING_H_

#include "utf8codec.h"
#include <boost/shared_ptr.hpp>
#include <exception>

namespace encoding {

const std::locale *getUtf8Locale();

const boost::archive::detail::utf8_codecvt_facet *getUtf8Codec();

bool setUtf8InternalLocale();

size_t importAsUtf8(wchar_t *output, const char *input, size_t output_size,
		size_t input_size);

size_t exportAsUtf8(char *output, const wchar_t *input, size_t output_size,
		size_t input_size);

class ConversionException: public std::exception {
	const char* what() const throw() {
		return "Unable to perform conversion.";
	}
};

}

#endif /* ENCODING_H_ */
