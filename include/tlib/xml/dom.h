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

#ifndef DOM_H_
#define DOM_H_

#include "../tlibptr.h"
#include "../tlibstr.h"
#include "../tlibustr.h"
#include "sax.h"
#include "xpath.h"
#include <list>
#include <vector>


namespace tlib
{
namespace xml
{

class DomNode;
class DomContainer;
class DomElement;
class DomDocument;
class DomAttribute;
class DomNodes;
class DomAttributes;
class DomText;
class DomComment;
class DomCdata;

typedef RefPtr<DomNode> DomNodePtr;
typedef RefPtr<DomContainer> DomContainerPtr;
typedef RefPtr<DomElement> DomElementPtr;
typedef RefPtr<DomDocument> DomDocumentPtr;
typedef RefPtr<DomAttribute> DomAttributePtr;
typedef RefPtr<DomText> DomTextPtr;
typedef RefPtr<DomComment> DomCommentPtr;
typedef RefPtr<DomCdata> DomCdataPtr;


class DomNodes
{
	friend class DomNode;
	friend class DomAttribute;
	friend class DomContainer;
	friend class DomElement;
	friend class DomDocument;
	friend class DomText;
	friend class DomComment;
	friend class DomCdata;
	inline DomNodes(const std::list<DomNodePtr>& nodes);
public:
	inline DomNodes(const DomNodes& nodes);
	inline size_t size() const;
	inline DomNodePtr operator [] (size_t index) const;
private:
	std::vector<DomNodePtr> _nodes;
};



class DomAttributes
{
	friend class DomElement;
	inline DomAttributes(const std::vector<DomAttributePtr>& attributes);
public:
	inline DomAttributes(const DomAttributes& attributes);
	inline size_t size() const;
	inline DomAttributePtr operator [] (size_t index) const;
	inline DomAttributePtr operator [] (const std::wstring& name) const;
private:
	std::vector<DomAttributePtr> _attributes;
	std::map<const std::wstring, size_t> _attributes_names;
};


class DomNode: public Object
{
protected:
	inline DomNode();
	DomNode(const DomNode&);
public:
	friend class DomElement;
	friend class DomDocument;
	friend class DomContainer;
	typedef enum _node_type
	{
		_node_document = 0,
		_node_element,
		_node_attribute,
		_node_text,
		_node_cdata,
		_node_comment,
	} NodeType;
	static const NodeType node_document = _node_document;
	static const NodeType node_element = _node_element;
	static const NodeType node_attribute = _node_attribute;
	static const NodeType node_text = _node_text;
	static const NodeType node_cdata = _node_cdata;
	static const NodeType node_comment = _node_comment;
	virtual ~DomNode();
public:
	// Returns an exact clone of this node.
	// If the boolean value is set to true,
	// the cloned node contains all the child nodes as well
	virtual DomNodePtr clone_node() const = 0;
	virtual const NodeType get_node_type() const = 0;
	virtual const std::wstring get_node_name() const = 0;
	virtual const std::wstring get_node_value() const = 0;
	virtual const std::wstring get_text() const = 0;
	inline DomContainerPtr get_parent() const;
	inline DomDocumentPtr get_document() const;
	// Do a XPATH search get the matched nodes.
	DomNodes xget(const xpath::XPath& xpath) const throw (std::runtime_error);
protected:
	virtual void set_document(DomDocument* document) = 0;
	DomContainer* _parent;
	DomDocument* _document;
};


class DomContainer: public DomNode
{
protected:
	DomContainer();
private:
	DomContainer(const DomContainer&);
public:
	virtual ~DomContainer();
	// Returns true if this node has any child nodes
	bool has_child_nodes() const;
	const DomNodes get_child_nodes() const;
	virtual DomNodePtr append_child(DomNodePtr new_node) = 0;
	virtual DomNodePtr insert_before(DomNodePtr new_node, DomNodePtr child) = 0;
	void remove_child(DomNodePtr child_node);
	void clear_child_nodes();
	void write_xml(std::wstring& out, bool format = true) const;
	void write_xml(std::string& out, const std::string& charset = "utf-8", bool format = true) const;
	void write_xml(std::ostream& out, const std::string& charset = "utf-8", bool format = true) const;
	void save_xml(const std::string& file, const std::string& charset = "utf-8", bool format = true) const throw(std::runtime_error);
protected:
	std::list<DomNodePtr> _children;
};


class DomElement: public DomContainer
{
	DomElement(const std::wstring& name);
	DomElement(const DomElement&);
public:
	static DomElementPtr create(const std::wstring& name);
	virtual DomNodePtr clone_node() const;
	virtual const NodeType get_node_type() const;
	virtual const std::wstring get_node_name() const;
	virtual const std::wstring get_node_value() const;
	virtual const std::wstring get_text() const;
	virtual ~DomElement();
protected:
	virtual void set_document(DomDocument* document);
public:
	// Remove all sub-tree add a new text node.
	void set_text(const std::wstring& value);
	// collapse all adjacent text nodes in sub-tree
	void normalize();

