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

#ifndef SAX_H_
#define SAX_H_

#include <stack>
#include "../tlibptr.h"
#include "../tlibustr.h"
#include "../lex/lexical.h"

namespace tlib
{
namespace xml
{

class SaxAttribute
{
public:
	inline SaxAttribute(const std::wstring& name, const std::wstring& text);
	std::wstring name;
	std::wstring value;
};

inline SaxAttribute::SaxAttribute(const std::wstring& name, const std::wstring& text)
: name(name), value(text)
{
}

typedef std::vector<SaxAttribute> SaxAttributes;

class SaxParserHandler
{
public:
	virtual ~SaxParserHandler() = 0;
protected:
	friend class SaxParser;
	virtual void on_start_document() {}
	virtual void on_processing_instruction(const std::wstring& /*target*/, const std::wstring& /*text*/) {}
	virtual void on_start_element(const std::wstring& /*name*/, const SaxAttributes& /*attributes*/) {}
	virtual void on_element(const std::wstring& /*name*/, const SaxAttributes& /*attributes*/) {}
	virtual void on_text(const std::wstring& /*text*/) {}
	virtual void on_entity(const std::wstring& /*entity*/) {}
	virtual	void on_cdata(const std::wstring& /*text*/) {}
	virtual void on_comment(const std::wstring& /*text*/) {}
	virtual void on_end_element(const std::wstring& /*name*/) {}
	virtual void on_end_document() {}
};



class SaxParser
{
public:
	explicit SaxParser(SaxParserHandler* handler = 0);
	~SaxParser();
	void set_handler(SaxParserHandler* handler);
	// Get the input stream's charset.
	inline const std::string& get_charset() const;
	inline const std::string& get_error() const;
	inline void set_substitute_entity(bool val = true);
	inline bool get_substitute_entity();

	template <typename T>
	bool parse(const std::basic_string<T>& src);
	bool parse_file(const std::string& file);
	bool parse(std::istream& ins, const std::string& charset = "utf-8",
			bool mbcs = true, bool charset_confirmed = false);
private:
	SaxParserHandler* _handler;
	bool _substitute_entity;
	std::string _charset;
	std::string _error;
};

inline const std::string& SaxParser::get_charset() const
{
	return _charset;
}
inline const std::string& SaxParser::get_error() const
{
	return _error;
}
inline void SaxParser::set_substitute_entity(bool val)
{
	_substitute_entity = val;
}
inline bool SaxParser::get_substitute_entity()
{
	return _substitute_entity;
}


} // namespace xml
} // namespace tlib




#endif /* SAX_H_ */
