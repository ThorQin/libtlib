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

#ifndef RANGES_H_
#define RANGES_H_

#include "../tlibbase.h"
#include <vector>
#include <algorithm>
#include "range.h"

namespace tlib
{

namespace lex
{

template <class T>
class RangeArray
{
public:
	typedef Range<T> value_type;
	typedef size_t size_type;
	inline RangeArray();
	inline RangeArray(const value_type& range);
	inline RangeArray(const RangeArray& ranges);
	inline RangeArray& operator = (const value_type& range);
	inline RangeArray& operator = (const RangeArray& ranges);
	inline RangeArray& operator += (const value_type& range);
	inline RangeArray& operator -= (const value_type& range);
	inline RangeArray& operator += (const RangeArray& ranges);
	inline RangeArray& operator -= (const RangeArray& ranges);
	inline const value_type& operator [] (size_type pos) const;
	inline bool empty() const;
	inline void clear();
	inline size_type size() const;
private:
	std::vector<value_type> _vec;
};

template <class T> inline
RangeArray<T>::RangeArray()
{
}

template <class T> inline
RangeArray<T>::RangeArray(const value_type& range)
{
	if (!range.empty())
		_vec.push_back(range);
}

template <class T> inline 
RangeArray<T>::RangeArray(const RangeArray<T>& ranges)
{
	_vec.assign(ranges._vec.begin(), ranges._vec.end());
}

template <class T> inline 
RangeArray<T>& RangeArray<T>::operator = (const value_type& range)
{
	clear();
	if (!range.empty())
		_vec.push_back(range);
	return *this;
}

template <class T> inline
RangeArray<T>& RangeArray<T>::operator = (const RangeArray<T>& ranges)
{
	_vec.assign(ranges._vec.begin(), ranges._vec.end());
}

template <class T> inline
RangeArray<T>& RangeArray<T>::operator += (const value_type& range)
{
	typename value_type::value_type min, max;
	if (range.empty())
		return *this;
	min = range.min();
	max = range.max();
	/* Get every item in sequence. */
	typename std::vector<value_type>::size_type i = 0;
	while (_vec.begin() + i != _vec.end())
	{
		value_type& src = *(_vec.begin() + i);
		if (relation(src, range) != 0)
		{
			min = min < src.min() ? min : src.min();
			max = max > src.max() ? max : src.max();
			_vec.erase(_vec.begin() + i);
		}
		else
			i++;
	}
	_vec.push_back(value_type(min, max));
	return *this;
}

template <class T> inline 
RangeArray<T>& RangeArray<T>::operator -= (const value_type& range)
{
	if (range.empty())
		return *this;
	/* Get every item in sequence. */
	typename std::vector<value_type>::size_type i = 0;
	while (_vec.begin() + i != _vec.end())
	{
		value_type& src = *(_vec.begin() + i);
		RangeRelation rel = relation(src, range);
		if (rel & R_INSIDE)
		{
			/* The "srcrange" inside of "range", so remove it from sequence. */
			_vec.erase(_vec.begin() + i);
		}
		else if (rel == R_OUTSIDE)
		{
			typename value_type::value_type min = src.min(), max = src.max();
			_vec.erase(_vec.begin() + i);
			if (min < range.min())
			{
				_vec.insert(_vec.begin() + i, value_type(min, range.min() - 1));
				i++;
			}
			if (max > range.max())
			{
				_vec.insert(_vec.begin() + i, value_type(range.max() + 1, max));
				i++;
			}
		}
		else if (rel == R_INTERSECT)
		{
			if (src.min() < range.min())
				src = value_type(src.min(), range.min() - 1);
			else
				src = value_type(range.max() + 1, src.max());
			i++;
		}
		else
			i++;
	}
	return *this;
}

template <class T> inline 
RangeArray<T>& RangeArray<T>::operator += (const RangeArray<T>& ranges)
{
	typename std::vector<value_type>::const_iterator iter = ranges._vec.begin();
	while (iter != ranges._vec.end())
	{
		*this += *iter;
		iter++;
	}
	return *this;
}

template <class T> inline 
RangeArray<T>& RangeArray<T>::operator -= (const RangeArray<T>& ranges)
{
	typename std::vector<value_type>::const_iterator iter = ranges._vec.begin();
	while (iter != ranges._vec.end())
	{
		*this -= *iter;
		iter++;
	}
	return *this;
}

template <class T> inline 
const typename RangeArray<T>::value_type& RangeArray<T>::operator [] (size_type pos) const
{
	return _vec[pos];
}

template <class T> inline 
bool RangeArray<T>::empty() const
{
	return _vec.empty();
}

template <class T> inline 
void RangeArray<T>::clear()
{
	_vec.clear();
}

template <class T> inline 
typename RangeArray<T>::size_type RangeArray<T>::size() const
{
	return _vec.size();
}

// Non-member functions

template <class T> inline
const RangeArray<T> operator + (const RangeArray<T>& rs, 
		const typename RangeArray<T>::value_type& r)
{
	RangeArray<T> ret(rs);
	ret += r;
	return ret;
}

template <class T> inline
const RangeArray<T> operator - (const RangeArray<T>& rs, 
		const typename RangeArray<T>::value_type& r)
{
	RangeArray<T> ret(rs);
	ret -= r;
	return ret;
}

template <class T> inline
const RangeArray<T> operator + (const RangeArray<T>& rs1, const RangeArray<T>& rs2)
{
	RangeArray<T> ret(rs1);
	ret += rs2;
	return ret;
}

template <class T> inline
const RangeArray<T> operator - (const RangeArray<T>& rs1, const RangeArray<T>& rs2)
{
	RangeArray<T> ret(rs1);
	ret -= rs2;
	return ret;
}

template <class T> inline
const RangeArray<T> operator & (const RangeArray<T>& rs1, const RangeArray<T>& rs2)
{
	RangeArray<T> tmp(rs1);
	tmp -= rs2;
	return rs1 - tmp;
}

template <class T> inline
const RangeArray<T> operator + (const typename RangeArray<T>::value_type& r1, 
		const typename RangeArray<T>::value_type& r2)
{
	RangeArray<T> ranges;
	ranges += r1;
	ranges += r2;
	return ranges;
}

template <class T> inline
const RangeArray<T> operator - (const typename RangeArray<T>::value_type& r1, 
		const typename RangeArray<T>::value_type& r2)
{
	RangeArray<T> ranges;
	ranges += r1;
	ranges -= r2;
	return ranges;
}

/* Get the range's complement. */
template <class T> inline
const RangeArray<T> operator ~ (const typename RangeArray<T>::value_type& r)
{
	if (r.empty())
		return RangeArray<T>::value_type::full_range;
	else if (r == RangeArray<T>::value_type::full_range)
		return RangeArray<T>::value_type::empty_range;
	else
		return RangeArray<T>::value_type::full_range - r;
}

/* Split ranges to no intersect mode. */
template <class T>
void split_range(std::vector<Range<T> >& ranges)
{
	typedef std::pair<int, int> RangeValue;
	typedef std::vector<RangeValue> RangeValueArray;
	RangeValueArray value_array;
	for (unsigned int i = 0; i < ranges.size(); i++)
	{
		value_array.push_back(RangeValue(ranges[i].min(), -1));
		value_array.push_back(RangeValue(ranges[i].max(), 1));
	}
	// Maybe need a custom compare function.
	std::sort(value_array.begin(), value_array.end());
	ranges.clear();
	RangeValue m, n;
	int t = 0;
	while (!value_array.empty())
	{
		if (t == 0)
		{
			m = value_array.front();
			value_array.erase(value_array.begin());
			t -= m.second;
		}
		n = value_array.front();
		value_array.erase(value_array.begin());
		t -= n.second;
		if (t > 0)
		{
			if (n.second == -1)
			{
				if (n.first > m.first)
					ranges.push_back(Range<T>((T)m.first, (T)(n.first - 1)));
				m = n;
			}
			else
			{
				if (n.first >= m.first)
					ranges.push_back(Range<T>((T)m.first, (T)n.first));
				m.first = n.first + 1;
			}
		}
		else
		{
			if (n.first >= m.first)
				ranges.push_back(Range<T>((T)m.first, (T)n.first));
		}
	}
}



} // End of namespace lex
} // End of namespace tlib

#endif // RANGES_H_
