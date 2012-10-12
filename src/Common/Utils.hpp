/*!
 * \file Utils.hpp
 * \brief Utility functions - declaration
 * \author mstefanc
 * \date 2010-06-09
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <vector>

namespace Utils {

#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define MARK_DEPRECATED(MSG) __attribute__((__deprecated__))
#elif defined(_MSC_VER)
#define MARK_DEPRECATED(MSG) __declspec(deprecated)
#else
#define MARK_DEPRECATED(MSG)
#endif /* __GNUC__ */






#if defined (_WIN32)
  #if defined(COMPILING_DLL)
    #define  MYLIB_EXPORT __declspec(dllexport)
  #else
    #define  MYLIB_EXPORT __declspec(dllimport)
  #endif /* MyLibrary_EXPORTS */
#else /* defined (_WIN32) */
 #define MYLIB_EXPORT
#endif

/*!
 * \brief Find all files matching given regular expression in specified location
 *
 * \param root directoory to search files
 * \param regexp regular expression to test file names
 * \param with_path if set to true, then output will be path+filename, otherwise only filename will be returned
 *
 * \see \ref regex_basics
 */
MYLIB_EXPORT std::vector<std::string> searchFiles(const std::string & root, const std::string & regexp, bool with_path = true);

/*!
 * List subdirectories of given folder
 * @param root
 * @param with_path
 * @return
 */
MYLIB_EXPORT std::vector<std::string> getSubdirs(const std::string & root, bool with_path);

MYLIB_EXPORT std::string findSubdir(const std::string & name, const std::vector<std::string> & roots, bool with_path);




template <class T>
unsigned int distLevenshtein(const T& s1, const T& s2)
{
	const size_t len1 = s1.size(), len2 = s2.size();
	std::vector<std::vector<unsigned int> > d(len1 + 1, std::vector<unsigned int>(len2 + 1));

	d[0][0] = 0;
	for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
	for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

	for(unsigned int i = 1; i <= len1; ++i)
		for(unsigned int j = 1; j <= len2; ++j)

                      d[i][j] = std::min( std::min(d[i - 1][j] + 1,d[i][j - 1] + 1),
                                          d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) );
	return d[len1][len2];
}



} //: namespace Utils

#endif /* UTILS_HPP_ */
