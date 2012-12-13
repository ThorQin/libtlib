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

#ifndef DFA_H_
#define DFA_H_


#include "exp.h"

namespace tlib
{
namespace lex
{

typedef struct _Transit
{
	unsigned int from;
	unsigned int to;
	unsigned int input;
	Action action;
} Transit;

typedef std::vector<Transit> TransitArray;
typedef std::vector<unsigned int> AcceptArray;


template <class T>
struct Dfa
{
	typedef std::vector<Range<T> > RangeVector;
	typedef Range<T> RangeType;
	TransitArray transits;
	AcceptArray accepting_states;
	RangeVector range_map;
	unsigned int total_states;
};

/* Compute the expression's node values.  */
template <class T>
void make_dfa(typename Exp<T>::ExpPtr exp, Dfa<T>& dfa);

} // End of namespace lex
} // End of namespace tlib

#endif /* DFA_H_ */
