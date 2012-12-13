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

#include "../include/tlib/binary.h"
#include "../include/tlib/strfunc.h"
#include "../include/tlib/charset.h"

#include <stdexcept>
#include <fstream>

namespace tlib {

std::string hex_str(const binary& data, LetterCase letter_case) {
	std::string result;
	result.reserve(data.size() * 2);
	static const char* lower_hex_table = "0123456789abcdef";
	static const char* upper_hex_table = "0123456789ABCDEF";
	const char* hex_table;
	if (letter_case == LetterCase::LOWER_CASE)
		hex_table = lower_hex_table;
	else
		hex_table = upper_hex_table;
	const unsigned char* scan = begin_ptr(data);
	const unsigned char* scan_end = end_ptr(data);
	while (scan < scan_end) {
		result.push_back(hex_table[*scan >> 4]);
		result.push_back(hex_table[*scan & 0x0F]);
		scan++;
	}
	return result;
}

template<>
binary load_binary<char>(const std::string& filename) throw (std::runtime_error) {
	binary result;
	std::ifstream infile(filename.c_str(),
			std::ios_base::in | std::ios_base::binary);
	if (infile.fail())
		throw std::runtime_error("Open file error: " + filename);
	infile >> result;
	infile.close();
	return result;
}
template
binary load_binary<char>(const std::string& filename)
		throw (std::runtime_error);

template<>
std::wstring load_string<wchar_t>(const binary& data) {
	//std::wstring result;
	bool bom;
	const char* charset = test_charset(data, bom);
	unsigned int cp = charset_to_codepage(charset);
	if (cp == 65001 && bom) { // utf-8
		return utf8_to_wstring(std::string(data.begin() + 3, data.end()));
	} else if ((cp == 1200 || cp == 1201) && bom) { // utf-16
		return charset_to_wstring(std::string(data.begin() + 2, data.end()),
				charset);
	} else
		return charset_to_wstring(std::string(data.begin(), data.end()),
				charset);

}
template
std::wstring load_string<wchar_t>(const binary& data);

template<>
std::string load_string<char>(const binary& data) {
	//std::string result;
	bool bom;
	const char* charset = test_charset(data, bom);
	unsigned int cp = charset_to_codepage(charset);
	if (cp == 65001 && bom) { // utf-8
		return utf8_to_locale(std::string(data.begin() + 3, data.end()));
	} else if ((cp == 1200 || cp == 1201) && bom) { // utf-16
		return wstring_to_locale(
				charset_to_wstring(std::string(data.begin() + 2, data.end()),
						charset));
	} else
		return wstring_to_locale(
				charset_to_wstring(std::string(data.begin(), data.end()),
						charset));
}
template
std::string load_string<char>(const binary& data);

} // end namespace tlib

