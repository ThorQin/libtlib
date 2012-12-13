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

#include <stack>
#include <set>
#include "sax.h"
#include "../lex/regex.h"
#include "../tlibdata.h"

extern const unsigned char sax_bc[];
extern const unsigned int sax_bc_length;

namespace tlib
{

wchar_t parse_hex(const char* str, size_t len);
wchar_t parse_dec(const char* str, size_t len);

namespace xml
{


SaxParserHandler::~SaxParserHandler()
{
}





SaxParser::SaxParser(SaxParserHandler* handler)
: _handler(handler), _substitute_entity(true), _charset("utf-8")
{
}

SaxParser::~SaxParser()
{
}


template <>
bool SaxParser::parse<char>(const std::basic_string<char>& src)
{
	std::istringstream iss(src);
	return parse(iss, "utf-8", true, false);
}

template <>
bool SaxParser::parse<wchar_t>(const std::basic_string<wchar_t>& src)
{
	// internal use utf8 charset to parser.
	std::istringstream iss(wstring_to_utf8(src));
	return parse(iss, "utf-8", false, true);
}

bool SaxParser::parse_file(const std::string& file)
{
	std::ifstream infile(file.c_str(), std::ios::in | std::ios::binary);
	if (!infile.good())
	{
		_error = "Open file failed.";
		return false;
	}
	std::filebuf* buf = infile.rdbuf();
	char buffer[4096];
	buf->pubsetbuf(buffer, 4096);
	char bom[3];
	infile.read(bom, 3);
	bool result = false;
	std::locale loc("");

	if (infile.gcount() == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
	{
		// utf8 BOM, so skip BOM and parse directly.
		result = parse(infile, "utf-8", true, true);
	}
	else if (infile.gcount() == 2 && bom[0] == 0xFF && bom[1] == 0xFE)
	{
		// utf16-le BOM
		std::locale utf16_loc(loc, new codecvt_char_utf16_le);
		infile.imbue(utf16_loc);
		result = parse(infile, "utf-8", false, true);
	}
	else if (infile.gcount() == 2 && bom[0] == 0xFE && bom[1] == 0xFF)
	{
		// utf16-be BOM
		std::locale utf16_loc(loc, new codecvt_char_utf16_be);
		infile.imbue(utf16_loc);
		result = parse(infile, "utf-8", false, true);
	}
	else
	{
		// No BOM, so use UTF8 as default charset and
		// will detect real charset via 'encoding' indicator.
		infile.seekg(0);
		result = parse(infile, "utf-8", true, false);
	}
	infile.close();
	return result;
}


typedef enum _sax_state
{
	_x_begin,
	_x_elem,
	_x_elem_name,
	_x_attr_name,
	_x_equal,
	_x_text,
	_x_elem_end,
	_x_elem_end_name,
	_x_end
} SaxState;

// Depend on 'gen/sax.lex' file
typedef enum _sax_token
{
	_t_start_beg = 1,
	_t_start_end,
	_t_close_beg,
	_t_close_end,
	_t_equal,
	_t_slash,
	_t_pi_xml,
	_t_pi,
	_t_comment,
	_t_cdata,
	_t_str,
	_t_name,
	_t_entity_lt,
	_t_entity_gt,
	_t_entity_amp,
	_t_entity_apos,
	_t_entity_quot,
	_t_entity_hex,
	_t_entity_dec,
	_t_entity,
	_t_space,
	_t_text
} SaxToken;



static const char* _err_invalid = "Invalid token in current context.";
static const char* _err_attr_dup = "Attribute duplicate.";
static const char* _err_not_match = "Element not match.";
static const char* _err_bad_char = "Invalid character.";
static const char* _err_encoding = "Invalid encoding.";
static const char* _err_not_finish = "Unexpected end of the document.";

#define RETURN_ERROR(x) { \
	std::stringstream ss; \
	ss << x << "(" << token.line << ":" << token.line_pos << ")"; \
	_error = ss.str(); return false; }

#define FIRE(x) { if (_handler) _handler->x; }
#define FIRE_TEXT {\
		trim(text); if (!text.empty()) { FIRE(on_text(text)); text.clear(); } }


extern const unsigned char node_name_bc[];
extern const unsigned int node_name_bc_length;

#define FIRE_PI { \
		std::wstring temp; \
		charset_to_wstring(token.str.substr(2, token.length - 4), _charset, temp); \
		std::wstring target; \
		regex_find(temp, lex::Regex<wchar_t>(node_name_bc, node_name_bc_length, true), target); \
		FIRE(on_processing_instruction(target, trim_copy(temp.substr(target.length())))); }

#define FIRE_COMMENT { \
		std::wstring temp; \
		charset_to_wstring(token.str.substr(4, token.length - 7), _charset, temp); \
		FIRE(on_comment(temp)); }

