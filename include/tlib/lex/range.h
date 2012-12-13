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

#ifndef RANGE_H_
#define RANGE_H_

#include "../tlibbase.h"
#include "unsigned.h"


namespace tlib
{

namespace lex
{

//template <class T>
//class NakedRange;

template <class T>
class Range
{
public:
	typedef T value_type;
	inline Range();
	inline Range(T min, T max);
	inline Range(const Range& r);
	//inline Range(const NakedRange<T>& naked);
	inline T min() const;
	inline T max() const;
	inline void set_min(T m);
	inline void set_max(T m);
	inline void set_range(T min, T max);
	inline bool empty() const;
	inline void clear();
	inline Range& operator = (const Range& r);
	inline bool operator == (const Range& r) const;
public:
	static const Range empty_range;
	static const Range full_range;
	static const T full_min;
	static const T full_max;
private:
	T _min;
	T _max;
	bool _empty;	
};

typedef Range<char> CharRange;
typedef Range<unsigned char> UCharRange;
typedef Range<short> ShortRange;
typedef Range<unsigned short> UShortRange;
typedef Range<int> IntRange;
typedef Range<unsigned int> UIntRange;
typedef Range<long> LongRange;
typedef Range<unsigned long> ULongRange;
typedef Range<long long> LLongRange;
typedef Range<unsigned long long> ULLongRange;


template<class T> inline
Range<T>::Range()
: _min(0), _max(0), _empty(true)
{
}

template<class T> inline 
Range<T>::Range(T min, T max)
{
	set_range(min, max);
}

template<class T> inline 
Range<T>::Range(const Range<T>& r)
{
	_min = r._min;
	_max = r._max;
	_empty = r._empty;
}

//template<class T> inline 
//Range<T>::Range(const NakedRange<T>& naked)
//{
//	set_range(naked.min, naked.max);
//}

template<class T> inline
T Range<T>::min() const
{
	return _min;
}

template<class T> inline
T Range<T>::max() const
{
	return _max;
}

template<class T> inline
void Range<T>::set_min(T m)
{
	_min = m;
	if (_min > _max)
		_max = _min;
	_empty = false;
}

template<class T> inline
void Range<T>::set_max(T m)
{
	_max = m;
	if (_max < _min)
		_min = _max;
}

template<class T> inline
void Range<T>::set_range(T min, T max)
{
	if (min <= max)
	{
		_min = min;
		_max = max;
	}
	else
	{
		_min = max;
		_max = min;
	}
	_empty = false;
}

template<class T> inline
bool Range<T>::empty() const
{
	return _empty;
}

template<class T> inline
void Range<T>::clear()
{
	_min = 0;
	_max = 0;
	_empty = true;
}

template<class T> inline
Range<T>& Range<T>::operator = (const Range<T>& r)
{
	_min = r._min;
	_max = r._max;
	_empty = r._empty;
	return *this;
}

template<class T> inline
bool Range<T>::operator == (const Range<T>& r) const
{
	return (_min == r._min && _max == r._max && _empty == r._empty);
}


//// A simple range class can not indicate NULL range.
//template <class T>
//class NakedRange
//{
//public:
//	inline NakedRange(const NakedRange& r);
//	inline NakedRange(const Range<T>& r);
//	inline NakedRange& operator = (const NakedRange& r);
//	inline NakedRange& operator = (const Range<T>& r);
//	T min;
//	T max;
//};
//
//template <class T> inline
//NakedRange<T>::NakedRange(const NakedRange<T>& r)
//: min(r.min), max(r.max)
//{
//}
//
//template <class T> inline 
//NakedRange<T>::NakedRange(const Range<T>& r)
//: min(r.min()), max(r.max())
//{
//}
//
//template <class T> inline 
//NakedRange<T>& NakedRange<T>::operator = (const NakedRange<T>& r)
//{
//	min = r.min;
//	max = r.max;
//	return *this;
//}
//
//template <class T> inline
//NakedRange<T>& NakedRange<T>::operator = (const Range<T>& r)
//{
//	min = r.min();
//	max = r.max();
//	return *this;
//}


/* Relationship of two section. */
typedef enum _range_relation
{
	R_UNRELATED 	= 0,
	R_INSIDE		= 1,
	R_OUTSIDE		= 2,
	R_OVERLAP		= 3,
	R_INTERSECT 	= 4
}
RangeRelation;

/* Get the relationship between r1 and r2. */
template <class T> inline
RangeRelation relation(const Range<T> &r1, const Range<T> &r2)
{
	if (r1.empty() || r2.empty())
		return R_UNRELATED;
	else if (r1.min() > r2.max() || r2.min() > r1.max())
		return R_UNRELATED;
	else if (r1.min() == r2.min() && r1.max() == r2.max())
		return R_OVERLAP;
	else if (r1.min() <= r2.min() && r1.max() >= r2.max())
		return R_OUTSIDE;
	else if (r2.min() <= r1.min() && r2.max() >= r1.max())
		return R_INSIDE;
	else
		return R_INTERSECT;
}

/* Get the intersection of two ranges. */
template <class T> inline
const Range<T> operator & (const Range<T> &r1, const Range<T> &r2)
{
	if (r1.empty() || r2.empty())
		return Range<T>();
	T retmin = r1.min() > r2.min() ? r1.min() : r2.min();
	T retmax = r1.max() < r2.max() ? r1.max() : r2.max();
	if (retmin <= retmax)
		return Range<T>(retmin, retmax);
	else
		return Range<T>();
}


} // End of namespace lex
} // End of namespace tlib
#endif // RANGE_H_
