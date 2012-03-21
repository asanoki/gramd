/*
 * encoding.cpp
 *
 *  Created on: Mar 19, 2012
 *      Author: asanoki
 */

#ifndef ENCODING_CPP_
#define ENCODING_CPP_

#include "encoding.h"
#include <cwchar>
#include <string>
#include <cassert>

#include <cstdio>

namespace encoding {

static boost::shared_ptr<boost::archive::detail::utf8_codecvt_facet> utf8_codec;
static boost::shared_ptr<std::locale> utf8_locale;

template<typename A>
class CascadeDeleter {
private:
	boost::shared_ptr<A> &m_second;
public:
	CascadeDeleter(boost::shared_ptr<A> &second) :
			m_second(second) {
	}
	void operator()(void *ptr) const {
		free(ptr);
	}
};

const std::locale *getUtf8Locale() {
	return utf8_locale.get();
}

const boost::archive::detail::utf8_codecvt_facet *getUtf8Codec() {
	return utf8_codec.get();
}

bool setUtf8InternalLocale() {
	static std::locale old_locale;
	utf8_codec.reset(new boost::archive::detail::utf8_codecvt_facet(),
			CascadeDeleter<std::locale>(utf8_locale));
	utf8_locale.reset(new std::locale(old_locale, utf8_codec.get()));
	return true; // not used
}

size_t importAsUtf8(wchar_t *output, const char *input, size_t output_size,
		size_t input_size) {
	const char *pc = 0;
	wchar_t *pwc = 0;
	std::codecvt<wchar_t, char, mbstate_t>::result result;
	std::mbstate_t state = std::mbstate_t();
	boost::archive::detail::utf8_codecvt_facet *codec_ptr = utf8_codec.get();
	result = codec_ptr->in(state, input, input + input_size, pc, output,
			output + output_size, pwc);
	if (result != (std::codecvt<wchar_t, char, mbstate_t>::ok)) {
		throw ConversionException();
	}
	size_t result_size = 0;
	while (*output++) {
		result_size++;
	}
	return result_size;
}

size_t exportAsUtf8(char *output, const wchar_t *input, size_t output_size,
		size_t input_size) {
	const wchar_t *pwc = 0;
	char *pc = 0;
	std::codecvt<wchar_t, char, mbstate_t>::result result;
	std::mbstate_t state = std::mbstate_t();
	boost::archive::detail::utf8_codecvt_facet *codec_ptr = utf8_codec.get();
	result = codec_ptr->out(state, input, input + input_size, pwc, output,
			output + output_size, pc);
	if (result != (std::codecvt<wchar_t, char, mbstate_t>::ok)) {
		throw ConversionException();
	}
	size_t result_size = 0;
	while (*output++) {
		result_size++;
	}
	return result_size;
}

}

#endif /* ENCODING_CPP_ */