#define FIRE_CDATA { \
		std::wstring temp; \
		charset_to_wstring(token.str.substr(9, token.length - 12), _charset, temp); \
		FIRE(on_cdata(temp)); }


extern const unsigned char node_entry_bc[];
extern const unsigned int node_entry_bc_length;

extern const unsigned char encoding_bc[];
extern const unsigned int encoding_bc_length;


// Format string in attribute
static void attr_repl(std::string& src)
{
	if (src.compare(0, 2, "&l") == 0) // &lt;
		src = "<";
	else if (src.compare(0, 2, "&g") == 0)
		src = ">";
	else if (src.compare(0, 3, "&am") == 0)
		src = "&";
	else if (src.compare(0, 3, "&ap") == 0)
		src = "'";
	else if (src.compare(0, 2, "&q") == 0)
		src = "\"";
	else if (src.compare(0, 3, "&#x") == 0)
		src = parse_hex(src.c_str() + 3, src.length() - 4);
	else if (src.compare(0, 2, "&#") == 0)
		src = parse_dec(src.c_str() + 2, src.length() - 3);
}

static void parse_attribute(const char* src, size_t len,
		const std::string& charset, std::wstring& attr)
{
	lex::Regex<char> attr_regex(node_entry_bc, node_entry_bc_length, true);
	std::string result = lex::regex_replace<char>(
			std::string(src + 1, len - 2), attr_regex, attr_repl);
	charset_to_wstring(result, charset, attr);
}

