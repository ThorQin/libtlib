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




/************************************************************************
* Lexical definition file:

1) File use "name: value" pair store lexical definitions,
   name is optional, can be empty,
   but separator ':' can not omission.

2) The valid name string format is: [_a-zA-Z][_\.\-a-zA-Z0-9]*

3) Name part can have option specifier enclosed with '(' and ')',
   option 'i' : indicate the parser should ignore the token;
   option 'm' : indicate the parser should use minimum match
   principle.

4) The line that start with '#' is the comment line,
   will be ignored by parser.

5) The line that start with "#!" is the preprocessor directive line.
   Syntax:
   	   #! <name>: <value>

   Definitions can be nested. For example:
       #! space: [ \r\n\t]
       #! spaces: ({space}+)

   Note:
       Very similar with c language, the preprocessor directive
       have the same side effect. So, sometimes developer need use
       preprocessor directive combine with "()".

6) The value part is a regular expression.

7) The name "error" is a internal key word, it can only use to
   indicate how to process errors: ignore or return error token
   to caller, "error" line can not have any expression.

Example file:

	# this is a comment
	#! space: 			[ \r\n\t]+
	error(i) :
	comment(i) : 		//[^\n]*
	some_name(mi) : 	[_a-zA-Z][_a-zA-Z0-9]*
	(i) : 				[ \t\r\n\v\f]*
	: 					"([^\"\n]|\\")*"
	: 					[0-9]+
	use_the_def:		{space}

*************************************************************************
* Example code: parse a file and print every token.
*************************************************************************

#include "tlib.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace tlib;

int main()
{
	try
	{
		lex::Lexical<char>::LexicalPtr lexical = lex::Lexical::create_by_file("test.lex");
		ifstream infile("test.cpp", ios::in);
		filebuf* buf = infile.rdbuf();
		char buffer[4096];
		buf->pubsetbuf(buffer, 4096);
		// lexical->best_match = false; // default is FALSE
		lexical->parse(infile);
		lex::Token<char> token;
		// while (lexical->next_token(token))
		while (lexical->fetch_next(token)) // use fetch_next to handle the EOF token.
		{
			cout << token.name << ": " << token.str << endl;
		}
		infile.close();
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	return 0;
}

*************************************************************************
* End of example.
*************************************************************************/


#ifndef LEXICAL_H_
#define LEXICAL_H_

#include "../binary.h"
#include "exp.h"
#include <istream>

namespace tlib
{
namespace syn
{
template <typename T>
class Syntax;
}
namespace lex
{

template <typename T>
class Token
{
public:
	inline Token();
	inline Token(const Token& other);
	Action action;
	unsigned int pos;
	unsigned int length;
	unsigned int line;
	unsigned int line_pos;
	std::basic_string<T> name;
	std::basic_string<T> str;
};

template <typename T> inline
Token<T>::Token()
: action(0), pos(0), length(0), line(0), line_pos(0)
{
}

template <typename T> inline
Token<T>::Token(const Token& other)
: action(other.action), pos(other.pos), length(other.length),
	line(other.line), line_pos(other.line_pos)
{
}

template <typename T>
class LexicalEnv
{
public:
	inline LexicalEnv();
	unsigned int state;
	unsigned int begin;
	unsigned int cur;
	unsigned int line;
	unsigned int line_pos;
	std::basic_istream<T>* in;
};

template <typename T> inline
LexicalEnv<T>::LexicalEnv()
: state(0), begin(0), cur(0), line(1), line_pos(1), in(0)
{
}




template <typename T>
class Lexical
{
public:
	friend class syn::Syntax<T>;
	typedef std::shared_ptr<Lexical> LexicalPtr;
private:
	explicit Lexical();
public:
	~Lexical();
	static LexicalPtr create(const std::basic_string<T>& lexical)
			throw(std::runtime_error);
	static LexicalPtr create_by_file(const std::string& filename)
			throw(std::runtime_error);
	static LexicalPtr create_by_bc(const void* data, unsigned long data_len)
			throw(std::runtime_error);
	static LexicalPtr create_by_static_bc(const void* data, unsigned long data_len)
			throw(std::runtime_error);
	static LexicalPtr create_by_bc_file(const std::string& filename)
			throw(std::runtime_error);
	static LexicalPtr create_by_stream(std::basic_istream<T>& in)
			throw(std::runtime_error);

	void save_bc(const std::string& filename) throw(std::runtime_error);

	void parse(const T* input_string) throw(std::runtime_error);
	void parse(const T* input_buffer, size_t buffer_len) throw(std::runtime_error);
	void parse(std::basic_istream<T>& input_stream) throw(std::runtime_error);

	/* Reset scan state and stream input pointer. */
	void reset();
	/* Get the next token. */
	Action next_token(Token<T>& token) throw(std::runtime_error);
	/* Get the next token, return 'false' when EOF, otherwise return
	 * 'true' whatever the action id existed or not. */
	bool fetch_next(Token<T>& token) throw(std::runtime_error);
	Action get_named_action_id(const std::basic_string<T>& token_name) const;
	bool best_match;
private:
	inline void reset_state();
	void next(Token<T>& token) throw(std::runtime_error);
private:
	LexicalEnv<T> _env;
	void* _data;
	bool _allocated;
};

template <typename T> inline
void Lexical<T>::reset_state()
{
	_env.state = 0;
}




} // End of namespace lex
} // End of namespace tlib

#endif // LEXICAL_H_


