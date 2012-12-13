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


#include "xpath.h"
#include "../lex/lexical.h"
#include <stack>

extern const unsigned char path_bc[];
extern const unsigned int path_bc_length;
extern const unsigned char predicate_bc[];
extern const unsigned int predicate_bc_length;

namespace tlib
{
namespace xml
{
namespace xpath
{


#define THROW(x) throw std::runtime_error(x)
static const char* _err_unexpected_token = "Invalid xpath syntax: Unexpected token.";
static const char* _err_unexpected_end = "Invalid xpath syntax: Unexpected end.";
static const char* _err_invalid_predicate = "Invalid xpath syntax: Invalid predicate.";

_Predicate::~_Predicate()
{
}


XPath::XPath(const std::wstring& xpath) throw (std::runtime_error)
{
	// This function has been finished.
	lex::Lexical<wchar_t>::LexicalPtr lex =
			lex::Lexical<wchar_t>::create_by_static_bc(path_bc, path_bc_length);
	std::wistringstream iss(xpath);
	lex->parse(iss);
	enum
	{
		T_ELEMENT = 1,
		T_ATTRIBUTE,
		T_FUNCTION,
		T_SELF,
		T_PARENT,
		T_PREDICATE,
		T_SPLIT,
		T_SPLIT2,
		T_OR
	};
	enum
	{
		S_BEGIN = 0,
		S_SPLIT,
		S_NODE,
		S_END_NODE,
		S_PREDICATE,
		S_END_PREDICATE
	};
	lex::Token<wchar_t> token;
	int state = S_BEGIN;
	Path path;
	Step step;
	while (lex->fetch_next(token))
	{
		if (state == S_BEGIN)
		{
			if (token.action == T_ELEMENT
					|| token.action == T_SELF
					|| token.action == T_PARENT)
			{
				step.axis = Step::AXIS_CURRENT;
				step.name = token.str;
				state = S_NODE;
			}
			else if (token.action == T_FUNCTION
					|| token.action == T_ATTRIBUTE)
			{
				step.axis = Step::AXIS_CURRENT;
				step.name = token.str;
				state = S_END_NODE;
			}
			else if (token.action == T_SPLIT)
			{
				step.axis = Step::AXIS_DIRECT;
				state = S_SPLIT;
			}
			else if (token.action == T_SPLIT2)
			{
				step.axis = Step::AXIS_INDIRECT;
				state = S_SPLIT;
			}
			else
			{
				THROW(_err_unexpected_token);
			}
		}
		else if (state == S_SPLIT)
		{
			if (token.action == T_ELEMENT
					|| token.action == T_SELF
					|| token.action == T_PARENT)
			{
				step.name = token.str;
				state = S_NODE;
			}
			else if (token.action == T_FUNCTION
					|| token.action == T_ATTRIBUTE)
			{
				step.name = token.str;
				state = S_END_NODE;
			}
			else if (token.action == T_OR)
			{
				if (path.empty())
				{
					path.push_back(step);
					paths.push_back(path);
					path.clear();
					step.reset();
					state = S_BEGIN;
				}
				else
					THROW(_err_unexpected_token);
			}
			else
				THROW(_err_unexpected_token);
		}
		else if (state == S_NODE)
		{
			if (token.action == T_PREDICATE)
			{
				step.predicate = Predicate::create(token.str);
				state = S_PREDICATE;
			}
			else if (token.action == T_SPLIT)
			{
				path.push_back(step);
				step.reset();
				step.axis = Step::AXIS_DIRECT;
				state = S_SPLIT;
			}
			else if (token.action == T_SPLIT2)
			{
				path.push_back(step);
				step.reset();
				step.axis = Step::AXIS_INDIRECT;
				state = S_SPLIT;
			}
			else if (token.action == T_OR)
			{
				path.push_back(step);
				paths.push_back(path);
				path.clear();
				step.reset();
				state = S_BEGIN;
			}
			else
			{
				THROW(_err_unexpected_token);
			}
		}
		else if (state == S_END_NODE)
		{
			if (token.action == T_PREDICATE)
			{
				step.predicate = Predicate::create(token.str);
				state = S_END_PREDICATE;
			}
			else if (token.action == T_OR)
			{
				path.push_back(step);
				paths.push_back(path);
				path.clear();
				step.reset();
				state = S_BEGIN;
			}
			else
			{
				THROW(_err_unexpected_token);
			}
		}
		else if (state == S_PREDICATE)
		{
			if (token.action == T_SPLIT)
			{
				path.push_back(step);
				step.reset();
				step.axis = Step::AXIS_DIRECT;
				state = S_SPLIT;
			}
			else if (token.action == T_SPLIT2)
			{
				path.push_back(step);
				step.reset();
				step.axis = Step::AXIS_INDIRECT;
				state = S_SPLIT;
			}
			else if (token.action == T_OR)
			{
				path.push_back(step);
				paths.push_back(path);
				path.clear();
				step.reset();
				state = S_BEGIN;
			}
			else
			{
				THROW(_err_unexpected_token);
			}
		}
		else if (state == S_END_PREDICATE)
		{
			if (token.action == T_OR)
			{
				path.push_back(step);
				paths.push_back(path);
				path.clear();
				step.reset();
				state = S_BEGIN;
			}
			else
			{
				THROW(_err_unexpected_token);
			}
		}
	}
	if (state == S_PREDICATE || state == S_END_PREDICATE
				|| state == S_NODE || state == S_END_NODE)
	{
		path.push_back(step);
		paths.push_back(path);
	}
	else if (state == S_SPLIT)
	{
		if (path.empty())
		{
			path.push_back(step);
			paths.push_back(path);
		}
		else
			THROW(_err_unexpected_token);
	}
	else
	{
		THROW(_err_unexpected_end);
	}
}


// Create predicate for XPath expression
PredicatePtr _Predicate::create(const std::wstring& predicate)
{
	enum
	{
		TK_EOF = 0,
		TK_INTEGER = 1, TK_FLOAT, TK_STRING, TK_PATH,
		TK_BEGIN,
		TK_END,
		TK_AND,
		TK_OR,
		TK_LS, TK_LQ, TK_ME, TK_MQ, TK_EQ, TK_NEQ
	};
	typedef enum _OperatorType
	{
		OP_EOF = 0, OP_VALUE = 1, OP_BEGIN, OP_END, OP_AND, OP_OR, OP_LOGIC
	} OperatorType;
	// Use 'tk_mp' map the token to expression element.
	static OperatorType tk_mp[TK_NEQ + 1] =
	{
		OP_EOF,
		OP_VALUE, OP_VALUE, OP_VALUE, OP_VALUE,
		OP_BEGIN,
		OP_END,
		OP_AND,
		OP_OR,
		OP_LOGIC, OP_LOGIC, OP_LOGIC, OP_LOGIC, OP_LOGIC, OP_LOGIC
	};

	/* Priority is use "Queued operator" .VS. "Current token". */
	static char priority_table[OP_LOGIC + 1][OP_LOGIC + 1] =
	{
	/*_Que \ Cur___#__val__(___)___&___|__log_*/
	/*|  #  |*/ { '=','?','<','?','<','<','<' },
	/*| val |*/ { '?','?','?','?','?','?','?' },
	/*|  (  |*/ { '?','?','<','=','<','<','<' },
	/*|  )  |*/ { '>','?','?','>','>','>','>' },
	/*|  &  |*/ { '>','?','<','>','>','>','<' },
	/*|  |  |*/ { '>','?','<','>','<','>','<' },
	/*| log |*/ { '>','?','<','>','>','>','>' }
	};

	lex::Lexical<wchar_t>::LexicalPtr lex =
			lex::Lexical<wchar_t>::create_by_static_bc(predicate_bc, predicate_bc_length);
	std::wistringstream iss(predicate.substr(1, predicate.size() - 2));
	lex->parse(iss);
	std::stack<unsigned int> op_stack;
	std::stack<PredicatePtr> exp_stack;
	lex::Token<wchar_t> token;
	op_stack.push(TK_EOF);
	bool is_eof = false;
	while (!is_eof)
	{
		lex->next_token(token);
		if (token.action == 0 && token.length == 0)
			is_eof = true;
		if (token.action >= TK_INTEGER
				&& token.action <= TK_PATH)
		{
			if (token.action == TK_INTEGER)
			{
				PredicatePtr p(new PredicateIntegerValue(token));
				exp_stack.push(p);
			}
			else if (token.action == TK_FLOAT)
			{
				PredicatePtr p(new PredicateFloatValue(token));
				exp_stack.push(p);
			}
			else if (token.action == TK_STRING)
			{
				PredicatePtr p(new PredicateStringValue(token));
				exp_stack.push(p);
			}
			else if (token.action == TK_PATH)
			{
				PredicatePtr p(new PredicatePathValue(token));
				exp_stack.push(p);
			}
			else
				THROW(_err_invalid_predicate);
		}
		else
		{
			if ((token.action >= TK_BEGIN && token.action <= TK_NEQ)
					|| (token.action == TK_EOF && token.length == 0))
			{
				for (;;)
				{
					OperatorType queue_op = tk_mp[op_stack.top()];
					OperatorType current_op = tk_mp[token.action];
					if (priority_table[queue_op][current_op] == '<')
					{
						op_stack.push(token.action);
						break;
					}
					else if (priority_table[queue_op][current_op] == '=')
					{
						op_stack.pop();
						break;
					}
					else if (priority_table[queue_op][current_op] == '>')
					{
						unsigned int op = op_stack.top();
						op_stack.pop();

						if (exp_stack.empty())
							THROW(_err_unexpected_token);
						PredicatePtr right = exp_stack.top();
						exp_stack.pop();
						if (exp_stack.empty())
							THROW(_err_unexpected_token);
						PredicatePtr left = exp_stack.top();
						exp_stack.pop();
						PredicatePtr p(new PredicateExp((ExpType)op, left, right));
						exp_stack.push(p);
					}
					else
						THROW(_err_invalid_predicate);
				}
			}
			else
				THROW(_err_invalid_predicate);
		}
	}
	if (op_stack.size() == 0 && exp_stack.size() == 1)
	{
		return exp_stack.top();
	}
	else
		THROW(_err_invalid_predicate);
}

}
}
}
