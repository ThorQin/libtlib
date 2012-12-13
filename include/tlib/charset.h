/************************************************************************
 *
 *  LibTLib
 *  Copyright (C) 2010  Thor Qin
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * Author: Thor Qin
 * Bug Report: thor.qin@gmail.com
 *
 **************************************************************************/

#ifndef CHARSET_H_
#define CHARSET_H_

#include <locale>

namespace tlib {

enum class CodePage
	: int {
		UTF8 = 65001, UTF16LE = 1200, UTF16BE = 1201
};

extern std::locale __tlib_c_locale;

// Initialize local information
void init_locale();
void init_locale(const std::string& locName);

// Convert between charset and windows codepage.
const char* codepage_to_charset(unsigned int codepage);
unsigned int charset_to_codepage(const char* charset);

// Get the current OS used charset.
const char* get_locale_charset();

// String charset convert.
std::string wstring_to_locale(const std::wstring &src);
std::wstring locale_to_wstring(const std::string& src);

std::string wstring_to_utf8(const std::wstring &src);
std::wstring utf8_to_wstring(const std::string& src);

std::string utf8_to_locale(const std::string& src);
std::string locale_to_utf8(const std::string& src);

std::wstring charset_to_wstring(const std::string& src,
		const std::string& charset);
std::string wstring_to_charset(const std::wstring& src,
		const std::string& charset);

inline std::string convert_charset(const std::string& src,
		const std::string& fromCharset, const std::string& toCharset) {
	return wstring_to_charset(charset_to_wstring(src, fromCharset), toCharset);
}

template<typename T>
inline const std::basic_string<T> str(const std::string& s);
template<>
inline const std::basic_string<char> str<char>(const std::string& s) {
	return s;
}
template<>
inline const std::basic_string<wchar_t> str<wchar_t>(const std::string& s) {
	return locale_to_wstring(s);
}

template<typename T>
inline const std::basic_string<T> str(const std::wstring& s);
template<>
inline const std::basic_string<char> str<char>(const std::wstring& s) {
	return wstring_to_locale(s);
}
template<>
inline const std::basic_string<wchar_t> str<wchar_t>(const std::wstring& s) {
	return s;
}

}

#endif /* CHARSET_H_ */
