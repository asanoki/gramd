/*
 * gramd.cpp
 *
 *  Created on: Mar 16, 2012
 *      Author: asanoki
 */

#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <locale>

#include <boost/program_options.hpp>
#include <boost/unordered_map.hpp>
#include <boost/progress.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/collections_load_imp.hpp>
#include <boost/serialization/collections_save_imp.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <libgram/solver.h>
#include <libgram/providers/simpleprovider.h>
#include <unistd.h>

#include "data.h"
#include "encoding.h"
#include "server.h"

namespace po = boost::program_options;

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, std::pair<const std::wstring, double>& s
		, const unsigned int version) {
	ar & s.first;
	ar & s.second;
}

template<class Archive>
void unserialize(Archive & ar, std::pair<const std::wstring, double>& s
		, const unsigned int version) {
	ar | s.first;
	ar | s.second;
}

} // namespace serialization
} // namespace boost

int main(int argc, char **argv) {
	try {
		// Main object
		libgram::Solver<wchar_t> solver;

		// Additional variables
		bool quiet = false;

		// Initialize
		po::options_description desc;
		desc.add_options()("epsilon", po::value<double>(), "epsilon value");
		desc.add_options()("help", "display help message");
		desc.add_options()("verbose,v", "output useful information");
		desc.add_options()("interactive,i", "do not detach");
		desc.add_options()("quiet,q", "do not display any startup messages");
		desc.add_options()("port,p", po::value<short>(), "set listening port");
		desc.add_options()("auto,a", po::value<std::string>(),
				"filename to store automatically chosen port");
		desc.add_options()("addr,a", "set listening address");
		desc.add_options()("locale,l", po::value<std::string>(),
				"define custom locale");
		desc.add_options()("load,L", po::value<std::vector<std::string> >(),
				"dictionaries with n-grams (--load or -L can be omitted)");

		po::positional_options_description p;
		p.add("load", -1);

		po::variables_map vm;
		po::store(
				po::command_line_parser(argc, argv).options(desc).positional(p).run(),
				vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc;
			return -1;
		}

		if (vm.count("quiet")) {
			quiet = true;
		}

		// Set dictionary locale
		encoding::setUtf8InternalLocale();

		// Configure global locale
		if (vm.count("locale")) {
			std::string locale_name = vm["locale"].as<std::string>();
			try {
				std::locale::global(std::locale(locale_name.c_str()));
				setlocale(LC_ALL, locale_name.c_str());
			} catch (...) {
				std::cerr << "Unable to set locale to: " << locale_name
						<< std::endl;
				return -1;
			}
		} else {
			// Default locale
			try {
				std::locale::global(std::locale(""));
				setlocale(LC_ALL, "");
			} catch (...) {
				std::cerr << "Unable to set to system locale. Trying C instead."
						<< std::endl;
				std::locale::global(std::locale("C"));
				setlocale(LC_ALL, "C");
			}
		}

		// Create listening socket
		if ((!vm.count("port") && !vm.count("auto")) || !vm.count("load")) {
			std::cout << desc;
			return -1;
		}

		unsigned short port;
		boost::shared_ptr<tcp::acceptor> acceptor;
		boost::asio::io_service io_service;
		int retries = 5000;
		if (vm.count("port")) {
			port = vm["port"].as<short>();
			acceptor = server_init(io_service, port);
		} else if (vm.count("auto")) {
			for (int retry = 0; retry < retries; retry++) {
				port = 45000 + retry;
				try {
					acceptor = server_init(io_service, port);
					break;
				} catch (std::exception &e) {
					if (retry == retries - 1) {
						throw;
					}
				}
			}
			std::ofstream port_file(vm["auto"].as<std::string>().c_str(), std::ios::out);
			port_file.imbue(std::locale("C"));
			port_file << port << "\n";
			port_file.close();
		}

		if (!quiet)
			std::cout << "Listening on port: " << port << "." << std::endl;

		// Load dictionaries
		boost::unordered_map<std::wstring, double> map;
		std::vector<std::string> dict =
				vm["load"].as<std::vector<std::string> >();
		int max_gram = 0;
		int grams_before = 0;
		for (std::vector<std::string>::iterator it = dict.begin();
				it != dict.end(); ++it) {
			std::string filename = *it;
			if (!quiet)
				std::cout << "Loading dictionary: " << filename << "..."
						<< std::endl;
			grams_before = map.size();
			int n = data::loadNGrams(filename, map, quiet);
			if (!quiet)
				std::cout << "Loaded, " << (map.size() - grams_before) << " * "
						<< n << "-grams." << std::endl;
			if (n < 0) {
				// Unabled to load dictionary
				std::cerr << "Unable to load dictionary: " << filename
						<< std::endl;
				return -1;
			}
			if (n > max_gram)
				max_gram = n;
		}
		if (!quiet)
			std::cout << "Loaded dictionaries with " << map.size()
					<< " grams in total." << std::endl;

		// Convert occurences to probabilities
		double ngrams_sum = 0;
		for (boost::unordered_map<std::wstring, double>::iterator it =
				map.begin(); it != map.end(); it++) {
			ngrams_sum += it->second;
		}
		for (boost::unordered_map<std::wstring, double>::iterator it =
				map.begin(); it != map.end(); it++) {
			map[it->first] = it->second / ngrams_sum;
		}

		/*
		 // Save cache
		 {
		 boost::progress_timer timer;
		 saveCache("cache.dat", map);
		 }

		 // Load cache
		 {
		 boost::progress_timer timer;
		 loadCache("cache.dat", map);
		 }
		 */

		// Create emission provider
		libgram::SimpleProvider<wchar_t,
				boost::unordered_map<std::wstring, double> > provider;
		provider.setContainer(&map);
		provider.setMaximumGram(max_gram);

		// Configure epsilon if specified
		if (vm.count("epsilon")) {
			double epsilon_value = vm["epsilon"].as<double>();
			provider.setEpsilon(epsilon_value);
		} else {
			provider.setAutoEpsilon();
		}
		if (!quiet)
			std::cout << "The longest gram is: " << provider.maximumGram()
					<< ", epsilon is: " << provider.epsilon() << "."
					<< std::endl;

		// Configure solver
		solver.setEmissionProvider(&provider);

		// Detach socket (or not)
		if (!vm.count("interactive")) {
			if (!quiet)
				std::cout << "Detaching..." << std::endl;
			if (daemon(0, 0) < 0) {
				std::cerr << "Unable to detach" << std::endl;
				return -1;
			}
		}

		// Start server loop
		if (!quiet)
			std::cout << "Started." << std::endl;
		server_loop(solver, io_service, acceptor);
		if (!quiet)
			std::cout << "Closed." << std::endl;

		return 0;
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
}

