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

#ifndef _TLIBSTR_H_
#define _TLIBSTR_H_

#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>
#include <fstream>
#include <wchar.h>


#ifdef __MSVC__
#  define USE_CONVERT_API
#  define strcasecmp _stricmp
#  define strncasecmp _strnicmp
#endif

namespace tlib {

bool _isspace(int ch);
bool _iscntrl(int ch);
bool _isalpha(int ch);
bool _isalnum(int ch);
bool _isdigit(int ch);
bool _isxdigit(int ch);
bool _islower(int ch);
bool _isupper(int ch);
bool _ispunct(int ch);
inline bool _isgraph(int ch) {
	return (ch >= 0x21 && ch <= 0x7e);
}

inline bool _isprint(int ch) {
	return (ch >= 0x20 && ch <= 0x7e);
}
inline bool _isascii(int ch) {
	return (ch >= 0x0 && ch <= 0x7f);
}


// String split and join
template <typename T_Char>
std::vector<std::basic_string<T_Char>> split(
		const std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& delimit);

template <typename T_Char>
inline std::vector<std::basic_string<T_Char>> split(
		const std::basic_string<T_Char>& src,
		const T_Char* delimit) {
	return split<T_Char>(src, std::basic_string<T_Char>(delimit));
}

template<typename T_Char>
std::basic_string<T_Char> join(
		const std::vector<std::basic_string<T_Char>>& array,
		const std::basic_string<T_Char>& delimit);

template<typename T_Char>
inline std::basic_string<T_Char> join(
		const std::vector<std::basic_string<T_Char>>& array,
		const T_Char* delimit) {
	return join<T_Char>(array, std::basic_string<T_Char>(delimit));
}

template<typename T_Char, typename Iterator>
inline std::basic_string<T_Char> join(
		Iterator begin, Iterator end,
		const std::basic_string<T_Char>& delimit) {
	std::basic_string<T_Char> result;
	Iterator it;
	for (it = begin; it != end; ++it) {
		if (!result.empty())
			result += delimit;
		result += *it;
	}
	return result;
}

template<typename T_Char, typename Iterator>
inline std::basic_string<T_Char> join(
		Iterator begin, Iterator end,
		const T_Char* delimit) {
	return join<T_Char>(begin, end, std::basic_string<T_Char>(delimit));
}


extern std::locale __tlib_c_locale;

template<typename T_Char, typename T>
struct __Cvt {
	// Convert any type to string.
	static std::basic_string<T_Char> to_string(const T& src) {
		std::basic_stringstream<T_Char> sbuf;
		// Bug fixed by Dennis Felippa.

		sbuf.imbue(__tlib_c_locale);
		sbuf << src;
		return sbuf.str();
	}
	// Convert string to any type.
	static T from_string(const std::basic_string<T_Char>& src) {
		std::basic_stringstream<T_Char> sbuf(src);
		// Bug fixed by Dennis Felippa.
		sbuf.imbue(__tlib_c_locale);
		T obj;
		sbuf >> obj;
		return obj;
	}
};

template<typename T_Char>
struct __Cvt<T_Char, unsigned char> {
	static std::basic_string<T_Char> to_string(const unsigned char& src) {
		return __Cvt<T_Char, unsigned long>::to_string(
				static_cast<const unsigned long>(src));
	}
	static unsigned char from_string(const std::basic_string<T_Char>& src) {
		return (const unsigned char) __Cvt<T_Char, unsigned long>::from_string(
				src);
	}
};

template<typename T_Char>
struct __Cvt<T_Char, char> {
	static inline std::basic_string<T_Char> to_string(const char& src) {
		return __Cvt<T_Char, long>::to_string(static_cast<const long>(src));
	}
	static inline char from_string(const std::basic_string<T_Char>& src) {
		return (const char) __Cvt<T_Char, long>::from_string(src);
	}
};


template<typename T_Char>
struct __Cvt<T_Char, unsigned short> {
	static std::basic_string<T_Char> to_string(const unsigned short& src) {
		return __Cvt<T_Char, unsigned long>::to_string(
				static_cast<const unsigned long>(src));
	}
	static unsigned short from_string(const std::basic_string<T_Char>& src) {
		return (const unsigned short) __Cvt<T_Char, unsigned long>::from_string(
				src);
	}
};

template<typename T_Char>
struct __Cvt<T_Char, short> {
	static inline std::basic_string<T_Char> to_string(const short& src) {
		return __Cvt<T_Char, long>::to_string(static_cast<const long>(src));
	}
	static inline short from_string(const std::basic_string<T_Char>& src) {
		return (const short) __Cvt<T_Char, long>::from_string(src);
	}
};

template<typename T_Char>
struct __Cvt<T_Char, unsigned int> {
	static inline std::basic_string<T_Char> to_string(const unsigned int& src) {
		return __Cvt<T_Char, unsigned long>::to_string(
				static_cast<const unsigned long>(src));
	}
	static inline unsigned int from_string(
			const std::basic_string<T_Char>& src) {
		return (const unsigned int) __Cvt<T_Char, unsigned long>::from_string(
				src);
	}
};

template<typename T_Char>
struct __Cvt<T_Char, int> {
	static inline std::basic_string<T_Char> to_string(const int& src) {
		return __Cvt<T_Char, long>::to_string(static_cast<const long>(src));
	}
	static inline int from_string(const std::basic_string<T_Char>& src) {
		return (const int) __Cvt<T_Char, long>::from_string(src);
	}
};


template<typename T_Char>
struct __Cvt<T_Char, unsigned long> {
	static std::basic_string<T_Char> to_string(const unsigned long& src) {
		static T_Char numbers[10] = { '0', '1', '2', '3', '4', '5', '6', '7',
				'8', '9' };
		std::basic_string<T_Char> result;
		result.reserve(32);
		unsigned long val = src;
		while (val > 0) {
			result.insert(result.begin(), numbers[(int) (val % 10)]);
			val /= 10;
		}
		return result;
	}
	static unsigned long from_string(
			const std::basic_string<T_Char>& src) {
		return (const unsigned long) __Cvt<T_Char, long>::from_string(src);
	}
};

template<typename T_Char>
struct __Cvt<T_Char, long> {
	static std::basic_string<T_Char> to_string(const long& src) {
		static T_Char numbers[10] = { '0', '1', '2', '3', '4', '5', '6', '7',
				'8', '9' };
		std::basic_string<T_Char> result;
		result.reserve(32);
		long val = src;
		int pos = 0;
		if (val < 0) {
			result.push_back((T_Char) '-');
			pos = 1;
			val = abs(val);
		}
		while (val > 0) {
			result.insert(result.begin() + pos, numbers[(int) (val % 10)]);
			val /= 10;
		}
		return result;
	}
	static long from_string(const std::basic_string<T_Char>& src) {
		long result = 0;
		bool neg = false;
		int state = 0;
		for (size_t i = 0; i < src.size(); i++) {
			int v = src[i];
			if (state == 0) {
				if (_isspace(v)) {
				} else if (v == '-') {
					neg = true;
					state = 1;
				} else if (v < '0' || v > '9') {
					return 0;
				} else {
					result *= 10;
					result += (v - '0');
					state = 1;
				}
			} else if (state == 1) {
				if (v < '0' || v > '9') {
					return (neg ? -result : result);
				} else {
					result *= 10;
					result += (v - '0');
				}
			}
		}
		return (neg ? -result : result);
	}
};





// Convert any type to string.
template<typename T>
inline std::string strfrom(const T& src) {
	return __Cvt<char, T>::to_string(src);
}
// Convert string to any type.
template<typename T>
inline T strto(const std::string& src) {
	return __Cvt<char, T>::from_string(src);
}
// Convert any type to wstring.
template<typename T>
inline std::wstring wstrfrom(const T& src) {
	return __Cvt<wchar_t, T>::to_string(src);
}
// Convert wstring to any type.
template<typename T>
inline T wstrto(const std::wstring& src) {
	return __Cvt<wchar_t, T>::from_string(src);
}




// Case converting.
template<class T_Char>
inline std::basic_string<T_Char> & to_lower(std::basic_string<T_Char>& src) {
	std::transform(src.begin(), src.end(), src.begin(), tolower);
	return src;
}
template<class T_Char>
inline std::basic_string<T_Char> to_lower_copy(
		const std::basic_string<T_Char>& src) {
	std::basic_string<T_Char> result(src);
	return to_lower(result);
}
template<class T_Char>
inline std::basic_string<T_Char> & to_upper(std::basic_string<T_Char>& src) {
	std::transform(src.begin(), src.end(), src.begin(), toupper);
	return src;
}
template<class T_Char>
inline std::basic_string<T_Char> to_upper_copy(
		const std::basic_string<T_Char>& src) {
	std::basic_string<T_Char> result(src);
	return to_upper(result);
}




// Replace sub string to a new string.
template<typename T_Char>
inline std::basic_string<T_Char>& replace(std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& fnd,
		const std::basic_string<T_Char>& rep, int rep_times) {
	typedef typename std::basic_string<T_Char>::size_type size_type;
	size_type pos = 0;
	size_type fnd_len = fnd.length();
	size_type rep_len = rep.length();
	int times = 0;
	for (;;) {
		if (rep_times >= 0 && times >= rep_times)
			break;
		pos = src.find(fnd, pos);
		if (pos == std::string::npos)
			break;
		src.replace(pos, fnd_len, rep);
		times++;
		pos += rep_len;
		if (fnd_len == 0)
			pos++;
	}
	return src;
}

template<typename T_Char>
inline std::basic_string<T_Char>& replace(std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& fnd,
		const T_Char* rep, int rep_times) {
	return replace(src, fnd, std::basic_string<T_Char>(rep), rep_times);
}

template<typename T_Char>
inline std::basic_string<T_Char>& replace(std::basic_string<T_Char>& src,
		const T_Char* fnd, const T_Char* rep, int rep_times) {
	return replace(src, std::basic_string<T_Char>(fnd),
			std::basic_string<T_Char>(rep), rep_times);
}

template<typename T_Char>
inline std::basic_string<T_Char> replace_copy(
		const std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& fnd,
		const std::basic_string<T_Char>& rep, int rep_times) {
	std::basic_string<T_Char> result(src);
	return replace(result, fnd, rep, rep_times);
}

template<typename T_Char>
inline std::basic_string<T_Char> replace_copy(
		const std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& fnd,
		const T_Char* rep, int rep_times) {
	return replace_copy(src, fnd, std::basic_string<T_Char>(rep), rep_times);
}

template<typename T_Char>
inline std::basic_string<T_Char> replace_copy(
		const std::basic_string<T_Char>& src,
		const T_Char* fnd,
		const T_Char* rep, int rep_times) {
	return replace_copy(src, std::basic_string<T_Char>(fnd),
			std::basic_string<T_Char>(rep), rep_times);
}

template<typename T_Char>
inline std::basic_string<T_Char>& replace(std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& fnd,
		const std::basic_string<T_Char>& rep) {
	typedef typename std::basic_string<T_Char>::size_type size_type;
	size_type pos = 0;
	size_type fnd_len = fnd.length();
	size_type rep_len = rep.length();
	for (;;) {
		pos = src.find(fnd, pos);
		if (pos == std::string::npos)
			break;
		src.replace(pos, fnd_len, rep);
		pos += rep_len;
		if (fnd_len == 0)
			pos++;
	}
	return src;
}

template<typename T_Char>
inline std::basic_string<T_Char>& replace(std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& fnd,
		const T_Char* rep) {
	return replace(src, fnd, std::basic_string<T_Char>(rep));
}

template<typename T_Char>
inline std::basic_string<T_Char>& replace(std::basic_string<T_Char>& src,
		const T_Char* fnd,
		const T_Char* rep) {
	return replace(src, std::basic_string<T_Char>(fnd),
			std::basic_string<T_Char>(rep));
}

template<typename T_Char>
inline std::basic_string<T_Char> replace_copy(
		const std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& fnd,
		const std::basic_string<T_Char>& rep) {
	std::basic_string<T_Char> result(src);
	return replace(result, fnd, rep);
}

template<typename T_Char>
inline std::basic_string<T_Char> replace_copy(
		const std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& fnd,
		const T_Char* rep) {
	return replace_copy(src, fnd, std::basic_string<T_Char>(rep));
}

template<typename T_Char>
inline std::basic_string<T_Char> replace_copy(
		const std::basic_string<T_Char>& src,
		const T_Char* fnd,
		const T_Char* rep) {
	return replace_copy(src, std::basic_string<T_Char>(fnd),
			std::basic_string<T_Char>(rep));
}


template<typename T_Char>
inline bool char_exist(const std::basic_string<T_Char>& src,
		const std::basic_string<T_Char>& char_seq) {
	for (typename std::basic_string<T_Char>::const_iterator it =
			char_seq.begin(); it != char_seq.end(); it++) {
		if (src.find(*it, 0) != src.npos)
			return true;
	}
	return false;
}






// String trim.
template<typename T_Char>
inline std::basic_string<T_Char>& trim_left(std::basic_string<T_Char>& src) {
	typename std::basic_string<T_Char>::iterator it = std::find_if(src.begin(),
			src.end(), std::not1(std::ptr_fun(tlib::_isspace)));
	src.erase(src.begin(), it);
	return src;
}
template<typename T_Char>
inline std::basic_string<T_Char> trim_left_copy(
		const std::basic_string<T_Char>& src) {
	std::basic_string<T_Char> result(src);
	return trim_left(result);
}
template<typename T_Char>
inline std::basic_string<T_Char>& trim_right(std::basic_string<T_Char>& src) {
	typename std::basic_string<T_Char>::reverse_iterator it = std::find_if(
			src.rbegin(), src.rend(), std::not1(std::ptr_fun(tlib::_isspace)));
	src.erase(it.base(), src.end());
	return src;
}
template<typename T_Char>
inline std::basic_string<T_Char> trim_right_copy(
		const std::basic_string<T_Char>& src) {
	std::basic_string<T_Char> result(src);
	return trim_right(result);
}
template<typename T_Char>
inline std::basic_string<T_Char>& trim(std::basic_string<T_Char>& src) {
	return trim_left(trim_right(src));
}
template<typename T_Char>
inline std::basic_string<T_Char> trim_copy(
		const std::basic_string<T_Char>& src) {
	std::basic_string<T_Char> result(src);
	trim_left(trim_right(result));
	return result;
}
// If trimmed string is empty then return true.
template<class T_Char>
inline bool is_trim_empty(const std::basic_string<T_Char>& src) {
	typename std::basic_string<T_Char>::const_iterator itr = std::find_if(
			src.begin(), src.end(), std::not1(std::ptr_fun(tlib::_isspace)));
	return itr == src.end();
}



} // end of tlib namespace.


#endif /* _TLIBSTR_H_ */
