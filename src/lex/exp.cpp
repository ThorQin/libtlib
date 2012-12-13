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

#include "../../include/tlib/lex/exp.h"
#include <string>
#include <vector>
#include <sstream>
#include <stack>
#include <map>
#include <set>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

namespace tlib
{
namespace lex
{

using namespace std;



template <class T>
Exp<T>::Exp(const Range<T>& r)
{
	type = RE_NODE_RANGE;
	range = r;
	action = 0;
}

/* Create a AND or OR node. */
template <class T>
Exp<T>::Exp(NodeType t, ExpPtr l, ExpPtr r)
{
	type = t;
	exp.left = l;
	exp.right = r;
	action = 0;
}

/* Create a "? + *" operator node. */
template <class T>
Exp<T>::Exp(NodeType t, ExpPtr c)
{
	type = t;
	child = c;
	action = 0;
}


/* Priority is use "Queued operator" .VS. "Current operator". */
static char priority_map[OP_EOF + 1][OP_EOF + 1] =
{
/*__Queue \ Scan________&____|____?____+____*____(____)____#________*/
/*~~[ & ]~~~~~~*/    { '>', '>', '<', '<', '<', '<', '>', '>' },
/*~~[ | ]~~~~~~*/    { '<', '>', '<', '<', '<', '<', '>', '>' },
/*~~[ ? ]~~~~~~*/    { '>', '>', '>', '>', '>', '>', '>', '>' },
/*~~[ + ]~~~~~~*/    { '>', '>', '>', '>', '>', '>', '>', '>' },
/*~~[ * ]~~~~~~*/    { '>', '>', '>', '>', '>', '>', '>', '>' },
/*~~[ ( ]~~~~~~*/    { '<', '<', '<', '<', '<', '<', '=', '?' },
/*~~[ ) ]~~~~~~*/    { '>', '>', '<', '<', '<', '?', '>', '>' },
/*~~[ # ]~~~~~~*/    { '<', '<', '<', '<', '<', '<', '?', '=' }
};

/* Compare the queued operator and scan operator and make expression. */
template <class T>
bool parse_op(stack<OperatorType> &op_stack,
		stack<typename Exp<T>::ExpPtr>& exp_stack,
		OperatorType scan_op)
{
	for (;;)
	{
		if (op_stack.empty())
			return false;
		OperatorType queue_op = op_stack.top();
		if (priority_map[queue_op][scan_op] == '>')
		{
			op_stack.pop();
			if (queue_op == OP_AND || queue_op == OP_OR)
			{
				if (exp_stack.empty())
					return false;
				typename Exp<T>::ExpPtr exp_right = exp_stack.top();
				exp_stack.pop();
				if (exp_stack.empty())
					return false;

				typename Exp<T>::ExpPtr exp_left = exp_stack.top();
				exp_stack.pop();
				typename Exp<T>::ExpPtr exp_op(new Exp<T>(static_cast<NodeType>
					(RE_NODE_AND + (queue_op - OP_AND)), exp_left, exp_right));
				exp_stack.push(exp_op);
			}
			else
				return false;
		}
		else if (priority_map[queue_op][scan_op] == '<')
		{
			op_stack.push(scan_op);
			return true;
		}
		else if (priority_map[queue_op][scan_op] == '=')
		{
			op_stack.pop();
			return true;
		}
		else // priority_map[queue_op][scan_op] == '?'
		{
			return false;
		}
	}
}

template Exp<char>::ExpPtr exp_parse(const char* expression) throw(runtime_error);
template Exp<unsigned char>::ExpPtr exp_parse(const unsigned char* expression) throw(runtime_error);
template Exp<wchar_t>::ExpPtr exp_parse(const wchar_t* expression) throw(runtime_error);

template <class T>
typename Exp<T>::ExpPtr exp_parse(const T* expression) throw(runtime_error)
{
	ExpScanner<T> scanner(expression);
	stack<OperatorType> op_stack;
	stack<typename Exp<T>::ExpPtr> exp_stack;
	typename ExpScanner<T>::ScanToken token;
	static const char* const error_messge = "expression parse error.";

	op_stack.push(OP_EOF);
	for (;;)
	{
		/* Get next token. */
		scanner.next_token(token);
		if (token.type == TOKEN_VALUE)
		{
			typename Exp<T>::ExpPtr exp(new Exp<T>(token.range));
			exp_stack.push(exp);
		}
		else if (token.type == TOKEN_OPERATOR)
		{
			OperatorType scan_op = token.op;
			if (scan_op >= OP_QUESTION && scan_op <= OP_CLOSURE)
			{
				if (exp_stack.empty())
					goto ErrorClean;
				typename Exp<T>::ExpPtr exp_child = exp_stack.top();
				exp_stack.pop();
				typename Exp<T>::ExpPtr exp_op(new Exp<T>(static_cast<NodeType>
					(RE_NODE_QUESTION + (scan_op - OP_QUESTION)), exp_child));
				exp_stack.push(exp_op);
			}
			else
			{
				if (!parse_op<T>(op_stack, exp_stack, scan_op))
					goto ErrorClean;
				if (scan_op == OP_EOF)
					break;
			}
		}
		else if (token.type == TOKEN_ERR)
		{
			goto ErrorClean;
		}
	}
	if (op_stack.empty() && exp_stack.size() == 1)
	{
		typename Exp<T>::ExpPtr exp = exp_stack.top();
		exp_stack.pop();
		return exp;
	}
#ifdef DEBUG
	else
	{
		std::cout << "op_stack size: " << op_stack.size() << std::endl;
		if (op_stack.size() > 0)
			std::cout << "op_stack last op: " << op_stack.top() << std::endl;

		std::cout << "exp_stack size: " << exp_stack.size() << std::endl;
	}
#endif
ErrorClean:
	throw runtime_error(error_messge);
}

template <class T>
struct ScanExp
{
	typename Exp<T>::ExpPtr exp;
	int state;
};

// Do a depth-first traversing
template
void exp_depth_travel<char>(Exp<char>::ExpPtr exp, 
		void (*callback)(Exp<char>::ExpPtr exp, void *param), 
		void *param);
template
void exp_depth_travel<unsigned char>(Exp<unsigned char>::ExpPtr exp, 
		void (*callback)(Exp<unsigned char>::ExpPtr exp, void *param), 
		void *param);
template 
void exp_depth_travel<wchar_t>(Exp<wchar_t>::ExpPtr exp, 
		void (*callback)(Exp<wchar_t>::ExpPtr exp, void *param), 
		void *param);


template <class T>
void exp_depth_travel(typename Exp<T>::ExpPtr exp, 
		void (*callback)(typename Exp<T>::ExpPtr exp, void *param), 
		void *param)
{
	stack<ScanExp<T> > scan_stack;
	ScanExp<T> root_scan =
	{ exp, 0 };
	scan_stack.push(root_scan);
	while (!scan_stack.empty())
	{
		ScanExp<T>& scan = scan_stack.top();
		if (scan.exp->type == RE_NODE_AND || scan.exp->type == RE_NODE_OR)
		{
			if (scan.state == 0)
			{
				ScanExp<T> tmp_scan =
				{ scan.exp->exp.left, 0 };
				scan_stack.push(tmp_scan);
				scan.state = 1;
			}
			else if (scan.state == 1)
			{
				ScanExp<T> tmp_scan =
				{ scan.exp->exp.right, 0 };
				scan_stack.push(tmp_scan);
				scan.state = 2;
			}
			else
			{
				callback(scan.exp, param);
				scan_stack.pop();
			}
		}
		else if (scan.exp->type == RE_NODE_RANGE)
		{
			callback(scan.exp, param);
			scan_stack.pop();
		}
		else
		{
			if (scan.state == 0)
			{
				ScanExp<T> tmp_scan =
				{ scan.exp->child, 0 };
				scan_stack.push(tmp_scan);
				scan.state = 1;
			}
			else
			{
				callback(scan.exp, param);
				scan_stack.pop();
			}
		}
	}
}

template class Exp<char>;
template class Exp<unsigned char>;
template class Exp<wchar_t>;

} // End of namespace lex
} // End of namespace tlib

