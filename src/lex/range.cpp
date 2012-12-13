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

#include "range.h"
#include <limits.h>

namespace tlib
{
namespace lex
{

template <>
const char Range<char>::full_min = SCHAR_MIN;
template <>
const char Range<char>::full_max = SCHAR_MAX;
template <>
const Range<char> Range<char>::empty_range = Range<char>();
template <>
const Range<char> Range<char>::full_range = 
	Range<char>(Range<char>::full_min, Range<char>::full_max);


template <>
const wchar_t Range<wchar_t>::full_min = 0;
template <>
const wchar_t Range<wchar_t>::full_max = USHRT_MAX;
template <>
const Range<wchar_t> Range<wchar_t>::empty_range = Range<wchar_t>();
template <>
const Range<wchar_t> Range<wchar_t>::full_range = 
	Range<wchar_t>(Range<wchar_t>::full_min, Range<wchar_t>::full_max);


template <>
const unsigned char Range<unsigned char>::full_min = 0;
template <>
const unsigned char Range<unsigned char>::full_max = UCHAR_MAX;
template <>
const Range<unsigned char> Range<unsigned char>::empty_range = Range<unsigned char>();
template <>
const Range<unsigned char> Range<unsigned char>::full_range = 
	Range<unsigned char>(Range<unsigned char>::full_min, Range<unsigned char>::full_max);


template <>
const short Range<short>::full_min = SHRT_MIN;
template <>
const short Range<short>::full_max = SHRT_MAX;
template <>
const Range<short> Range<short>::empty_range = Range<short>();
template <>
const Range<short> Range<short>::full_range = 
	Range<short>(Range<short>::full_min, Range<short>::full_max);


template <>
const unsigned short Range<unsigned short>::full_min = 0;
template <>
const unsigned short Range<unsigned short>::full_max = USHRT_MAX;
template <>
const Range<unsigned short> Range<unsigned short>::empty_range = Range<unsigned short>();
template <>
const Range<unsigned short> Range<unsigned short>::full_range = 
	Range<unsigned short>(Range<unsigned short>::full_min, Range<unsigned short>::full_max);


template <>
const int Range<int>::full_min = INT_MIN;
template <>
const int Range<int>::full_max = INT_MAX;
template <>
const Range<int> Range<int>::empty_range = Range<int>();
template <>
const Range<int> Range<int>::full_range = 
	Range<int>(Range<int>::full_min, Range<int>::full_max);


template <>
const unsigned int Range<unsigned int>::full_min = 0;
template <>
const unsigned int Range<unsigned int>::full_max = UINT_MAX;
template <>
const Range<unsigned int> Range<unsigned int>::empty_range = Range<unsigned int>();
template <>
const Range<unsigned int> Range<unsigned int>::full_range = 
	Range<unsigned int>(Range<unsigned int>::full_min, Range<unsigned int>::full_max);


template <>
const long Range<long>::full_min = LONG_MIN;
template <>
const long Range<long>::full_max = LONG_MAX;
template <>
const Range<long> Range<long>::empty_range = Range<long>();
template <>
const Range<long> Range<long>::full_range = 
	Range<long>(Range<long>::full_min, Range<long>::full_max);


template <>
const unsigned long Range<unsigned long>::full_min = 0;
template <>
const unsigned long Range<unsigned long>::full_max = ULONG_MAX;
template <>
const Range<unsigned long> Range<unsigned long>::empty_range = Range<unsigned long>();
template <>
const Range<unsigned long> Range<unsigned long>::full_range = 
	Range<unsigned long>(Range<unsigned long>::full_min, Range<unsigned long>::full_max);


template <>
const long long Range<long long>::full_min = LLONG_MIN;
template <>
const long long Range<long long>::full_max = LLONG_MAX;
template <>
const Range<long long> Range<long long>::empty_range = Range<long long>();
template <>
const Range<long long> Range<long long>::full_range = 
	Range<long long>(Range<long long>::full_min, Range<long long>::full_max);


template <>
const unsigned long long Range<unsigned long long>::full_min = 0;
template <>
const unsigned long long Range<unsigned long long>::full_max = ULLONG_MAX;
template <>
const Range<unsigned long long> Range<unsigned long long>::empty_range = Range<unsigned long long>();
template <>
const Range<unsigned long long> Range<unsigned long long>::full_range = 
	Range<unsigned long long>(Range<unsigned long long>::full_min, Range<unsigned long long>::full_max);


} // End of namespace lex
} // End of namespace tlib