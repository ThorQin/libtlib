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

#ifndef REGEX_H_
#define REGEX_H_

#include "../tlibbase.h"
#include "lexical.h"

namespace tlib
{
namespace lex
{


template <typename T>
class Regex
{
public:
	inline Regex(const std::basic_string<T>& exp);
	inline Regex(const T* exp);
	inline Regex(const unsigned char* bc, size_t bc_len, bool is_static = false);
	typename Lexical<T>::LexicalPtr _lexical;
};

template <typename T> inline
Regex<T>::Regex(const unsigned char* bc, size_t bc_len, bool is_static)
{
	if (is_static)
		_lexical = Lexical<T>::create_by_static_bc(bc, bc_len);
	else
		_lexical = Lexical<T>::create_by_bc(bc, bc_len);
	_lexical->best_match = true;
}

template <> inline
Regex<char>::Regex(const std::string& exp)
{
	_lexical = Lexical<char>::create(":" + exp);
	_lexical->best_match = true;
}

template <> inline
Regex<unsigned char>::Regex(const std::basic_string<unsigned char>& exp)
{
	_lexical = Lexical<unsigned char>::create((const unsigned char*)":" + exp);
	_lexical->best_match = true;
}


template <> inline
Regex<wchar_t>::Regex(const std::wstring& exp)
{
	_lexical = Lexical<wchar_t>::create(L":" + exp);
	_lexical->best_match = true;
}

template <> inline
Regex<char>::Regex(const char* exp)
{
	_lexical = Lexical<char>::create(std::string(":") + exp);
	_lexical->best_match = true;
}

template <> inline
Regex<unsigned char>::Regex(const unsigned char* exp)
{
	_lexical = Lexical<unsigned char>::create(
			std::basic_string<unsigned char>((const unsigned char*)":") + exp);
	_lexical->best_match = true;
}

template <> inline
Regex<wchar_t>::Regex(const wchar_t* exp)
{
	_lexical = Lexical<wchar_t>::create(std::wstring(L":") + exp);
	_lexical->best_match = true;
}


// Use regular expression to replace matched characters.
template <typename T>
const std::basic_string<T> regex_replace(const std::basic_string<T>& source,
		const Regex<T>& exp,
		const std::basic_string<T>& rep, int repl_times = -1)
		throw (std::runtime_error)
{
	std::basic_string<T> result;
	if (source.length() == 0)
	{
		return source;
	}
	std::basic_istringstream<T> ss(source);
	exp._lexical->parse(ss);
	int rep_times = 0;
	Token<T> token;
	while (exp._lexical->fetch_next(token))
	{
		if (token.action == 1)
		{
			if (repl_times == -1 || rep_times < repl_times)
			{
				result += rep;
				rep_times++;
			}
			else
			{
				result += source.substr(token.pos);
				return result;
			}
		}
		else
			result += token.str;
	}
	return result;
}


template <typename T, typename _Repl>
const std::basic_string<T> regex_replace(const std::basic_string<T>& source,
		const Regex<T>& exp,
		_Repl rep_op, int repl_times = -1) throw (std::runtime_error)
{
	std::basic_string<T> result;
	if (source.length() == 0)
	{
		return source;
	}
	std::basic_istringstream<T> ss(source);
	exp._lexical->parse(ss);
	int rep_times = 0;
	Token<T> token;
	while (exp._lexical->fetch_next(token))
	{
		if (token.action == 1)
		{
			if (repl_times == -1 || rep_times < repl_times)
			{
				rep_op(token.str);
				result += token.str;
				rep_times++;
			}
			else
			{
				result += source.substr(token.pos);
				return result;
			}
		}
		else
			result += token.str;
	}
	return result;
}

template <typename T>
size_t regex_find(const std::basic_string<T>& source,
		const Regex<T>& exp, std::basic_string<T>& fnd, size_t start = 0)
		throw (std::runtime_error)
{
	if (start >= source.length())
		return source.npos;

	std::basic_istringstream<T> ss(source.c_str() + start);
	exp._lexical->parse(ss);
	Token<T> token;
	while (exp._lexical->fetch_next(token))
	{
		if (token.action == 1)
		{
			fnd = token.str;
			return start + token.pos;
		}
	}
	return source.npos;
}


template <typename T>
size_t regex_find(const std::basic_string<T>& source,
		const Regex<T>& exp, size_t start = 0) throw (std::runtime_error)
{
	if (start >= source.length())
		return source.npos;

	std::basic_istringstream<T> ss(source.c_str() + start);
	exp._lexical->parse(ss);
	Token<T> token;
	while (exp._lexical->fetch_next(token))
	{
		if (token.action == 1)
			return start + token.pos;
	}
	return source.npos;
}


}
}

#endif /* REGEX_H_ */