	// Attributes
	void set_attribute(const std::wstring& name, const std::wstring& value);
	DomAttributePtr set_attribute_node(DomAttributePtr attribute);
	const std::wstring get_attribute(const std::wstring& name) const;
	DomAttributePtr get_attribute_node(const std::wstring& name) const;
	void remove_attribute(const std::wstring& name);
	void remove_attribute_node(DomAttributePtr attribute);
	DomAttributes get_attributes() const;
	void clear_attributes();

	// Appends the node newChild at the end of the child nodes for this node
	virtual DomNodePtr append_child(DomNodePtr new_node);
	// Inserts a new node, newNode, before the existing node, refNode
	virtual DomNodePtr insert_before(DomNodePtr new_node, DomNodePtr child);
	DomNodePtr replace_child(DomNodePtr new_node, DomNodePtr child);
	// Removes the specified node by nodeName
	void remove_child_by_name(const std::wstring& node_name);
private:
	std::wstring _node_name;
	std::vector<DomAttributePtr> _attributes;
	typedef std::map<const std::wstring, size_t> NameMap;
	NameMap _attributes_names;
};


class DomAttribute: public DomNode
{
	DomAttribute(const std::wstring& name);
	DomAttribute(const std::wstring& name, const std::wstring& value);
	DomAttribute(const DomAttribute&);
	friend class DomElement;
public:
	static DomAttributePtr create(const std::wstring& name);
	static DomAttributePtr create(const std::wstring& name, const std::wstring& value);

	virtual DomNodePtr clone_node() const;
	virtual const NodeType get_node_type() const;
	virtual const std::wstring get_node_name() const;
	virtual const std::wstring get_node_value() const;
	virtual const std::wstring get_text() const;
public:
	void set_node_value(const std::wstring& value);
protected:
	virtual void set_document(DomDocument* document);
private:
	std::wstring _node_name;
	std::wstring _node_value;
};


class DomDocument: public DomContainer
{
	DomDocument();
	DomDocument(const DomDocument&);
public:
	static DomDocumentPtr create();
	virtual DomNodePtr clone_node() const;
	virtual const NodeType get_node_type() const;
	virtual const std::wstring get_node_name() const;
	virtual const std::wstring get_node_value() const;
	virtual const std::wstring get_text() const;

	DomElementPtr get_root_node() const;

	DomNodePtr append_child(DomNodePtr new_node);
	DomNodePtr insert_before(DomNodePtr new_node, DomNodePtr child);
protected:
	virtual void set_document(DomDocument* document);
};


class DomContent: public DomNode
{
protected:
	DomContent();
	DomContent(const std::wstring& text);
	friend class DomDocument;
	friend class DomElement;
private:
	// Not allow.
	DomContent(const DomContent&);
public:
	virtual ~DomContent();
	virtual const std::wstring get_node_value() const;
	virtual const std::wstring get_text() const;
	void set_node_value(const std::wstring& text);
protected:
	virtual void set_document(DomDocument* document);
protected:
	std::wstring _node_value;
};


class DomText: public DomContent
{
	DomText();
	DomText(const std::wstring& text);
	// Not allow.
	DomText(const DomText&);
public:
	static DomTextPtr create();
	static DomTextPtr create(const std::wstring& text);
	virtual DomNodePtr clone_node() const;
	virtual const NodeType get_node_type() const;
	virtual const std::wstring get_node_name() const;
};


class DomComment: public DomContent
{
	DomComment();
	DomComment(const std::wstring& text);
	// Not allow.
	DomComment(const DomComment&);
public:
	static DomCommentPtr create();
	static DomCommentPtr create(const std::wstring& text);
	virtual DomNodePtr clone_node() const;
	virtual const NodeType get_node_type() const;
	virtual const std::wstring get_node_name() const;
};


class DomCdata: public DomContent
{
	DomCdata();
	DomCdata(const std::wstring& text);
	// Not allow.
	DomCdata(const DomCdata&);
public:
	static DomCdataPtr create();
	static DomCdataPtr create(const std::wstring& text);
	virtual DomNodePtr clone_node() const;
	virtual const NodeType get_node_type() const;
	virtual const std::wstring get_node_name() const;
};



class DomParser: public SaxParserHandler
{
public:
	DomParser();
	virtual ~DomParser();
	// Get the input stream's charset.
	inline const std::string& get_charset() const;
	inline const std::string& get_error() const;

