/*
 * server.h
 *
 *  Created on: Mar 16, 2012
 *      Author: asanoki
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/unordered_map.hpp>
#include <sstream>
#include <cstdlib>
#include <exception>
#include <libgram/solver.h>

#include "data.h"
#include "utf8codec.h"

using boost::asio::ip::tcp;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

class DataException : public std::exception {
	const char* what() const throw() {
		return "Data too big to fit designed buffer.";
	}
};

void session(libgram::Solver<wchar_t> &solver, socket_ptr sock) {
	try {
		boost::asio::streambuf response;
		std::istream response_stream(&response);
		std::wstringstream task_stream;
		wchar_t wchar_buffer[4096];
		int empty_lines_counter = 0;
		// bool locale_auto = true;

		boost::asio::ip::tcp::no_delay option(true);
		sock.get()->set_option(option);

		for (;;) {
			boost::system::error_code error;
			boost::asio::read(*sock, response,
					boost::asio::transfer_at_least(1), error);
			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.
			std::string line;
			std::wstring decoded_line;
			while (std::getline(response_stream, line)) {
				/*
				 if (line.compare("utf-8") == 0) {
				 locale_auto = false;
				 continue;
				 }
				 if (line.compare("auto") == 0) {
				 locale_auto = true;
				 continue;
				 }
				 */
				// std::cout << "Received: " << line << std::endl;
				// Recoding of the line
				// OBSOLETE: mbstowcs(wchar_buffer, line.c_str(), 4096); // TODO: Check off by one
				if (line.length() > 4096 - 1) {
					throw DataException();
				}
				encoding::importAsUtf8(wchar_buffer, line.c_str(), 4096 /* Check off by one for \0 */, line.length() + 1);
				decoded_line = wchar_buffer;
				if (decoded_line.empty()) {
					empty_lines_counter++;
					if (empty_lines_counter == 2) {
						// Got empty line, so we will process
						empty_lines_counter = 0;
//						std::cerr << "task received." << std::endl;
						// std::cout << "Task received." << std::endl;
						libgram::Query<wchar_t> query;
						data::loadQuery(task_stream, query);
						std::wstring result = solver.solve(query) + L"\n";
						// OBSOLETE: int length = wcstombs(output_buffer.get(), result.c_str(),
						// 		result.size() * 16); // TODO: Check off by one
						boost::shared_ptr<char> output_buffer(new char[(result.length() + 1) * 16]); // TODO: Possible buffer overflow for crazy encodings
						size_t length = encoding::exportAsUtf8(output_buffer.get(), result.c_str(),
								(result.length() + 1) * 16, result.length() + 1);
						boost::asio::write(
								*sock,
								boost::asio::buffer(output_buffer.get(),
										length));
						task_stream.clear();
					}
				} else {
					empty_lines_counter = 0;
				}
				task_stream << decoded_line.c_str();
				task_stream << L"\n";
			}
			response_stream.clear();
		}
	} catch (std::exception& e) {
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}

boost::shared_ptr<tcp::acceptor> server_init(
		boost::asio::io_service& io_service, short port) {
	return boost::shared_ptr<tcp::acceptor>(
			new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), port)));
}

void server_loop(libgram::Solver<wchar_t> &solver,
		boost::asio::io_service& io_service,
		boost::shared_ptr<tcp::acceptor> acceptor) {
	for (;;) {
		socket_ptr sock(new tcp::socket(io_service));
		acceptor->accept(*sock);
		boost::thread t(boost::bind(session, solver, sock));
	}
}

#endif /* SERVER_H_ */