bool SaxParser::parse(std::istream& ins,
		const std::string& charset, bool mbcs, bool charset_confirmed)
{
	_charset = charset;
	_error.clear();
	SaxState state = _x_begin;

	std::stack<std::wstring> path;
	std::set<std::wstring> attr_keys;
	std::vector<SaxAttribute> attributes;
	std::wstring attribute;
	std::wstring text;
	try
	{
		lex::Lexical<char>::LexicalPtr lex =
				lex::Lexical<char>::create_by_static_bc(::sax_bc, ::sax_bc_length);
		lex->parse(ins);
		lex->best_match = true;
		FIRE(on_start_document());
		lex::Token<char> token;
		while (lex->fetch_next(token))
		{
			if (token.action == 0)
			{
				RETURN_ERROR(_err_bad_char);
			}
			if (state == _x_begin)
			{
				if (token.action == _t_start_beg)
				{
					state = _x_elem;
				}
				else if (token.action == _t_pi_xml)
				{
					// parse xml version and encoding
					std::string xml_pi = token.str.substr(5, token.length - 7);
					std::string encoding;
					lex::Regex<char> encoding_regex(
							encoding_bc, encoding_bc_length, true);
					size_t fnd = lex::regex_find<char>(
							xml_pi, encoding_regex, encoding);
					if (fnd != xml_pi.npos)
					{
						lex::Regex<char> string_regex("\"[^\"]*\"|'[^']*'");
						lex::regex_find<char>(encoding, string_regex, encoding);
						encoding = encoding.substr(1, encoding.length() - 2);
						unsigned int cp = convert_charset_to_codepage(encoding.c_str());
						if (cp == 0)
						{
							RETURN_ERROR(_err_encoding);
						}
						if (cp == CODEPAGE_UTF16LE || cp == CODEPAGE_UTF16BE)
						{
							if (mbcs)
							{
								RETURN_ERROR(_err_encoding);
							}
							cp = CODEPAGE_UTF8;
							encoding = "utf-8";
						}
						if (convert_charset_to_codepage(_charset.c_str()) != cp)
						{
							if (charset_confirmed)
							{
								RETURN_ERROR(_err_encoding);
							}
							else
								_charset = encoding;
						}
					}

					std::wstring temp;
					charset_to_wstring(xml_pi, _charset, temp);
					FIRE(on_processing_instruction(L"xml", temp));
				}
				else if (token.action == _t_pi)
				{
					FIRE_PI;
				}
				else if (token.action == _t_comment)
				{
					FIRE_COMMENT;
				}
				else if (token.action == _t_space)
				{
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
			else if (state == _x_elem)
			{
				if (token.action == _t_name)
				{
					std::wstring name;
					charset_to_wstring(token.str, _charset, name);
					path.push(name);
					state = _x_elem_name;
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
			else if (state == _x_elem_name)
			{
				if (token.action == _t_start_end)
				{
					FIRE(on_start_element(path.top(), attributes));
					attr_keys.clear();
					attributes.clear();
					state = _x_text;
				}
				else if (token.action == _t_close_end)
				{
					FIRE(on_element(path.top(), attributes));
					attr_keys.clear();
					attributes.clear();
					path.pop();
					if (path.empty())
						state = _x_end;
					else
						state = _x_text;
				}
				else if (token.action == _t_name)
				{
					// start a attribute.
					charset_to_wstring(token.str, _charset, attribute);
					if (attr_keys.find(attribute) != attr_keys.end())
					{
						RETURN_ERROR(_err_attr_dup);
					}
					state = _x_attr_name;
				}
				else if (token.action == _t_space)
				{
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
			else if (state == _x_attr_name)
			{
				if (token.action == _t_equal)
				{
					state = _x_equal;
				}
				else if (token.action == _t_space)
				{
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
			else if (state == _x_equal)
			{
				if (token.action == _t_str)
				{
					std::wstring value;
					parse_attribute(token.str.c_str(), token.length, _charset, value);
					attr_keys.insert(attribute);
					attributes.push_back(SaxAttribute(attribute, value));
					state = _x_elem_name;
				}
				else if (token.action == _t_space)
				{
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
			else if (state == _x_text)
			{
				if (token.action == _t_start_beg)
				{
					FIRE_TEXT;
					state = _x_elem;
				}
				else if (token.action == _t_close_beg)
				{
					FIRE_TEXT;
					state = _x_elem_end;
				}
				else if (token.action == _t_pi_xml)
				{
					FIRE_TEXT;
					RETURN_ERROR(_err_invalid);
				}
				else if (token.action == _t_pi)
				{
					FIRE_TEXT;
					FIRE_PI;
				}
				else if (token.action == _t_comment)
				{
					FIRE_TEXT;
					FIRE_COMMENT;
				}
				else if (token.action == _t_cdata)
				{
					FIRE_TEXT;
					FIRE_CDATA;
				}
				else if (token.action == _t_space || token.action == _t_text
						 || token.action == _t_name || token.action == _t_equal
						 || token.action == _t_slash || token.action == _t_start_end)
				{
					std::wstring temp_text;
					charset_to_wstring(token.str, _charset, temp_text);
					text += temp_text;
				}
				else if (token.action == _t_entity_lt)
				{
					if (_substitute_entity)
						text.push_back(L'<');
					else
					{
						FIRE_TEXT;
						FIRE(on_entity(L"&lt;"));
					}
				}
				else if (token.action == _t_entity_gt)
				{
					if (_substitute_entity)
						text.push_back(L'>');
					else
					{
						FIRE_TEXT;
						FIRE(on_entity(L"&gt;"));
					}
				}
				else if (token.action == _t_entity_amp)
				{
					if (_substitute_entity)
						text.push_back(L'&');
					else
					{
						FIRE_TEXT;
						FIRE(on_entity(L"&amp;"));
					}
				}
				else if (token.action == _t_entity_apos)
				{
					if (_substitute_entity)
						text.push_back(L'\'');
					else
					{
						FIRE_TEXT;
						FIRE(on_entity(L"&apos;"));
					}
				}
				else if (token.action == _t_entity_quot)
				{
					if (_substitute_entity)
						text.push_back(L'"');
					else
					{
						FIRE_TEXT;
						FIRE(on_entity(L"&quot;"));
					}
				}
				else if (token.action == _t_entity_hex)
				{
					if (_substitute_entity)
					{
						// &#x1234;
						wchar_t wch = parse_hex(token.str.c_str() + 3, token.str.length() - 4);
						text.push_back(wch);
					}
					else
					{
						FIRE_TEXT;
						FIRE(on_entity(charset_to_wstring(token.str, _charset)));
					}
				}
				else if (token.action == _t_entity_dec)
				{
					if (_substitute_entity)
					{
						wchar_t wch = parse_dec(token.str.c_str() + 3, token.str.length() - 4);
						text.push_back(wch);
					}
					else
					{
						FIRE_TEXT;
						FIRE(on_entity(charset_to_wstring(token.str, _charset)));
					}
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
			else if (state == _x_elem_end)
			{
				if (token.action == _t_name)
				{
					std::wstring name;
					charset_to_wstring(token.str, _charset, name);
					if (name != path.top())
					{
						RETURN_ERROR(_err_not_match);
					}
					state = _x_elem_end_name;
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
			else if (state == _x_elem_end_name)
			{
				if (token.action == _t_start_end)
				{
					FIRE(on_end_element(path.top()));
					path.pop();
					if (path.empty())
						state = _x_end;
					else
						state = _x_text;
				}
				else if (token.action == _t_space)
				{
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
			else if (state == _x_end)
			{
				if (token.action == _t_space)
				{
				}
				else if (token.action == _t_pi)
				{
					FIRE_PI;
				}
				else if (token.action == _t_comment)
				{
					FIRE_COMMENT;
				}
				else
				{
					RETURN_ERROR(_err_invalid);
				}
			}
		}

		if (state != _x_end)
		{
			_error = _err_not_finish;
			return false;
		}
		FIRE(on_end_document());
	}
	catch (const std::exception& e)
	{
		if (e.what())
			_error = e.what();
		return false;
	}
	return true;
}


void SaxParser::set_handler(SaxParserHandler* handler)
{
	_handler = handler;
}



}
}
