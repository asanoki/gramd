/*
 * log.h
 *
 *  Created on: Mar 31, 2012
 *      Author: asanoki
 */

#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include <ostream>

class Log;

typedef std::wostream& (*manip1)(std::wostream&);

typedef std::basic_ios<std::wostream::char_type, std::ostream::traits_type> ios_type;
typedef ios_type& (*manip2)(ios_type&);

typedef std::ios_base& (*manip3)(std::ios_base&);

class Log {
private:
	std::wostream &m_stream;
	bool m_enabled;

public:
	static Log debug;
	static Log info;
	static Log message;
	static Log notice;
	static Log warning;
	static Log error;

	void enable() {
		m_enabled = true;
	}

	void disable() {
		m_enabled = false;
	}

	bool enabled() {
		return m_enabled;
	}

	Log& operator<<(manip1 pf) {
		if (m_enabled)
			m_stream << pf;
	}

	Log& operator<<(manip2 pf) {
		if (m_enabled)
			m_stream << pf;
	}

	Log& operator<<(manip3 pf) {
		if (m_enabled)
			m_stream << pf;
	}

	std::wostream &stream() {
		return m_stream;
	}

	Log(std::wostream &stream) :
			m_stream(stream) {
	}
};

template<typename T>
inline Log &operator<<(Log &l, const T &t) {
	if (l.enabled()) {
		l.stream() << t;
	}
	return l;
}

#endif /* LOG_H_ */
