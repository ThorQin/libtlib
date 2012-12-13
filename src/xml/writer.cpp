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

#include "../lex/regex.h"
#include "../tlibdata.h"
#include "writer.h"

namespace tlib
{
namespace xml
{


XmlWriter::XmlWriter(std::ostream& out, const std::string& charset, bool format)
: _out(out), _charset(charset), _charset_ext(charset),
  _format(format), _level(), _text(),
  _utf16le_loc(std::locale(""), new tlib::codecvt_char_utf16_le),
  _utf16be_loc(std::locale(""), new tlib::codecvt_char_utf16_be)
{
	if (_charset.empty())
	{
		_charset = "utf-8";
		_charset_ext = _charset;
	}
	unsigned int cp = convert_charset_to_codepage(_charset.c_str());
	if (cp == 0 || cp == 12001 || cp == 12000)
		throw std::runtime_error("Unsupported charset");

	if (cp == CODEPAGE_UTF16LE)
	{
		_charset = "utf-8";
		_out.imbue(_utf16le_loc);
	}
	else if (cp == CODEPAGE_UTF16BE)
	{
		_charset = "utf-8";
		_out.imbue(_utf16be_loc);
	}
}
XmlWriter::XmlWriter(std::ofstream& out, const std::string& charset, bool format)
: _out(out), _charset(charset), _charset_ext(charset),
  _format(format), _level(), _text(),
  _utf16le_loc(std::locale(""), new tlib::codecvt_char_utf16_le),
  _utf16be_loc(std::locale(""), new tlib::codecvt_char_utf16_be)
{
	if (_charset.empty())
	{
		_charset = "utf-8";
		_charset_ext = _charset;
	}
	unsigned int cp = convert_charset_to_codepage(_charset.c_str());
	if (cp == 0 || cp == 12001 || cp == 12000)
		throw std::runtime_error("Unsupported charset");

	if (cp == CODEPAGE_UTF16LE)
	{
		_charset = "utf-8";
		_out.imbue(_utf16le_loc);
		_out << "\xEF\xBB\xBF";
	}
	else if (cp == CODEPAGE_UTF16BE)
	{
		_charset = "utf-8";
		_out.imbue(_utf16be_loc);
		_out << "\xEF\xBB\xBF";
	}
	else if (cp == CODEPAGE_UTF8)
	{
		_out << "\xEF\xBB\xBF";
	}
}

static inline std::ostream& indent_out(std::ostream& out, const std::stack<bool>& level)
{
	static const char* double_space = "  ";
	for (size_t i = 0; i < level.size(); i++)
		out << double_space;
	return out;
}


#define IDOUT (_format ? indent_out(_out, _level) : _out)
#define NEW_LINE (_format ? "\n" : "")
#define CVT(x) wstring_to_charset(x, _charset)


void XmlWriter::write_start_document()
{
	_text.clear();
	_out << "<?xml version=\"1.0\" encoding=\"" << _charset_ext << "\" ?>\n";
}


static void value_format_repl(std::string& rep)
{
	if (rep.compare(0, 1, "&") == 0)
	{
		rep = "&amp;";
	}
	else
		rep = "&quot;";
}
void XmlWriter::write_element(const std::wstring& name, const xml::SaxAttributes& attributes)
{
	if (!_text.empty())
	{
		_out << "\n";
		IDOUT << _text;
		_text.clear();
	}
	if (!_level.empty())
	{
		_level.top() = true;
		_out << NEW_LINE;
	}
	IDOUT << "<" << CVT(name);
	lex::Regex<char> value_format_regex("&|\"");
	for (size_t i = 0; i < attributes.size(); i++)
	{
		std::string value =
				lex::regex_replace<char>(CVT(attributes[i].value), value_format_regex, value_format_repl);
		_out << " " << CVT(attributes[i].name) << "=\"" << value << "\"";
	}
	_out << "/>";
}

void XmlWriter::write_start_element(const std::wstring& name, const xml::SaxAttributes& attributes)
{
	if (!_text.empty())
	{
		_out << NEW_LINE;
		IDOUT << _text;
		_text.clear();
	}
	if (!_level.empty())
	{
		_level.top() = true;
		_out << NEW_LINE;
	}
	IDOUT << "<" << CVT(name);
	lex::Regex<char> value_format_regex("&|\"");
	for (size_t i = 0; i < attributes.size(); i++)
	{
		std::string value =
				lex::regex_replace<char>(CVT(attributes[i].value), value_format_regex, value_format_repl);
		_out << " " << CVT(attributes[i].name) << "=\"" << value << "\"";
	}
	_out << ">";
	_level.push(false);
}



// Format text with XML entity.
static void text_format_repl(std::string& rep)
{
	if (rep.compare(0, 1, "&") == 0)
	{
		rep = "&amp;";
	}
	else if (rep.compare(0, 1, "<") == 0)
		rep = "&lt;";
	else
		rep = "&gt;";
}
void XmlWriter::write_text(const std::wstring& text)
{
	if (_level.empty())
		return;
	lex::Regex<char> text_format_regex("&|<|>");
	std::string value = lex::regex_replace<char>(CVT(text),
			text_format_regex, text_format_repl);
	_text += value;
}

void XmlWriter::write_entity(const std::wstring& entity)
{
	if (_level.empty())
		return;
	_text += CVT(entity);
}

void XmlWriter::write_cdata(const std::wstring& text)
{
	if (_level.empty())
		return;
	if (!_text.empty())
	{
		_out << NEW_LINE;
		IDOUT << _text;
		_text.clear();
	}
	if (!_level.empty())
	{
		_level.top() = true;
		_out << NEW_LINE;
	}
	IDOUT << "<![CDATA[" << CVT(text) << "]]>";
}

void XmlWriter::write_comment(const std::wstring& text)
{
	if (!_text.empty())
	{
		_out << NEW_LINE;
		IDOUT << _text;
		_text.clear();
	}
	if (!_level.empty())
	{
		_level.top() = true;
		_out << NEW_LINE;
	}
	IDOUT << "<!--" << CVT(text) << "-->";
}

void XmlWriter::write_processing_instruction(const std::wstring& target, const std::wstring& text)
{
	if (!_text.empty())
	{
		_out << NEW_LINE;
		IDOUT << _text;
		_text.clear();
	}
	if (!_level.empty())
	{
		_level.top() = true;
		_out << NEW_LINE;
	}
	if (text.empty())
		IDOUT << "<?" << CVT(target) << "?>";
	else
		IDOUT << "<?" << CVT(target + L" " + text) << "?>";
}

void XmlWriter::write_end_element(const std::wstring& name)
{
	if (_level.empty())
		return;
	if (!_text.empty())
	{
		if (_level.top())
		{
			_out << NEW_LINE;
			IDOUT << _text;
		}
		else
			_out << _text;
		_text.clear();
	}
	if (_level.top())
	{
		_level.pop();
		_out << NEW_LINE;
		if (_format)
			indent_out(_out, _level);
	}
	else
		_level.pop();
	_out << "</" << CVT(name) << ">";
}

void XmlWriter::write_end_document()
{
	_out.flush();
}





SaxWriter::SaxWriter(std::ostream& out, const std::string& charset, bool format)
: _writer(out, charset, format)
{
}
SaxWriter::SaxWriter(std::ofstream& out, const std::string& charset, bool format)
: _writer(out, charset, format)
{
}
SaxWriter::~SaxWriter()
{
}

void SaxWriter::on_start_document()
{
	_writer.write_start_document();
}

void SaxWriter::on_processing_instruction(const std::wstring& target, const std::wstring& text)
{
	if (target.compare(L"xml") == 0)
	{
		// this is a XML-PI.
	}
	else
	{
		_writer.write_processing_instruction(target, text);
	}
}

void SaxWriter::on_element(const std::wstring& name, const xml::SaxAttributes& attributes)
{
	_writer.write_element(name, attributes);
}

void SaxWriter::on_start_element(const std::wstring& name, const xml::SaxAttributes& attributes)
{
	_writer.write_start_element(name, attributes);
}

void SaxWriter::on_text(const std::wstring& text)
{
	_writer.write_text(text);
}

void SaxWriter::on_entity(const std::wstring& entity)
{
	_writer.write_entity(entity);
}

void SaxWriter::on_cdata(const std::wstring& text)
{
	_writer.write_cdata(text);
}

void SaxWriter::on_comment(const std::wstring& text)
{
	_writer.write_comment(text);
}

void SaxWriter::on_end_element(const std::wstring& name)
{
	_writer.write_end_element(name);
}

void SaxWriter::on_end_document()
{
	_writer.write_end_document();
}


}
}


