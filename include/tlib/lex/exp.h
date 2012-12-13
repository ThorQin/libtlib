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

#ifndef EXP_H_
#define EXP_H_


#include "scan.h"
#include <stdexcept>
#include <memory>

namespace tlib
{

namespace lex
{

typedef enum _NodeType
{
	RE_NODE_RANGE,
	RE_NODE_AND,
	RE_NODE_OR,
	RE_NODE_QUESTION,
	RE_NODE_PLUS,
	RE_NODE_CLOSURE
} NodeType;


typedef unsigned int Action;

template <class T>
class Exp
{
public:
	typedef std::shared_ptr<Exp<T> > ExpPtr;
	Exp(const Range<T>& r);
	Exp(NodeType t, ExpPtr c);
	Exp(NodeType t, ExpPtr l, ExpPtr r);
public:
	NodeType type;

	// range, exp or child, choose one between three.
	Range<T> range;
	struct
	{
		ExpPtr left;
		ExpPtr right;
	}
	exp;
	ExpPtr child;

	bool nullable;
	std::vector<unsigned int> firstpos;
	std::vector<unsigned int> lastpos;
public:
	Action action;
};

typedef enum _JoinType
{
	RE_JOIN_AND = 0,
	RE_JOIN_OR
} JoinType;

template <class T> inline
typename Exp<T>::ExpPtr exp_join(typename Exp<T>::ExpPtr l, typename Exp<T>::ExpPtr r, JoinType type);

typedef enum _PackType
{
	RE_PACK_0_M,
	RE_PACK_1_M,
	RE_PACK_0_1
} PackType;

template <class T> inline
typename Exp<T>::ExpPtr exp_package(typename Exp<T>::ExpPtr c, PackType type);

/* Create a new regular expression that parse from a string.*/
template <class T>
typename Exp<T>::ExpPtr exp_parse(const T* expression) throw (std::runtime_error);

/* Do a depth-first traversing */

//typedef void (*TravelCallback)(typename Exp<T>::ExpPtr exp, void *param);
template <class T>
void exp_depth_travel(typename Exp<T>::ExpPtr exp, 
		void (*callback)(typename Exp<T>::ExpPtr exp, void *param), void* param);


template <class T> inline
typename Exp<T>::ExpPtr exp_join(typename Exp<T>::ExpPtr l, typename Exp<T>::ExpPtr r, JoinType type)
{
	return typename Exp<T>::ExpPtr(new Exp<T>((type == RE_JOIN_AND ? RE_NODE_AND : RE_NODE_OR), l, r));
}


template <class T> inline
typename Exp<T>::ExpPtr exp_package(typename Exp<T>::ExpPtr c, PackType type)
{
	static const NodeType pack[RE_PACK_0_1 + 1] =
	{ RE_NODE_CLOSURE, RE_NODE_PLUS, RE_NODE_QUESTION };
	return typename Exp<T>::ExpPtr(new Exp<T>(pack[type], c));
}


} // End of namespace lex
} // End of namespace tlib

#endif /* EXP_H_ */

