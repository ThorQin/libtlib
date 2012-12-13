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

#include "scan.h"
#include <string.h>

namespace tlib
{

namespace lex
{

/* States for scan regular expression. */
typedef enum _REScanState
{
	RE_S_ERR = 0,
	RE_S_BEGIN,
	RE_S_CHARACTER,
	RE_S_CHARACTER_DOT,
	RE_S_OPERATOR_AND,
	RE_S_OPERATOR_OR,
	RE_S_OPERATOR_PLUS,
	RE_S_OPERATOR_QUESTION,
	RE_S_OPERATOR_CLOSURE,
	RE_S_L_ROUND_BRACKET,
	RE_S_R_ROUND_BRACKET,
	RE_S_L_SQUARE_BRACKET,
	RE_S_R_SQUARE_BRACKET,
	RE_S_ESC,
	RE_S_ESC_X,
	RE_S_ESC_X0,
	RE_S_ESC_X00,
	RE_S_ESC_CHARACTER,
	RE_SR_NEGATOR,
	RE_SR_CHARACTER_START,
	RE_SR_CHARACTER_END,
	RE_SR_S_ESC,
	RE_SR_S_ESC_X,
	RE_SR_S_ESC_X0,
	RE_SR_S_ESC_X00,
	RE_SR_S_ESC_CHARACTER,
	RE_SR_E_ESC,
	RE_SR_E_ESC_X,
	RE_SR_E_ESC_X0,
	RE_SR_E_ESC_X00,
	RE_SR_E_ESC_CHARACTER,
	RE_SR_CONNECTOR,
	RE_S_END
}
ScanState;

/* Character class for scan regular expression. */
typedef enum _REScanCharacter
{
	RE_C_DOT = 0, /* . */
	RE_C_OR, /* | */
	RE_C_PLUS, /* + */
	RE_C_MINUS, /* - */
	RE_C_QUESTION, /* ? */
	RE_C_ASTERISK, /* * */
	RE_C_SQUARE, /* ^ */
	RE_C_BACK_SLANT, /* \ */
	RE_C_L_ROUND_BRACKET, /* ( */
	RE_C_R_ROUND_BRACKET, /* ) */
	RE_C_L_SQUARE_BRACKET, /* [ */
	RE_C_R_SQUARE_BRACKET, /* ] */
	RE_C_HEX_CHARACTER, /* [0-9a-fA-F] */
	RE_C_OTHER_CHARACTER, /* other charactor. */
	RE_C_ESC_X, /* x */
	RE_C_EOF
}
ScanCharacter;

static int _character_map[128] =
{
	15, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13,  8,  9,  5,  2, 13,  3,  0, 13,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13,  4,
	13, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 10,  7, 11,  6, 13,
	13, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 14, 13, 13, 13,  1, 13, 13, 13
};

static inline int map_character(int ch)
{
	if (ch >= 0 && ch < 128)
		return _character_map[ch];
	else
		return 13;
}

/* State transition adjacency matrix. */
static int state_matrix[RE_S_END + 1][RE_C_EOF + 1] =
{
/*_________________________________.___|___+___-___?___*___^___\___(___)___[___]__Hex__o___x___#______*/
/*(0)RE_S_ERR = 0,*/			{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
/*(1)RE_S_BEGIN,*/				{  3,  0,  0,  2,  0,  0,  2, 13,  9,  0, 11,  0,  2,  2,  2, 32 },
/*(2)RE_S_CHARACTER,*/			{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(3)RE_S_CHARACTER_DOT,*/		{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(4)RE_S_OPERATOR_AND,*/		{  3,  0,  0,  2,  0,  0,  2, 13,  9,  0, 11,  0,  2,  2,  2,  0 },
/*(5)RE_S_OPERATOR_OR,*/		{  3,  0,  0,  2,  0,  0,  2, 13,  9,  0, 11,  0,  2,  2,  2,  0 },
/*(6)RE_S_OPERATOR_PLUS,*/		{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(7)RE_S_OPERATOR_QUESTION,*/	{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(8)RE_S_OPERATOR_CLOSURE,*/	{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(9)RE_S_L_ROUND_BRACKET,*/	{  3,  0,  0,  2,  0,  0,  2, 13,  9, 10, 11,  0,  2,  2,  2,  0 },
/*(10)RE_S_R_ROUND_BRACKET,*/	{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(11)RE_S_L_SQUARE_BRACKET,*/	{  0,  0,  0,  0,  0,  0, 18, 21,  0,  0,  0,  0, 19, 19, 19,  0 },
/*(12)RE_S_R_SQUARE_BRACKET,*/	{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(13)RE_S_ESC,*/				{ 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 14,  0 },
/*(14)RE_S_ESC_X,*/				{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0, 15,  4,  4, 32 },
/*(15)RE_S_ESC_X0,*/			{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0, 16,  4,  4, 32 },
/*(16)RE_S_ESC_X00,*/			{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(17)RE_S_ESC_CHARACTER,*/		{  4,  5,  6,  4,  7,  8,  4,  4,  4, 10,  4,  0,  4,  4,  4, 32 },
/*(18)RE_SR_NEGATOR,*/			{  0,  0,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0, 19, 19, 19,  0 },
/*(19)RE_SR_CHARACTER_START,*/	{  0,  0,  0, 31,  0,  0,  0, 21,  0,  0,  0, 12, 19, 19, 19,  0 },
/*(20)RE_SR_CHARACTER_END,*/	{  0,  0,  0,  0,  0,  0,  0, 21,  0,  0,  0, 12, 19, 19, 19,  0 },
/*(21)RE_SR_S_ESC,*/			{ 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 22,  0 },
/*(22)RE_SR_S_ESC_X,*/			{  0,  0,  0, 31,  0,  0,  0, 21,  0,  0,  0, 12, 23, 19, 19,  0 },
/*(23)RE_SR_S_ESC_X0,*/			{  0,  0,  0, 31,  0,  0,  0, 21,  0,  0,  0, 12, 24, 19, 19,  0 },
/*(24)RE_SR_S_ESC_X00,*/		{  0,  0,  0, 31,  0,  0,  0, 21,  0,  0,  0, 12, 19, 19, 19,  0 },
/*(25)RE_SR_S_ESC_CHARACTER,*/	{  0,  0,  0, 31,  0,  0,  0, 21,  0,  0,  0, 12, 19, 19, 19,  0 },
/*(26)RE_SR_E_ESC,*/			{ 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 27,  0 },
/*(27)RE_SR_E_ESC_X,*/			{  0,  0,  0,  0,  0,  0,  0, 21,  0,  0,  0, 12, 28, 19, 19,  0 },
/*(28)RE_SR_E_ESC_X0,*/			{  0,  0,  0,  0,  0,  0,  0, 21,  0,  0,  0, 12, 29, 19, 19,  0 },
/*(29)RE_SR_E_ESC_X00,*/		{  0,  0,  0,  0,  0,  0,  0, 21,  0,  0,  0, 12, 19, 19, 19,  0 },
/*(30)RE_SR_E_ESC_CHARACTER,*/	{  0,  0,  0,  0,  0,  0,  0, 21,  0,  0,  0, 12, 19, 19, 19,  0 },
/*(31)RE_SR_CONNECTOR,*/		{  0,  0,  0,  0,  0,  0,  0, 26,  0,  0,  0,  0, 20, 20, 20,  0 },
/*(32)RE_S_END*/				{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }
};





template <class T> inline
typename ExpScanner<T>::REStateRoutine ExpScanner<T>::get_state_routines(int scan_state)
{
	static typename ExpScanner<T>::REStateRoutine re_state_routines[RE_S_END + 1] =
	{
	/* 0*/&ExpScanner<T>::rt_s_err,
	/* 1*/0,
	/* 2*/&ExpScanner<T>::rt_s_char,
	/* 3*/&ExpScanner<T>::rt_s_dot,
	/* 4*/&ExpScanner<T>::rt_s_op_and,
	/* 5*/&ExpScanner<T>::rt_s_op_or,
	/* 6*/&ExpScanner<T>::rt_s_plus,
	/* 7*/&ExpScanner<T>::rt_s_question,
	/* 8*/&ExpScanner<T>::rt_s_closure,
	/* 9*/&ExpScanner<T>::rt_s_lrb,
	/*10*/&ExpScanner<T>::rt_s_rrb,
	/*11*/&ExpScanner<T>::rt_s_lsb,
	/*12*/&ExpScanner<T>::rt_s_rsb,
	/*13*/&ExpScanner<T>::rt_pass,
	/*14*/&ExpScanner<T>::rt_s_esc_x,
	/*15*/&ExpScanner<T>::rt_s_esc_0,
	/*16*/&ExpScanner<T>::rt_s_esc_00,
	/*17*/&ExpScanner<T>::rt_s_esc_char,
	/*18*/&ExpScanner<T>::rt_sr_neg,
	/*19*/&ExpScanner<T>::rt_sr_first,
	/*20*/&ExpScanner<T>::rt_sr_second,
	/*21*/&ExpScanner<T>::rt_pass,
	/*22*/&ExpScanner<T>::rt_sr_s_esc_x,
	/*23*/&ExpScanner<T>::rt_sr_s_esc_0,
	/*24*/&ExpScanner<T>::rt_sr_s_esc_00,
	/*25*/&ExpScanner<T>::rt_sr_s_esc_char,
	/*26*/&ExpScanner<T>::rt_pass,
	/*27*/&ExpScanner<T>::rt_sr_e_esc_x,
	/*28*/&ExpScanner<T>::rt_sr_e_esc_0,
	/*29*/&ExpScanner<T>::rt_sr_e_esc_00,
	/*30*/&ExpScanner<T>::rt_sr_e_esc_char,
	/*31*/&ExpScanner<T>::rt_pass,
	/*32*/&ExpScanner<T>::rt_s_eof
	};
	return re_state_routines[scan_state];
}


template <class T>
void ExpScanner<T>::push_token(ScanToken &token)
{
	_token_buffer.push(token);
}

template <class T>
void ExpScanner<T>::pop_token(ScanToken &token)
{
	token = _token_buffer.front();
	_token_buffer.pop();
}

template <class T>
void ExpScanner<T>::reset_range()
{
	_ranges.clear();
}

/* Record line and column position. */
template <class T>
void ExpScanner<T>::next_char()
{
	if (*_scan == '\n')
	{
		pos.line++;
		pos.column = 1;
	}
	else
		pos.column++;
	_scan++;
}


static const unsigned char hex_decode_mask[128] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


template <class T>
static T hex_value(T ch)
{
	if (ch >= 0 && ch < 128)
		return hex_decode_mask[(int)ch];
	else
		return 0;
}

template <class T>
static T hex_value2(T ch1, T ch2)
{
	return (hex_value<T>(ch1) << 4) | hex_value<T>(ch2);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~  State routine procedure.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
bool ExpScanner<T>::rt_s_err(ScanToken &token)
{
	token.type = TOKEN_ERR;
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_eof(ScanToken &token)
{
	token.type = TOKEN_OPERATOR;
	token.op = OP_EOF;
	return true;
}
template <class T>
bool ExpScanner<T>::rt_s_op_and(ScanToken &token)
{
	token.type = TOKEN_OPERATOR;
	token.op = OP_AND;
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_op_or(ScanToken &token)
{
	token.type = TOKEN_OPERATOR;
	token.op = OP_OR;
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_char(ScanToken &token)
{
	token.type = TOKEN_VALUE;
	token.range = Range<T>(*_scan, *_scan);
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_dot(ScanToken &token)
{
	token.type = TOKEN_VALUE;
	token.range = Range<T>::full_range;
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_plus(ScanToken &token)
{
	token.type = TOKEN_OPERATOR;
	token.op = OP_PLUS;
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_question(ScanToken &token)
{
	token.type = TOKEN_OPERATOR;
	token.op = OP_QUESTION;
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_closure(ScanToken &token)
{
	token.type = TOKEN_OPERATOR;
	token.op = OP_CLOSURE;
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_lrb(ScanToken &token)
{
	token.type = TOKEN_OPERATOR;
	token.op = OP_L_BRACKET;
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_rrb(ScanToken &token)
{
	token.type = TOKEN_OPERATOR;
	token.op = OP_R_BRACKET;
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_lsb(ScanToken& /*token*/)
{
	ScanToken tmp_token;
	tmp_token.type = TOKEN_OPERATOR;
	tmp_token.op = OP_L_BRACKET;
	push_token(tmp_token);
	reset_range();
	_neg = false;
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_s_rsb(ScanToken &token)
{
	size_t i = 0, total = _ranges.size();
	ScanToken tmp_token;
	while (i < total)
	{
		if (i > 0)
		{
			tmp_token.type = TOKEN_OPERATOR;
			tmp_token.op = OP_OR;
			push_token(tmp_token);
		}
		tmp_token.type = TOKEN_VALUE;
		tmp_token.range = _ranges[i];
		push_token(tmp_token);
		++i;
	}
	tmp_token.type = TOKEN_OPERATOR;
	tmp_token.op = OP_R_BRACKET;
	push_token(tmp_token);

	/* So extract the first token. */
	pop_token(token);
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_esc_x(ScanToken &token)
{
	if (map_character(*(_scan + 1)) != RE_C_HEX_CHARACTER)
	{
		token.type = TOKEN_VALUE;
		token.range = Range<T>('x', 'x');
		next_char();
		return true;
	}
	else
	{
		next_char();
		return false;
	}
}

template <class T>
bool ExpScanner<T>::rt_s_esc_0(ScanToken &token)
{
	if (map_character(*(_scan + 1)) != RE_C_HEX_CHARACTER)
	{
		token.type = TOKEN_VALUE;
		ScanChar ch = hex_value<T>(*_scan);
		token.range = Range<T>(ch, ch);
		next_char();
		return true;
	}
	else
	{
		next_char();
		return false;
	}
}

template <class T>
bool ExpScanner<T>::rt_s_esc_00(ScanToken &token)
{
	token.type = TOKEN_VALUE;
	ScanChar ch = hex_value2<T>(*(_scan - 1), *_scan);
	token.range = Range<T>(ch, ch);
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_s_esc_char(ScanToken &token)
{
	token.type = TOKEN_VALUE;
	ScanChar ch;
	if (*_scan == 'n')
		ch = '\n';
	else if (*_scan == 't')
		ch = '\t';
	else if (*_scan == 'r')
		ch = '\r';
	else if (*_scan == 's')
		ch = ' ';
	else if (*_scan == 'v')
		ch = '\v';
	else if (*_scan == 'f')
		ch = '\f';
	else if (*_scan == 'b')
		ch = '\b';
	else if (*_scan == 'a')
		ch = '\a';
	else if (*_scan == '0')
		ch = 0;
	else
		ch = *_scan;
	token.range = Range<T>(ch, ch);
	next_char();
	return true;
}

template <class T>
bool ExpScanner<T>::rt_sr_neg(ScanToken& /*token*/)
{
	_neg = true;
	_ranges += Range<T>::full_range;
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_first(ScanToken& /*token*/)
{
	if (*(_scan + 1) != '-')
	{
		if (_neg)
			_ranges -= Range<T>(*_scan, *_scan);
		else
			_ranges += Range<T>(*_scan, *_scan);
	}
	else
		_first_char = *_scan;
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_second(ScanToken& /*token*/)
{
	if (_neg)
		_ranges -= Range<T>(_first_char, *_scan);
	else
		_ranges += Range<T>(_first_char, *_scan);
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_s_esc_x(ScanToken& /*token*/)
{
	if (map_character(*(_scan + 1)) != RE_C_HEX_CHARACTER)
	{
		if (*(_scan + 1) != '-')
		{
			if (_neg)
				_ranges -= Range<T>('x', 'x');
			else
				_ranges += Range<T>('x', 'x');
		}
		else
			_first_char = 'x';
	}
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_s_esc_0(ScanToken& /*token*/)
{
	if (map_character(*(_scan + 1)) != RE_C_HEX_CHARACTER)
	{
		ScanChar ch = hex_value<T>(*_scan);
		if (*(_scan + 1) != '-')
		{
			if (_neg)
				_ranges -= Range<T>(ch, ch);
			else
				_ranges += Range<T>(ch, ch);
		}
		else
			_first_char = ch;
	}
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_s_esc_00(ScanToken& /*token*/)
{
	ScanChar ch = hex_value2<T>(*(_scan - 1), *_scan);
	if (*(_scan + 1) != '-')
	{
		if (_neg)
			_ranges -= Range<T>(ch, ch);
		else
			_ranges += Range<T>(ch, ch);
	}
	else
		_first_char = ch;
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_s_esc_char(ScanToken& /*token*/)
{
	ScanChar ch;
	if (*_scan == 'n')
		ch = '\n';
	else if (*_scan == 't')
		ch = '\t';
	else if (*_scan == 'r')
		ch = '\r';
	else if (*_scan == 's')
		ch = ' ';
	else if (*_scan == 'v')
		ch = '\v';
	else if (*_scan == 'f')
		ch = '\f';
	else if (*_scan == 'b')
		ch = '\b';
	else if (*_scan == 'a')
		ch = '\a';
	else if (*_scan == '0')
		ch = 0;
	else
		ch = *_scan;

	if (*(_scan + 1) != '-')
	{
		if (_neg)
			_ranges -= Range<T>(ch, ch);
		else
			_ranges += Range<T>(ch, ch);
	}
	else
		_first_char = ch;
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_e_esc_x(ScanToken& /*token*/)
{
	if (map_character(*(_scan + 1)) != RE_C_HEX_CHARACTER)
	{
		if (_neg)
			_ranges -= Range<T>(_first_char, 'x');
		else
			_ranges += Range<T>(_first_char, 'x');
	}
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_e_esc_0(ScanToken& /*token*/)
{
	if (map_character(*(_scan + 1)) != RE_C_HEX_CHARACTER)
	{
		if (_neg)
			_ranges -= Range<T>(_first_char, hex_value<T>(*_scan));
		else
			_ranges += Range<T>(_first_char, hex_value<T>(*_scan));
	}
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_e_esc_00(ScanToken& /*token*/)
{
	if (_neg)
		_ranges -= Range<T>(_first_char, hex_value2<T>(*(_scan - 1), *_scan));
	else
		_ranges += Range<T>(_first_char, hex_value2<T>(*(_scan - 1), *_scan));
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_sr_e_esc_char(ScanToken& /*token*/)
{
	ScanChar ch;
	if (*_scan == 'n')
		ch = '\n';
	else if (*_scan == 't')
		ch = '\t';
	else if (*_scan == 'r')
		ch = '\r';
	else if (*_scan == 's')
		ch = ' ';
	else if (*_scan == 'v')
		ch = '\v';
	else if (*_scan == 'f')
		ch = '\f';
	else if (*_scan == 'b')
		ch = '\b';
	else if (*_scan == 'a')
		ch = '\a';
	else if (*_scan == '0')
		ch = 0;
	else
		ch = *_scan;
	if (_neg)
		_ranges -= Range<T>(_first_char, ch);
	else
		_ranges += Range<T>(_first_char, ch);
	next_char();
	return false;
}

template <class T>
bool ExpScanner<T>::rt_pass(ScanToken& /*token*/)
{
	next_char();
	return false;
}

/* Create a parse context. */
template <class T>
ExpScanner<T>::ExpScanner(const ScanChar* parse_data)
: src(parse_data), _scan(parse_data), _scan_state(RE_S_BEGIN)
{
	pos.line = 1;
	pos.column = 1;
}

template <class T>
ScanTokenType ExpScanner<T>::next_token(ScanToken& token)
{
	if (!_token_buffer.empty())
	{
		pop_token(token);
		return token.type;
	}
	if (_scan_state == RE_S_ERR || _scan_state == RE_S_END)
	{
		token.type = TOKEN_ERR;
		return TOKEN_ERR;
	}
	for (;;)
	{
		/* State transition. */
		_scan_state = state_matrix
			[_scan_state][map_character(*_scan)];
		/* Call routine procedure. */
		REStateRoutine routine = get_state_routines(_scan_state);
		if ((this->*routine)(token))
			return token.type;
	}
}


template class ExpScanner<char>;
template class ExpScanner<unsigned char>;
template class ExpScanner<wchar_t>;

} // End of namespace lex
} // End of namespace tlib

