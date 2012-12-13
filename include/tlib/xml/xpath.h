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


#ifndef XPATH_H_
#define XPATH_H_

#include "../tlibptr.h"
#include "../tlibstr.h"
#include "../lex/lexical.h"
#include <string>
#include <list>

namespace tlib
{
namespace xml
{
namespace xpath
{
// Because I won't implement a full version of XPATH standard,
// so I used a easy way to describe XPATH expression.
class XPath;

typedef struct _Predicate : public tlib::Object
{
	virtual ~_Predicate() = 0;
	typedef enum _exp_type
	{
		E_INTEGER = 1,
		E_FLOAT = 2,
		E_STRING = 3,
		E_PATH = 4,
		E_AND = 7,
		E_OR = 8,
		E_LESS = 9,
		E_LESS_EQUAL = 10,
		E_MORE = 11,
		E_MORE_EQUAL = 12,
		E_EQUAL = 13,
		E_NOT_EQUAL = 14
	} ExpType;
	ExpType type;
	inline _Predicate(ExpType t);
	static RefPtr<struct _Predicate> create(const std::wstring& predicate);
} Predicate;
inline _Predicate::_Predicate(ExpType t)
: type(t)
{
}
typedef tlib::RefPtr<Predicate> PredicatePtr;


typedef struct _PredicateExp : public _Predicate
{
	inline _PredicateExp(ExpType t, PredicatePtr l, PredicatePtr r);
	PredicatePtr left;
	PredicatePtr right;
} PredicateExp;
inline _PredicateExp::_PredicateExp(ExpType t, PredicatePtr l, PredicatePtr r)
: _Predicate(t), left(l), right(r)
{
}
typedef tlib::RefPtr<PredicateExp> PredicateExpPtr;


typedef struct _PredicateValue : public _Predicate
{
	typedef lex::Token<wchar_t> Token;
	Token token;
	inline _PredicateValue(ExpType t, const Token& token);
} PredicateValue;
inline _PredicateValue::_PredicateValue(ExpType t, const Token& tk)
: _Predicate(t), token(tk)
{
}
typedef tlib::RefPtr<PredicateValue> PredicateValuePtr;


typedef struct _PredicateStringValue : public _PredicateValue
{
	std::wstring value;
	inline _PredicateStringValue(const Token& tk);
} PredicateStringValue;
inline _PredicateStringValue::_PredicateStringValue(const Token& tk)
: _PredicateValue(E_STRING, tk), value(tk.str)
{
}
typedef tlib::RefPtr<PredicateStringValue> PredicateStringValuePtr;


typedef struct _PredicateIntegerValue : public _PredicateValue
{
	int value;
	inline _PredicateIntegerValue(const Token& tk);
} PredicateIntegerValue;
inline _PredicateIntegerValue::_PredicateIntegerValue(const Token& tk)
: _PredicateValue(E_INTEGER, tk), value(wstrto<int>(tk.str))
{
}
typedef tlib::RefPtr<PredicateIntegerValue> PredicateIntegerValuePtr;


typedef struct _PredicateFloatValue : public _PredicateValue
{
	float value;
	inline _PredicateFloatValue(const Token& tk);
} PredicateFloatValue;
inline _PredicateFloatValue::_PredicateFloatValue(const Token& tk)
: _PredicateValue(E_FLOAT, tk), value(wstrto<float>(tk.str))
{
}
typedef tlib::RefPtr<PredicateFloatValue> PredicateFloatValuePtr;


class Step
{
public:
	typedef enum _axis_type
	{
		AXIS_CURRENT = 0,
		AXIS_DIRECT,
		AXIS_INDIRECT
	} AxisType;
	AxisType axis;
	std::wstring name;
	PredicatePtr predicate;

	inline Step();
	inline void reset();
};

inline Step::Step() : axis(AXIS_CURRENT)
{
}
inline void Step::reset()
{
	axis = AXIS_CURRENT;
	name.clear();
	predicate.reset();
}

typedef std::list<Step> Path;
typedef std::list<Path> Paths;

class XPath
{
public:
	XPath(const std::wstring& xpath) throw (std::runtime_error);
	inline XPath(const std::string& xpath) throw (std::runtime_error);
public:
	Paths paths;
};
inline XPath::XPath(const std::string& xpath) throw (std::runtime_error)
{
	XPath(locale_to_wstring(xpath));
}

typedef struct _PredicatePathValue : public _PredicateValue
{
	XPath value;
	inline _PredicatePathValue(const Token& tk);
} PredicatePathValue;
inline _PredicatePathValue::_PredicatePathValue(const Token& tk)
: _PredicateValue(E_PATH, tk), value(tk.str)
{
}
typedef tlib::RefPtr<PredicatePathValue> PredicatePathValuePtr;





}
}
}




#endif /* XPATH_H_ */

