/*!
 * \file Utils.cpp
 * \brief Utility functions - implementation
 * \author mstefanc
 * \date Jun 9, 2010
 */

#include "Utils.hpp"

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <boost/regex.hpp>
#include <boost/foreach.hpp>

using namespace std;

namespace Utils {

std::vector<std::string> searchFiles(const std::string & root, const std::string & regexp, bool with_path) {
	boost::regex e(regexp);
	std::vector<std::string> ret;

	path dir_path(root);
	if ( !exists( dir_path ) )
		return ret;


	directory_iterator end_itr; // default construction yields past-the-end
	for ( directory_iterator itr( dir_path ); itr != end_itr; ++itr )
	{
		std::string fname = itr->path().filename();
		if (regex_match(fname, e)) {
			if (with_path)
				ret.push_back(itr->path().file_string());
			else
				ret.push_back(fname);
		}
	}

	return ret;
}

std::vector<std::string> getSubdirs(const std::string & root, bool with_path) {
	std::vector<std::string> ret;

	path dir_path(root);
	if ( !exists( dir_path ) )
		return ret;

	directory_iterator end_itr; // default construction yields past-the-end
	for ( directory_iterator itr( dir_path ); itr != end_itr; ++itr )
	{
		std::string fname = itr->path().filename();
		if (is_directory(itr->status())) {
			if (with_path)
				ret.push_back(itr->path().file_string());
			else
				ret.push_back(fname);
		}
	}

	return ret;
}

std::string findSubdir(const std::string & name, const std::vector<std::string> & roots, bool with_path) {
	std::string root;
	BOOST_FOREACH(root, roots) {
		path dir_path(root);
		if ( !exists( dir_path ) )
			continue;

		directory_iterator end_itr; // default construction yields past-the-end
		for ( directory_iterator itr( dir_path ); itr != end_itr; ++itr )
		{
			std::string fname = itr->path().filename();
			if (is_directory(itr->status()) && fname == name) {
				if (with_path)
					return itr->path().file_string();
				else
					return fname;
			}
		}
	}

	return "";
}

} //: namespace Utils
