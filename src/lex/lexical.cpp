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

#include "lexical.h"
#include "regex.h"
#include "dfa.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include "../tlibstr.h"


extern const unsigned char lex_name_bc[];
extern const unsigned int lex_name_bc_length;
extern const unsigned char lex_name_ubc[];
extern const unsigned int lex_name_ubc_length;
extern const unsigned char lex_name_wbc[];
extern const unsigned int lex_name_wbc_length;

namespace tlib
{

namespace lex
{

using namespace std;




template<typename T>
static bool is_key_name(const basic_string<T>& key)
{
	const T* scan = key.c_str();
	if (*scan == 0)
		return false;

	bool is_start = true;
	while (*scan != '\0')
	{
		if (is_start)
		{
			if (!((*scan >= 'a' && *scan <= 'z')
					|| (*scan >= 'A' && *scan <= 'Z')
					|| *scan == '_'))
			{
				return false;
			}
			is_start = false;
		}
		else
		{
			if (!((*scan >= 'a' && *scan <= 'z')
					|| (*scan >= 'A' && *scan <= 'Z')
					|| (*scan >= '0' && *scan <= '9')
					|| *scan == '_'))
			{
				return false;
			}
		}
		scan++;
	}
	return true;
}

/* 	Format key, If success, return the formated key name, minimum match
	and ignore informations by referenced parameters.
	If error then keep the parameters no change, and return false. */
template <typename T>
static bool format_key(basic_string<T>& key, bool& minimum, bool& ignore)
{
	const T* scan = key.c_str();
	int state = 0;
	basic_string<T> out;
	bool min_out = false;
	bool ign_out = false;
	while (*scan != '\0')
	{
		switch (state)
		{
		case 0:
			if (*scan == ' ' || *scan == '\t')
			{
			}
			else if ((*scan >= 'a' && *scan <= 'z') ||
				(*scan >= 'A' && *scan <= 'Z') ||
				(*scan >= '0' && *scan <= '9') || *scan == '_')
			{
				out.push_back(*scan);
				state = 1;
			}
			else if (*scan == '(')
			{
				state = 3;
			}
			else
				return false;
			break;
		case 1:
			if (*scan == '_' || (*scan >= 'a' && *scan <= 'z') ||
				(*scan >= 'A' && *scan <= 'Z') ||
				(*scan >= '0' && *scan <= '9') ||
				*scan == '-' || *scan == '.')
			{
				out.push_back(*scan);
			}
			else if (*scan == ' ' || *scan == '\t')
			{
				state = 2;
			}
			else if (*scan == '(')
			{
				state = 3;
			}
			break;
		case 2:
			if (*scan == ' ' || *scan == '\t')
			{
			}
			else if (*scan == '(')
			{
				state = 3;
			}
			else
				return false;
			break;
		case 3:
			if (*scan == 'm' || *scan == '<')
			{
				min_out = true;
			}
			else if (*scan == 'i' || *scan == '-')
			{
				ign_out = true;
			}
			else if (*scan == ')')
			{
				state = 4;
			}
			else if (*scan == ' ' || *scan == '\t')
			{
			}
			else
				return false;
			break;
		case 4:
			if (*scan == ' ' || *scan == '\t')
			{
			}
			else
				return false;
			break;
		}
		scan++;
	}
	if (state == 0 || state == 1 || state == 2 || state == 4)
	{
		key = out;
		to_lower(key);
		minimum = min_out;
		ignore = ign_out;
		return true;
	}
	else
		return false;
}

#pragma pack(push)
#pragma pack(4)

typedef struct _dfa_transit
{
	bool final_state;
	unsigned int state;
	unsigned int action;
} DfaTransit;

typedef struct _action_info
{
	bool minimum;
	bool ignore;
	unsigned int name_offset;
} ActionInfo;

typedef struct _dfa_data
{
	unsigned int header_size;
	unsigned int data_size;
	unsigned int char_size;
	unsigned int char_map_offset;
	unsigned int char_map_count;
	unsigned int transit_map_offset;
	unsigned int transit_map_state_count;
	unsigned int transit_map_input_count;
	unsigned int action_info_offset;
	unsigned int action_info_count;
	unsigned int action_names_offset;
	unsigned int action_names_size;
} DfaData;


template <typename T>
class Define
{
public:
	basic_string<T> name;
	basic_string<T> value;
	bool minimum;
	bool ignore;
	int line;
};

#pragma pack(pop)


template <typename T>
typename Lexical<T>::LexicalPtr Lexical<T>::create(const std::basic_string<T>& lexical)
		throw(std::runtime_error)
{
	basic_istringstream<T> in(lexical);
	return create_by_stream(in);
}

template <>
Lexical<char>::LexicalPtr Lexical<char>::create_by_file(const std::string& filename)
		throw(std::runtime_error)
{
	std::string str;
	load_text_file_to_string(filename, str);
	return create(str);
}

template <>
Lexical<unsigned char>::LexicalPtr Lexical<unsigned char>::create_by_file(const std::string& filename)
		throw(std::runtime_error)
{
	std::string str;
	load_text_file_to_string(filename, str);
	const unsigned char* tmp = (const unsigned char*)str.c_str();
	return create(tmp);
}

template <>
Lexical<wchar_t>::LexicalPtr Lexical<wchar_t>::create_by_file(const std::string& filename)
		throw(std::runtime_error)
{
	std::wstring str;
	load_text_file_to_wstring(filename, str);
	return create(str);
}


template <typename T> inline
void error_define(Define<T>& def);
template<> inline
void error_define<char>(Define<char>& def)
{
	def.name = "error";
	def.value.clear();
	def.ignore = false;
	def.minimum = false;
	def.line = 0;
}
template<> inline
void error_define<unsigned char>(Define<unsigned char>& def)
{
	def.name = (const unsigned char*)"error";
	def.value.clear();
	def.ignore = false;
	def.minimum = false;
	def.line = 0;
}
template<> inline
void error_define<wchar_t>(Define<wchar_t>& def)
{
	def.name = L"error";
	def.value.clear();
	def.ignore = false;
	def.minimum = false;
	def.line = 0;
}

template <typename T> inline
bool string_compare(const std::basic_string<T>& str1, const char* str2);
template <> inline
bool string_compare<char>(const std::basic_string<char>& str1, const char* str2)
{
	return str1 == str2;
}
template <> inline
bool string_compare<unsigned char>(const std::basic_string<unsigned char>& str1, const char* str2)
{
	return str1 == (const unsigned char*)str2;
}
template <> inline
bool string_compare<wchar_t>(const std::basic_string<wchar_t>& str1, const char* str2)
{
	return wstring_to_locale(str1) == str2;
}


template <typename T> inline
void init_size(unsigned int& char_size, unsigned int& char_map_count);
template <> inline
void init_size<char>(unsigned int& char_size, unsigned int& char_map_count)
{
	char_size = sizeof(unsigned short);
	char_map_count = 256;
}
template <> inline
void init_size<unsigned char>(unsigned int& char_size, unsigned int& char_map_count)
{
	char_size = sizeof(unsigned short);
	char_map_count = 256;
}
template <> inline
void init_size<wchar_t>(unsigned int& char_size, unsigned int& char_map_count)
{
	char_size = sizeof(unsigned short);
	char_map_count = 65536;
}

template <typename T> inline
void init_char_map(DfaData* dfa_data, Dfa<T>& dfa);

template <> inline
void init_char_map<char>(DfaData* dfa_data, Dfa<char>& dfa)
{
	unsigned short* char_array = (unsigned short*)((char*)dfa_data + dfa_data->char_map_offset);
	for (unsigned int i = 0; i < 256; i++)
		char_array[i] = (unsigned short)dfa.range_map.size();
	for (unsigned int i = 0; i < dfa.range_map.size(); i++)
	{
		for (int ch = dfa.range_map[i].min(); ch <= dfa.range_map[i].max(); ++ch)
			char_array[ch + 128] = (unsigned short)i;
	}
}

template <> inline
void init_char_map<unsigned char>(DfaData* dfa_data, Dfa<unsigned char>& dfa)
{
	unsigned short* char_array = (unsigned short*)((char*)dfa_data + dfa_data->char_map_offset);
	for (unsigned int i = 0; i < 256; i++)
		char_array[i] = (unsigned short)dfa.range_map.size();
	for (unsigned int i = 0; i < dfa.range_map.size(); i++)
	{
		for (int ch = dfa.range_map[i].min(); ch <= dfa.range_map[i].max(); ++ch)
			char_array[ch] = (unsigned short)i;
	}
}

template <> inline
void init_char_map<wchar_t>(DfaData* dfa_data, Dfa<wchar_t>& dfa)
{
	unsigned short* char_array = (unsigned short*)((char*)dfa_data + dfa_data->char_map_offset);
	for (unsigned int i = 0; i < 65536; i++)
		char_array[i] = (unsigned short)dfa.range_map.size();
	for (unsigned int i = 0; i < dfa.range_map.size(); i++)
	{
		for (int ch = dfa.range_map[i].min(); ch <= dfa.range_map[i].max(); ++ch)
			char_array[ch] = (unsigned short)i;
	}
}


#ifdef __MSVC__
#pragma warning(disable : 4996)
#endif

template <typename T> inline
void copy_str(T* dest, const T* source);
template <> inline
void copy_str<char>(char* dest, const char* source)
{
	strcpy(dest, source);
}
template <> inline
void copy_str<unsigned char>(unsigned char* dest, const unsigned char* source)
{
	strcpy((char*)dest, (const char*)source);
}
template <> inline
void copy_str<wchar_t>(wchar_t* dest, const wchar_t* source)
{
	wcscpy(dest, source);
}


template <typename T> inline
const std::string to_string(const std::basic_string<T>& str);

template <> inline
const std::string to_string<char>(const std::basic_string<char>& str)
{
	return str;
}

template <> inline
const std::string to_string<unsigned char>(const std::basic_string<unsigned char>& str)
{
	return std::string((const char*)str.c_str());
}

template <> inline
const std::string to_string<wchar_t>(const std::basic_string<wchar_t>& str)
{
	return wstring_to_utf8(str);
}

template <typename T>
class LexRepl
{
public:
	typedef std::vector<Define<T> > Defines;
	inline LexRepl(const Defines& defines)
	: _defines(defines)
	{
	}
	inline void operator () (std::basic_string<T>& s)
	{
		for (size_t i = 0; i < _defines.size(); i++)
		{
			if (s.compare(1, s.size() - 2, _defines[i].name) == 0)
			{
				s = T('(') + _defines[i].value + T(')');
				break;
			}
		}
	}
private:
	const Defines& _defines;
};


template <typename T> inline
const unsigned char* get_regex_bc(unsigned int& len);
template <> inline
const unsigned char* get_regex_bc<char>(unsigned int& len)
{
	len = lex_name_bc_length;
	return lex_name_bc;
}
template <> inline
const unsigned char* get_regex_bc<unsigned char>(unsigned int& len)
{
	len = lex_name_ubc_length;
	return lex_name_ubc;
}
template <> inline
const unsigned char* get_regex_bc<wchar_t>(unsigned int& len)
{
	len = lex_name_wbc_length;
	return lex_name_wbc;
}





template <typename T>
const std::basic_string<T> replace_macro(
		const std::basic_string<T>& source,
		const std::map<std::basic_string<T>, std::basic_string<T> >& repls,
		std::set<std::basic_string<T> > repl_keys)
		throw (std::runtime_error)
{
	if (source.length() == 0)
	{
		return source;
	}
	unsigned int bc_len = 0;
	const unsigned char* bc = get_regex_bc<T>(bc_len);
	Regex<T> exp(bc, bc_len, true);

	std::basic_string<T> result;

	typedef std::map<std::basic_string<T>, std::basic_string<T> > ReplMap;

	std::basic_istringstream<T> ss(source);
	exp._lexical->parse(ss);
	Token<T> token;
	while (exp._lexical->fetch_next(token))
	{
		if (token.action == 1)
		{
			std::basic_string<T> key_name =
					token.str.substr(1, token.str.length() - 2);

			if (repl_keys.find(key_name) != repl_keys.end())
				throw runtime_error("Recursive definition: " + to_string<T>(key_name));

			typename ReplMap::const_iterator it = repls.find(key_name);
			if (it != repls.end())
			{
				repl_keys.insert(key_name);
				result += replace_macro(it->second, repls, repl_keys);
				repl_keys.erase(key_name);
			}
			else
				result += token.str;
		}
		else
			result += token.str;
	}
	return result;
}


template <typename T>
typename Lexical<T>::LexicalPtr Lexical<T>::create_by_stream(std::basic_istream<T>& in)
		throw(std::runtime_error)
{
	typedef std::vector<Define<T> > Defines;
	Defines defines;
	Define<T> define_error;
	error_define<T>(define_error);
	defines.push_back(define_error);

	// Used for replace defined macro token
	typedef std::map<std::basic_string<T>, std::basic_string<T> > Repls;
	Repls repls;

	basic_string<T> line;
	int line_count = 0;
	// Whether is the line is extended line.
	bool is_continue = false;
	while (in.good())
	{
		getline(in, line, (T)'\n');
		line_count++;
		// The line continue to the above line.
		if (_isspace(line[0]))
			is_continue = true;
		else
			is_continue = false;

		trim(line);
		if (line.empty())
			continue;
		if (line[0] == '#')
		{
			if (line.length() > 2 && line[1] == '!')
			{
				// Define replace macro line
				line = line.substr(2);
				size_t find_pos = line.find(':', 0);
				if (find_pos != string::npos)
				{
					basic_string<T> name = line.substr(0, find_pos);
					trim(name);
					if (!is_key_name(name))
						continue;
					basic_string<T> value = line.substr(find_pos + 1);
					trim(value);
					repls[name] = value;
				}
			}
			continue;
		}

		// Follow the previous line.
		if (is_continue)
		{
			if (defines.size() > 1)
			{
				defines.back().value += line;
			}
			else
			{
				stringstream sbuf;
				sbuf << "Syntax error: line " << line_count
						<< ": does not yet have any lines can be extended.";
				throw runtime_error(sbuf.str().c_str());
			}
			continue;
		}

		// Start a new define line.
		size_t find_pos = line.find(':', 0);
		if (find_pos == string::npos)
		{
			stringstream sbuf;
			sbuf << "Syntax error: line " << line_count
					<< ": must in [NAME]<:><VALUE> format";
			throw runtime_error(sbuf.str().c_str());
		}

		basic_string<T> name = line.substr(0, find_pos);
		bool minimum = false;
		bool ignore = false;
		if (!format_key(name, minimum, ignore))
		{
			stringstream sbuf;
			sbuf << "Syntax error: line " << line_count
					<< ": invalid token name.";
			throw runtime_error(sbuf.str().c_str());
		}

		basic_string<T> value = line.substr(find_pos + 1);
		trim(value);
		if (value.empty() && !string_compare(name, "error"))
		{
			stringstream sbuf;
			sbuf << "Syntax error: line " << line_count
					<< ": expression can't be empty.";
			throw runtime_error(sbuf.str().c_str());
		}

		if (!string_compare(name, "error"))
		{
			Define<T> define =
			{ name, value, minimum, ignore, line_count};
			defines.push_back(define);
		}
		else
			defines[0].ignore = ignore;
	}


	//LexRepl<T> repl(defines);
	// Create expression tree
	typename Exp<T>::ExpPtr exp;
	for (size_t i = 1; i < defines.size(); i++)
	{
		typename Exp<T>::ExpPtr tmp_exp;
		try
		{
			// asm("INT $3");
			// Replace {name} with a real regex.
			std::set<basic_string<T> > repl_keys;
			defines[i].value = replace_macro<T>(defines[i].value, repls, repl_keys);

			tmp_exp = exp_parse(defines[i].value.c_str());
		}
		catch (const std::exception& err)
		{
			stringstream sbuf;
			sbuf << "Syntax error: line " << defines[i].line << ": " << err.what();
			throw runtime_error(sbuf.str().c_str());
		}
		tmp_exp->action = (Action)i;
		if (exp)
			exp = exp_join<T>(exp, tmp_exp, RE_JOIN_OR);
		else
			exp = tmp_exp;
	}

	// Create DFA
	Dfa<T> dfa;
	if (exp)
	{
		make_dfa(exp, dfa);
	}
	else
		throw runtime_error("No define any words.");

	DfaData header;
	header.header_size = sizeof(DfaData);

	header.char_map_offset = header.header_size;
	init_size<T>(header.char_size, header.char_map_count);

	header.transit_map_offset = header.char_map_offset
		+ header.char_size * header.char_map_count;
	header.transit_map_state_count = dfa.total_states;
	header.transit_map_input_count = (unsigned int)dfa.range_map.size() + 1;

	header.action_info_offset = header.transit_map_offset
		+ header.transit_map_state_count * header.transit_map_input_count
		* sizeof(DfaTransit);
	header.action_info_count = (unsigned int)defines.size();

	header.action_names_offset = header.action_info_offset
		+ header.action_info_count * sizeof(ActionInfo);
	header.action_names_size = 0;
	for (unsigned int i = 0; i < defines.size(); i++)
	{
		header.action_names_size += (unsigned int)((defines[i].name.length() + 1) * sizeof(T));
	}
	header.data_size = header.action_names_offset + header.action_names_size;

	LexicalPtr lexical(new Lexical());
	lexical->_data = malloc(header.data_size);
	if (!lexical->_data)
	{
		throw runtime_error("Out of memory.");
	}
	lexical->_allocated = true;
	DfaData* dfa_data = (DfaData*)lexical->_data;
	memcpy(dfa_data, &header, sizeof(DfaData));

	// Initialize char map.
	init_char_map(dfa_data, dfa);

	// Initialize transit map
	DfaTransit *transit = (DfaTransit*)((char*)dfa_data + dfa_data->transit_map_offset);
	for (unsigned int i = 0; i < dfa.total_states; i++)
	{
		for (unsigned int j = 0; j < dfa_data->transit_map_input_count; j++)
		{
			unsigned int idx = i * dfa_data->transit_map_input_count + j;
			transit[idx].final_state = false;
			transit[idx].state = dfa.total_states;
			transit[idx].action = 0;
		}
	}
	for (unsigned int i = 0; i < dfa.transits.size(); i++)
	{
		unsigned int idx = dfa.transits[i].from * dfa_data->transit_map_input_count
				+ dfa.transits[i].input;

		transit[idx].state = dfa.transits[i].to;
		transit[idx].final_state = false;
		transit[idx].action = 0;
		for (unsigned int j = 0; j < dfa.accepting_states.size(); j++)
		{
			if (transit[idx].state == dfa.accepting_states[j])
			{
				transit[idx].final_state = true;
				transit[idx].action = dfa.transits[i].action;
				break;
			}
		}
	}

	ActionInfo* action_info = (ActionInfo*)((char*)dfa_data + dfa_data->action_info_offset);

	unsigned int name_offset = dfa_data->action_names_offset;
	for (unsigned int i = 0; i < defines.size(); i++)
	{
		action_info[i].ignore = defines[i].ignore;
		action_info[i].minimum = defines[i].minimum;
		action_info[i].name_offset = name_offset;

		copy_str((T*)((char*)dfa_data + action_info[i].name_offset),
			defines[i].name.c_str());

		name_offset += (unsigned int)((defines[i].name.length() + 1) * sizeof(T));
	}
	return lexical;
}

template <typename T>
bool valid_dfa_data(const DfaData* data)
{
	if (!data)
		return false;
	if (data->header_size != sizeof(DfaData))
		return false;
	if (data->data_size < data->header_size)
		return false;

	unsigned int char_size = 0;
	unsigned int char_map_count = 0;
	init_size<T>(char_size, char_map_count);
	if (data->char_size != char_size ||
		data->char_map_count != char_map_count)
		return false;

	if (data->data_size != data->header_size
		+ data->char_map_count * data->char_size
		+ data->transit_map_state_count * data->transit_map_input_count * sizeof(DfaTransit)
		+ data->action_info_count * sizeof(ActionInfo)
		+ data->action_names_size)
		return false;

	return true;
}

template <typename T>
typename Lexical<T>::LexicalPtr Lexical<T>::create_by_bc_file(const std::string& filename)
		throw(std::runtime_error)
{
	ifstream infile(filename.c_str(), ios::in | ios::binary);
	if (!infile.is_open())
		throw runtime_error("Can't open file.");

	DfaData data;
	infile.read((char*)&data, sizeof(DfaData));
	if (infile.gcount() < (int)sizeof(DfaData))
	{
		infile.close();
		throw runtime_error("Invalid file format.");
	}

	if (!valid_dfa_data<T>(&data))
	{
		infile.close();
		throw runtime_error("Invalid file format.");
	}

	LexicalPtr lexical(new Lexical());
	lexical->_data = malloc(data.data_size);
	if (!lexical->_data)
	{
		throw runtime_error("Out of memory.");
	}
	lexical->_allocated = true;
	memcpy(lexical->_data, &data, data.header_size);

	infile.read((char*)lexical->_data + data.header_size, data.data_size - data.header_size);
	if (infile.gcount() < (int)(data.data_size - data.header_size))
	{
		infile.close();
		throw runtime_error("Invalid file format.");
	}

	return lexical;
}


template <typename T>
typename Lexical<T>::LexicalPtr Lexical<T>::create_by_bc(const void* data, unsigned long data_len)
		throw(std::runtime_error)
{
	if (data_len < sizeof(DfaData))
		throw runtime_error("Invalid data format.");

	DfaData* dfa_data = (DfaData*)data;
	if (!valid_dfa_data<T>(dfa_data))
		throw runtime_error("Invalid data format.");

	if (data_len < dfa_data->data_size)
		throw runtime_error("Invalid data format.");

	LexicalPtr lexical(new Lexical());
	lexical->_data = malloc(dfa_data->data_size);
	if (!lexical->_data)
	{
		throw runtime_error("Out of memory.");
	}
	lexical->_allocated = true;
	memcpy(lexical->_data, data, dfa_data->data_size);
	return lexical;
}
template <typename T>
typename Lexical<T>::LexicalPtr Lexical<T>::create_by_static_bc(const void* data, unsigned long data_len)
		throw(std::runtime_error)
{
	if (data_len < sizeof(DfaData))
		throw runtime_error("Invalid data format.");

	DfaData* dfa_data = (DfaData*)data;
	if (!valid_dfa_data<T>(dfa_data))
		throw runtime_error("Invalid data format.");

	if (data_len < dfa_data->data_size)
		throw runtime_error("Invalid data format.");

	LexicalPtr lexical(new Lexical());
	lexical->_data = (void*)data;
	lexical->_allocated = false;
	return lexical;
}

template <typename T>
void Lexical<T>::save_bc(const std::string& filename) throw(std::runtime_error)
{
	if (!_data)
		throw std::runtime_error("Not initialized.");
	ofstream outfile(filename.c_str(), ios::out | ios::binary);
	if (!outfile.good())
		throw runtime_error("Can't open file.");

	DfaData* dfa_data = (DfaData*)_data;
	outfile.write((char*)_data, dfa_data->data_size);
	if (!outfile.good())
	{
		outfile.close();
		throw runtime_error("Write file error.");
	}
	outfile.close();
}


template <typename T>
Lexical<T>::Lexical()
: best_match(false), _data(0), _allocated(false)
{
	reset();
}

template <typename T>
Lexical<T>::~Lexical()
{
	if (_data && _allocated)
		free(_data);
}

template <typename T>
void Lexical<T>::parse(std::basic_istream<T>& input_stream) throw(std::runtime_error)
{
	_env.in = &input_stream;
	if (!_env.in)
		throw std::runtime_error("Invalid parameters.");
	_env.begin = _env.in->tellg();
	reset();
}

template <typename T>
void Lexical<T>::parse(const T* input_string) throw(std::runtime_error)
{
	std::basic_istringstream<T> iss(input_string);
	parse(iss);
}

template <typename T>
void Lexical<T>::parse(const T* input_buffer, size_t buffer_len) throw(std::runtime_error)
{
	std::basic_istringstream<T> iss(std::basic_string<T>(input_buffer, buffer_len));
	parse(iss);
}


template <typename T>
void Lexical<T>::reset()
{
	if (_env.in)
		_env.in->seekg(_env.begin, ios::beg);
	_env.line = 1;
	_env.line_pos = 1;
	_env.cur = _env.begin;
	reset_state();
}


template <typename T>
inline T next_char(LexicalEnv<T>& env)
{
	T ch = 0;
	env.in->get(ch);
	env.cur++;
	if (ch == '\n')
	{
		env.line++;
		env.line_pos = 1;
	}
	else
		env.line_pos++;
	return ch;
}

template <typename T> inline
unsigned short map_char(unsigned short* char_map, T ch);
template <> inline
unsigned short map_char<char>(unsigned short* char_map, char ch)
{
	return char_map[((int)ch) + 128];
}
template <> inline
unsigned short map_char<unsigned char>(unsigned short* char_map, unsigned char ch)
{
	return char_map[(int)ch];
}
template <> inline
unsigned short map_char<wchar_t>(unsigned short* char_map, wchar_t ch)
{
	return char_map[(int)ch];
}


template <typename T>
void Lexical<T>::next(Token<T>& token) throw(std::runtime_error)
{
	if (!_env.in)
		throw std::runtime_error("No stream.");

	if (!_data)
		throw std::runtime_error("Not initialized.");

	DfaData* data = (DfaData*)_data;
	unsigned short* char_map = (unsigned short*)((char*)_data + data->char_map_offset);
	DfaTransit* transit_map = (DfaTransit*)((char*)_data + data->transit_map_offset);
	ActionInfo* action_info = (ActionInfo*)((char*)_data + data->action_info_offset);

	token.action = 0;
	token.length = 0;
	token.line = _env.line;
	token.line_pos = _env.line_pos;
	token.pos = _env.cur;
	token.str.clear();

	unsigned int previous_final_line = _env.line;
	unsigned int previous_final_line_pos = _env.line_pos;

	const DfaTransit* transit = 0;
	T ch;
	while ((ch = next_char(_env)) != 0)
	{
		unsigned short input = map_char(char_map, ch);
		transit = &transit_map[_env.state * data->transit_map_input_count + input];
		token.str.push_back(ch);

		if (transit->final_state)
		{
			// token.str.push_back(ch);
			token.length = (unsigned int)token.str.length();
			token.action = (Action)transit->action;
			if (action_info[token.action].minimum)
			{
				token.name = (T*)((char*)_data + action_info[token.action].name_offset);
				reset_state();
				return;
			}
			else
				_env.state = transit->state;

			previous_final_line = _env.line;
			previous_final_line_pos = _env.line_pos;
		}
		else
		{
			if (transit->state < data->transit_map_state_count)
			{
				// token.str.push_back(ch);
				_env.state = transit->state;
				if (token.str.length() == 1)
				{
					previous_final_line = _env.line;
					previous_final_line_pos = _env.line_pos;
				}
			}
			else
			{
				if (token.action == 0)
				{
					if (best_match)
					{
						token.length = 1;
						_env.cur = token.pos + token.length;
						token.str.resize(token.length);
						_env.line = previous_final_line;
						_env.line_pos = previous_final_line_pos;
						_env.in->clear();
						_env.in->seekg(_env.cur, ios::beg);
					}
					else
					{
						//token.str.push_back(ch);
						token.length++;
					}
				}
				else
				{
					_env.cur = token.pos + token.length;
					token.str.resize(token.length);
					_env.line = previous_final_line;
					_env.line_pos = previous_final_line_pos;
					_env.in->clear();
					_env.in->seekg(_env.cur, ios::beg);
				}
				token.name = (T*)((char*)_data + action_info[token.action].name_offset);
				reset_state();
				return;
			}
		}
	}
	if (token.action == 0)
	{
		if (best_match && token.str.length() > 1)
		{
			token.length = 1;
			_env.cur = token.pos + token.length;
			token.str.resize(token.length);
			_env.line = previous_final_line;
			_env.line_pos = previous_final_line_pos;
			_env.in->clear();
			_env.in->seekg(_env.cur, ios::beg);
		}
		else
		{
			token.length = (unsigned int)token.str.length();
		}
	}
	else
	{
		_env.cur = token.pos + token.length;
		token.str.resize(token.length);
		_env.line = previous_final_line;
		_env.line_pos = previous_final_line_pos;
		_env.in->clear();
		_env.in->seekg(_env.cur, ios::beg);
	}
	token.name = (T*)((char*)_data + action_info[token.action].name_offset);
	reset_state();
	return;
}

template <typename T>
Action Lexical<T>::next_token(Token<T>& token) throw(std::runtime_error)
{
	if (!_data)
		throw std::runtime_error("Not initialized.");
	DfaData* data = (DfaData*)_data;
	ActionInfo* action_info = (ActionInfo*)((char*)_data + data->action_info_offset);
	do
	{
		next(token);
	}
	while (action_info[token.action].ignore && token.length > 0);
	return token.action;
}

template <typename T>
bool Lexical<T>::fetch_next(Token<T>& token) throw(std::runtime_error)
{
	if (next_token(token) == 0 && token.length == 0)
		return false;
	else
		return true;

}

template <typename T>
Action Lexical<T>::get_named_action_id(const std::basic_string<T>& token_name) const
{
	DfaData* dfa_data = (DfaData*)_data;
	ActionInfo* action_info = (ActionInfo*)((char*)dfa_data + dfa_data->action_info_offset);
	for (size_t i = 0; i < dfa_data->action_info_count; i++)
	{
		if (token_name.compare((T*)((char*)dfa_data + action_info[i].name_offset)) == 0)
			return i;
	}
	return -1;
}


template class Lexical<char>;
template class Lexical<unsigned char>;
template class Lexical<wchar_t>;


} // End of namespace lex
} // End of namespace tlib


