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


#ifndef WRITER_H_
#define WRITER_H_

#include "sax.h"

namespace tlib
{
namespace xml
{



class XmlWriter
{
public:
	XmlWriter(std::ostream& out, const std::string& charset = "utf-8", bool format = true);
	XmlWriter(std::ofstream& out, const std::string& charset = "utf-8", bool format = true);
	void write_start_document();
	void write_processing_instruction(const std::wstring& target, const std::wstring& text);
	void write_start_element(const std::wstring& name, const SaxAttributes& attributes);
	void write_element(const std::wstring& name, const SaxAttributes& attributes);
	void write_text(const std::wstring& text);
	void write_entity(const std::wstring& entity);
	void write_cdata(const std::wstring& text);
	void write_comment(const std::wstring& text);
	void write_end_element(const std::wstring& name);
	void write_end_document();
private:
	std::ostream& _out;
	std::string _charset;
	std::string _charset_ext;
	bool _format;
	// whether the level have any child nodes
	std::stack<bool> _level;
	std::string _text;
	std::locale _utf16le_loc;
	std::locale _utf16be_loc;
};


class SaxWriter: public SaxParserHandler
{
public:
	SaxWriter(std::ostream& out, const std::string& charset = "utf-8", bool format = true);
	SaxWriter(std::ofstream& out, const std::string& charset = "utf-8", bool format = true);
	virtual ~SaxWriter();
protected:
	virtual void on_start_document();
	virtual void on_processing_instruction(const std::wstring& target, const std::wstring& text);
	virtual void on_start_element(const std::wstring& name, const SaxAttributes& attributes);
	virtual void on_element(const std::wstring& name, const SaxAttributes& attributes);
	virtual void on_text(const std::wstring& text);
	virtual void on_entity(const std::wstring& entity);
	virtual	void on_cdata(const std::wstring& text);
	virtual void on_comment(const std::wstring& text);
	virtual void on_end_element(const std::wstring& name);
	virtual void on_end_document();
	XmlWriter _writer;
};


}
}


#endif /* WRITER_H_ */
