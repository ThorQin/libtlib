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

#ifndef SCAN_H_
#define SCAN_H_

#include <queue>
#include "ranges.h"

namespace tlib
{

namespace lex
{

typedef enum _TokenType
{
	TOKEN_VALUE = 0,
	TOKEN_OPERATOR,
	TOKEN_ERR
}
ScanTokenType;

typedef enum _OperatorType
{
	OP_AND = 0,
	OP_OR,
	OP_QUESTION,
	OP_PLUS,
	OP_CLOSURE,
	OP_L_BRACKET,
	OP_R_BRACKET,
	OP_EOF
}
OperatorType;

/* Scan token. */
template <class T>
class ScanTokenT
{
public:
	ScanTokenType type;
	// range or op, choose between the two.
	Range<T> range;
	OperatorType op;
};

class ScanPos
{
public:
	unsigned int line;
	unsigned int column;
};


/* Scanning context for parse regular expression. */
template <class T>
class ExpScanner
{
public:
	typedef T ScanChar;
	typedef ScanTokenT<T> ScanToken;
	ExpScanner(const ScanChar* parse_data);
	ScanTokenType next_token(ScanToken& token);
	bool rt_s_err(ScanToken& token);
	bool rt_s_eof(ScanToken& token);
	bool rt_s_op_and(ScanToken& token);
	bool rt_s_op_or(ScanToken& token);
	bool rt_s_char(ScanToken& token);
	bool rt_s_dot(ScanToken& token);
	bool rt_s_plus(ScanToken& token);
	bool rt_s_question(ScanToken& token);
	bool rt_s_closure(ScanToken& token);
	bool rt_s_lrb(ScanToken& token);
	bool rt_s_rrb(ScanToken& token);
	bool rt_s_lsb(ScanToken& token);
	bool rt_s_rsb(ScanToken& token);
	bool rt_s_esc_x(ScanToken& token);
	bool rt_s_esc_0(ScanToken& token);
	bool rt_s_esc_00(ScanToken& token);
	bool rt_s_esc_char(ScanToken& token);
	bool rt_sr_neg(ScanToken& token);
	bool rt_sr_first(ScanToken& token);
	bool rt_sr_second(ScanToken& token);
	bool rt_sr_s_esc_x(ScanToken& token);
	bool rt_sr_s_esc_0(ScanToken& token);
	bool rt_sr_s_esc_00(ScanToken& token);
	bool rt_sr_s_esc_char(ScanToken& token);
	bool rt_sr_e_esc_x(ScanToken& token);
	bool rt_sr_e_esc_0(ScanToken& token);
	bool rt_sr_e_esc_00(ScanToken& token);
	bool rt_sr_e_esc_char(ScanToken& token);
	bool rt_pass(ScanToken& token);
private:
	typedef bool (ExpScanner<T>::*REStateRoutine)(
		typename ExpScanner<T>::ScanToken& token);
	REStateRoutine get_state_routines(int scan_state);
	inline void push_token(ScanToken& token);
	inline void pop_token(ScanToken& token);
	inline void reset_range();
	void next_char();
public:
	const ScanChar* src;
	ScanPos pos;
private:
	bool _neg;
	const ScanChar* _scan;
	int _scan_state;
	std::queue<ScanToken> _token_buffer;
	RangeArray<T> _ranges;
	ScanChar _first_char;
};

} // End of namespace lex
} // End of namespace tlib

#endif /* SCAN_H_ */

