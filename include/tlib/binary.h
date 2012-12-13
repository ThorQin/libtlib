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

#ifndef _TLIBDATA_H_
#define _TLIBDATA_H_

#include "strfunc.h"
#include "charset.h"
#include <iostream>
#include <vector>
#include <stdexcept>

namespace tlib {

template <class T>
inline T* begin_ptr(std::vector<T>& v)
{return  v.empty() ? 0 : &v[0];}

template <class T>
inline const T* begin_ptr(const std::vector<T>& v)
{return  v.empty() ? 0 : &v[0];}

template <class T>
inline T* end_ptr(std::vector<T>& v)
{return v.empty() ? 0 : (begin_ptr(v) + v.size());}

template <class T>
inline const T* end_ptr(const std::vector<T>& v)
{return v.empty() ? 0 : (begin_ptr(v) + v.size());}

// binary class store raw binary data
typedef std::vector<unsigned char> binary;

enum class LetterCase : int {
	LOWER_CASE = 0, UPPER_CASE = 1
};

std::string hex_str(const binary& data,
		LetterCase letter_case = LetterCase::LOWER_CASE);


template<typename T>
inline binary& operator <<(binary& v1, const T& v2) {
	v1.insert(v1.end(), (unsigned char*)&v2, (unsigned char*)&v2 + sizeof(T));
	return v1;
}

template<>
inline binary& operator << <binary>(binary& v1, const binary& v2) {
	v1.insert(v1.end(), v2.begin(), v2.end());
	return v1;
}

template<>
inline binary& operator << <std::string>(binary& v1, const std::string& v2) {
	v1.insert(v1.end(), v2.begin(), v2.end());
	return v1;
}

inline binary& operator <<(binary& v1, const char* v2) {
	v1.insert(v1.end(), (const unsigned char*)v2,
			(const unsigned char*)v2 + strlen(v2));
	return v1;
}

template<>
inline binary& operator << <std::wstring>(binary& v1, const std::wstring& v2) {
	v1.insert(v1.end(), (const unsigned char*)v2.c_str(),
			(const unsigned char*)v2.c_str() + v2.size() * sizeof(wchar_t));
	return v1;
}

inline binary& operator <<(binary& v1, const wchar_t* v2) {
	v1.insert(v1.end(), (const unsigned char*)v2,
			(const unsigned char*)v2 + wcslen(v2) * sizeof(wchar_t));
	return v1;
}

template<typename T>
inline binary to_binary(const T& value) {
	binary data;
	data << value;
	return data;
}

// Convert any type to string.
template<>
inline std::string strfrom<binary>(const binary& src) {
	return std::string(src.begin(), src.end());
}
// Convert string to any type.
template<>
inline binary strto<binary>(const std::string& src) {
	return to_binary(src);
}
// Convert any type to wstring.
template<>
inline std::wstring wstrfrom<binary>(const binary& src) {
	const wchar_t* begin = (const wchar_t*)begin_ptr(src);
	const wchar_t* end = begin + src.size() / sizeof(wchar_t);
	return std::wstring(begin, end);
}
// Convert wstring to any type.
template<>
inline binary wstrto<binary>(const std::wstring& src) {
	return to_binary(src);
}



inline void pop_back(binary& data, size_t size) {
	binary::iterator pop_begin, pop_end;
	if (size > data.size()) {
		pop_begin = data.begin();
		pop_end = data.end();
	} else {
		pop_begin = data.begin() + data.size() - size;
		pop_end = pop_begin + size;
	}
	data.erase(pop_begin, pop_end);
}

inline void pop_front(binary& data, size_t size) {
	binary::iterator pop_begin, pop_end;
	if (size > data.size()) {
		pop_begin = data.begin();
		pop_end = data.end();
	} else {
		pop_begin = data.begin();
		pop_end = pop_begin + size;
	}
	data.erase(pop_begin, pop_end);
}

template<typename T>
inline void extract_back(binary& data, T& value) {
	const unsigned char* begin;
	size_t size = sizeof(value);
	if (size > data.size()) {
		size = data.size();
		begin = begin_ptr(data);
	} else {
		begin = begin_ptr(data) + data.size() - size;
	}
	memcpy(&value, begin, size);
	pop_back(data, size);
}

template<>
inline void extract_back<binary>(binary& data, binary& value) {
	binary::iterator pop_begin, pop_end;
	size_t size = value.size();
	if (size > data.size()) {
		pop_begin = data.begin();
		pop_end = data.end();
	} else {
		pop_begin = data.begin() + data.size() - size;
		pop_end = pop_begin + size;
	}
	value.assign(pop_begin, pop_end);
	data.erase(pop_begin, pop_end);
}

template<>
inline void extract_back<std::string>(binary& data, std::string& value) {
	const unsigned char* pop_begin;
	const unsigned char* pop_end;
	size_t size = value.size();
	if (size > data.size()) {
		size = data.size();
		pop_begin = begin_ptr(data);
		pop_end = end_ptr(data);
	} else {
		pop_begin = begin_ptr(data) + data.size() - size;
		pop_end = pop_begin + size;
	}
	value.assign((const char*)pop_begin, (const char*)pop_end);
	pop_back(data, size);
}

template<>
inline void extract_back<std::wstring>(binary& data, std::wstring& value) {
	const unsigned char* pop_begin;
	size_t size = value.size() * sizeof(wchar_t);
	if (size > data.size()) {
		size = data.size();
		pop_begin = begin_ptr(data);
	} else {
		pop_begin = begin_ptr(data) + data.size() - size;
	}
	value.assign((const wchar_t*)pop_begin,
			((const wchar_t*)pop_begin) + size / sizeof(wchar_t));
	pop_back(data, size);
}

template<typename T>
inline void extract_front(binary& data, T& value) {
	const unsigned char* begin;
	size_t size = sizeof(value);
	if (size > data.size()) {
		begin = begin_ptr(data);
		size = data.size();
	} else {
		begin = begin_ptr(data);
	}
	memcpy(&value, begin, size);
	pop_front(data, size);
}

template<>
inline void extract_front<std::string>(binary& data, std::string& value) {
	const unsigned char* pop_begin;
	const unsigned char* pop_end;
	size_t size = value.size();
	if (size > data.size()) {
		size = data.size();
		pop_begin = begin_ptr(data);
		pop_end = end_ptr(data);
	} else {
		pop_begin = begin_ptr(data);
		pop_end = pop_begin + size;
	}
	value.assign((const char*)pop_begin, (const char*)pop_end);
	pop_back(data, size);
}

template<>
inline void extract_front<std::wstring>(binary& data, std::wstring& value) {
	const unsigned char* pop_begin;
	size_t size = value.size() * sizeof(wchar_t);
	if (size > data.size()) {
		size = data.size();
		pop_begin = begin_ptr(data);
	} else {
		pop_begin = begin_ptr(data);
	}
	value.assign((const wchar_t*)pop_begin,
			((const wchar_t*)pop_begin) + size / sizeof(wchar_t));
	pop_front(data, size);
}

// Get the data's character set and whether have a BOM flag.
const char* test_charset(const binary& data, bool& bom);

template<typename TChar>
binary load_binary(const std::basic_string<TChar>& filename)
throw(std::runtime_error);

template<>
binary load_binary<char>(const std::string& filename)
throw (std::runtime_error);

template<>
inline binary load_binary<wchar_t>(const std::wstring& filename)
throw(std::runtime_error) {
	return load_binary<char>(wstring_to_locale(filename));
}

template<typename TChar>
std::basic_string<TChar> load_string(const binary& data);

template<typename TChar, typename TFile>
inline std::basic_string<TChar> load_string(const std::basic_string<TFile>& filename) {
	return load_string<TChar>(load_binary(filename));
}

} // end namespace tlib


inline std::ostream& operator <<(std::ostream& out, const tlib::binary& data) {
	if (out.good())
		out.write((const char*) tlib::begin_ptr(data),
				(std::streamsize) data.size());
	return out;
}

inline std::istream & operator >>(std::istream& in, tlib::binary& data) {
	data.clear();
	while (in.good()) {
		char buf[512];
		in.read(buf, 512);
		data.insert(data.end(), buf, buf + in.gcount());
	}
	return in;
}




#endif /* _TLIBDATA_H_ */