	template <typename T> inline
	bool parse(const std::basic_string<T>& src);
	inline bool parse_file(const std::string& file);
	inline bool parse(std::istream& ins, const std::string& charset = "utf-8",
			bool mbcs = true, bool charset_confirmed = false);

	inline DomDocumentPtr get_document() const;
private:
	virtual void on_start_document();
	virtual void on_processing_instruction(const std::wstring& text);
	virtual void on_start_element(const std::wstring& name, const SaxAttributes& attributes);
	virtual void on_element(const std::wstring& name, const SaxAttributes& attributes);
	virtual void on_text(const std::wstring& text);
	virtual void on_entity(const std::wstring& entity);
	virtual	void on_cdata(const std::wstring& text);
	virtual void on_comment(const std::wstring& text);
	virtual void on_end_element(const std::wstring& name);
	virtual void on_end_document();
private:
	DomDocumentPtr _document;
	SaxParser _sax_parser;
	std::stack<DomContainerPtr> _path;
};

inline const std::string& DomParser::get_error() const
{
	return _sax_parser.get_error();
}
inline const std::string& DomParser::get_charset() const
{
	return _sax_parser.get_charset();
}
template <typename T> inline
bool DomParser::parse(const std::basic_string<T>& src)
{
	bool result = _sax_parser.parse(src);
	if (!result)
	{
		_document.reset();
	}
	return result;
}
inline bool DomParser::parse_file(const std::string& file)
{
	bool result = _sax_parser.parse_file(file);
	if (!result)
	{
		_document.reset();
	}
	return result;
}
inline bool DomParser::parse(std::istream& ins, const std::string& charset,
		bool mbcs, bool charset_confirmed)
{
	bool result = _sax_parser.parse(ins, charset, mbcs, charset_confirmed);
	if (!result)
	{
		_document.reset();
	}
	return result;
}
inline DomDocumentPtr DomParser::get_document() const
{
	return _document;
}




inline DomNode::DomNode()
: _parent(0), _document(0)
{
}
inline DomContainerPtr DomNode::get_parent() const
{
	return DomContainerPtr(_parent);
}
inline DomDocumentPtr DomNode::get_document() const
{
	return DomDocumentPtr(_document);
}

inline DomNodes::DomNodes(const std::list<DomNodePtr>& nodes)
{
	_nodes.assign(nodes.begin(), nodes.end());
}

inline DomNodes::DomNodes(const DomNodes& nodes)
{
	_nodes.assign(nodes._nodes.begin(), nodes._nodes.end());
}
inline size_t DomNodes::size() const
{
	return _nodes.size();
}
inline DomNodePtr DomNodes::operator [] (size_t index) const
{
	return _nodes.at(index);
}

inline DomAttributes::DomAttributes(const std::vector<DomAttributePtr>& attributes)
{
	typedef std::map<const std::wstring, size_t>::value_type value_type;
	_attributes.assign(attributes.begin(), attributes.end());
	for (size_t i = 0; i < _attributes.size(); i++)
		_attributes_names.insert(value_type(_attributes[i]->get_node_name(), i));
}
inline DomAttributes::DomAttributes(const DomAttributes& attributes)
{
	typedef std::map<const std::wstring, size_t>::value_type value_type;
	_attributes.assign(attributes._attributes.begin(), attributes._attributes.end());
	for (size_t i = 0; i < _attributes.size(); i++)
		_attributes_names.insert(value_type(_attributes[i]->get_node_name(), i));
}
inline size_t DomAttributes::size() const
{
	return _attributes.size();
}
inline DomAttributePtr DomAttributes::operator [] (size_t index) const
{
	return _attributes.at(index);
}
inline DomAttributePtr DomAttributes::operator [] (const std::wstring& name) const
{
	return _attributes[_attributes_names.at(name)];
}


}
}

#endif /* DOM_H_ */
