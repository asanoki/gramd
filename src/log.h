/*
 * log.h
 *
 *  Created on: Mar 31, 2012
 *      Author: asanoki
 */

#ifndef LOG_H_
#define LOG_H_

#include <iostream>

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

typedef std::wostream& (*ostream_manipulator)(std::wostream&);

inline Log& operator<<(Log& os, ostream_manipulator pf) {
	return operator<<<ostream_manipulator>(os, pf);
}

#endif /* LOG_H_ */
