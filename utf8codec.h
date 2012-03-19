/*
 * utf8codec.h
 *
 *  Created on: Mar 19, 2012
 *      Author: asanoki
 */

#ifndef UTF8CODEC_H_
#define UTF8CODEC_H_

#define BOOST_UTF8_BEGIN_NAMESPACE \
     namespace boost { namespace archive { namespace detail {
#define BOOST_UTF8_DECL
#define BOOST_UTF8_END_NAMESPACE }}}
#include <boost/detail/utf8_codecvt_facet.hpp>

#endif /* UTF8CODEC_H_ */
