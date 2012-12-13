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


#include "dom.h"
#include "sax.h"
#include "../lex/regex.h"
#include "writer.h"

namespace tlib
{
namespace xml
{

DomNode::~DomNode()
{
}

// Do a search with path steps and current node, find matched node add it to node list.
static void select_nodes(const xpath::XPath& path, DomNode* cur, std::list<DomNodePtr>& nodes)
{

}

// Do a XPATH search get the matched nodes.
DomNodes DomNode::xget(const xpath::XPath& xpath) const throw (std::runtime_error)
{
	std::list<DomNodePtr> nodes;
#warning "not finished."


	return nodes;
}


static const char* _err_pos =
		"This type of node can not be added to the this location.";

DomContainer::DomContainer()
: DomNode()
{
}

DomContainer::~DomContainer()
{
}

bool DomContainer::has_child_nodes() const
{
	return !_children.empty();
}

const DomNodes DomContainer::get_child_nodes() const
{
	return DomNodes(_children);
}

void DomContainer::remove_child(DomNodePtr child_node)
{
	std::list<DomNodePtr>::iterator itr = _children.begin();
	while (itr != _children.end())
	{
		if ((*itr) == child_node)
		{
			(*itr)->_parent = 0;
			(*itr)->set_document(0);
			_children.erase(itr++);
			break;
		}
		else
			itr++;
	}
}

void DomContainer::clear_child_nodes()
{
	std::list<DomNodePtr>::const_iterator itr = _children.begin();
	for (; itr != _children.end(); itr++)
	{
		(*itr)->_parent = 0;
		(*itr)->set_document(0);
	}
	_children.clear();
}


void DomContainer::write_xml(std::wstring& result, bool format) const
{
	std::ostringstream out;
	write_xml(out, "utf-8", format);
	utf8_to_wstring(out.str(), result);
}

void DomContainer::write_xml(std::string& result, const std::string& charset, bool format) const
{
	std::ostringstream out;
	write_xml(out, charset, format);
	result = out.str();
}

static void make_xml(const DomContainer* node, XmlWriter& writer)
{
	if (node->get_node_type() == DomNode::node_document)
		writer.write_start_document();
	else
	{
		const DomElement* element = dynamic_cast<const DomElement*>(node);
		SaxAttributes sax_attrs;
		DomAttributes dom_attrs = element->get_attributes();
		for (size_t i = 0; i < dom_attrs.size(); i++)
			sax_attrs.push_back(SaxAttribute(dom_attrs[i]->get_node_name(), dom_attrs[i]->get_node_value()));
		if (element->has_child_nodes())
			writer.write_start_element(node->get_node_name(), sax_attrs);
		else
		{
			writer.write_element(node->get_node_name(), sax_attrs);
			return;
		}
	}
	DomNodes child_nodes = node->get_child_nodes();
	for (size_t i = 0; i < child_nodes.size(); i++)
	{
		switch (child_nodes[i]->get_node_type())
		{
		case DomNode::node_document:
		case DomNode::node_element:
			make_xml(dynamic_cast<const DomContainer*>(child_nodes[i].operator ->()), writer);
			break;
		case DomNode::node_text:
			writer.write_text(child_nodes[i]->get_node_value());
			break;
		case DomNode::node_cdata:
			writer.write_cdata(child_nodes[i]->get_node_value());
			break;
		case DomNode::node_comment:
			writer.write_comment(child_nodes[i]->get_node_value());
			break;
		default:
			break;
		}
	}
	if (node->get_node_type() == DomNode::node_document)
		writer.write_end_document();
	else
	{
		if (node->has_child_nodes())
			writer.write_end_element(node->get_node_name());
	}
}

void DomContainer::write_xml(std::ostream& out_stream, const std::string& charset, bool format) const
{
	XmlWriter writer(out_stream, charset, format);
	make_xml(this, writer);
}

void DomContainer::save_xml(const std::string& file, const std::string& charset, bool format) const throw(std::runtime_error)
{
	std::ofstream out(file.c_str(), std::ios::out | std::ios::binary);
	if (!out.good())
		throw std::runtime_error("Can't open file.");
	XmlWriter writer(out, charset, format);
	make_xml(this, writer);
	out.close();
}



extern const unsigned char name_check_bc[];
extern const unsigned int name_check_bc_length;

DomElement::DomElement(const std::wstring& name)
: DomContainer(), _node_name(name)
{
	if (is_trim_empty(_node_name))
		throw std::runtime_error("Node name can't be empty.");
	trim(_node_name);

	lex::Regex<wchar_t> name_check_regex(name_check_bc,
			name_check_bc_length, true);
	if (lex::regex_find(_node_name, name_check_regex) != _node_name.npos)
		throw std::runtime_error("Invalid node name.");
}

DomElementPtr DomElement::create(const std::wstring& name)
{
	return DomElementPtr(new DomElement(name));
}

DomNodePtr DomElement::clone_node() const
{
	DomElementPtr element = create(_node_name);
	for (size_t i = 0; i < _attributes.size(); i++)
		element->set_attribute_node(DomAttributePtr::cast_dynamic(_attributes[i]->clone_node()));

	std::list<DomNodePtr>::const_iterator itr;
	for (itr = _children.begin(); itr != _children.end(); itr++)
		element->append_child((*itr)->clone_node());

	return element;
}

const DomNode::NodeType DomElement::get_node_type() const
{
	return node_element;
}

const std::wstring DomElement::get_node_name() const
{
	return _node_name;
}

const std::wstring DomElement::get_node_value() const
{
	std::wstring value;
	std::list<DomNodePtr>::const_iterator itr;
	for (itr = _children.begin(); itr != _children.end(); itr++)
	{
		if ((*itr)->get_node_type() == node_text ||
				(*itr)->get_node_type() == node_cdata)
		{
			value += (*itr)->get_node_value();
		}
	}
	return value;
}

const std::wstring DomElement::get_text() const
{
	std::wstring text;
	std::list<DomNodePtr>::const_iterator itr;
	for (itr = _children.begin(); itr != _children.end(); itr++)
	{
		if ((*itr)->get_node_type() == node_text
				|| (*itr)->get_node_type() == node_cdata
				|| (*itr)->get_node_type() == node_element)
		{
			text += (*itr)->get_text();
		}
	}
	return text;
}

DomElement::~DomElement()
{
	clear_attributes();
	clear_child_nodes();
}

void DomElement::set_text(const std::wstring& value)
{
	_children.clear();
	append_child(DomText::create(value));
}

void DomElement::normalize()
{
	DomTextPtr text;
	std::list<DomNodePtr>::const_iterator itr = _children.begin();
	while (itr != _children.end())
	{
		if ((*itr)->get_node_type() == node_text)
		{
			if (text)
			{
				text->set_node_value(text->get_node_value() + (*itr)->get_node_value());
				remove_child((*itr++));
				continue;
			}
			else
				text = DomTextPtr::cast_dynamic(*itr);
		}
		else
			text.reset();
		itr++;
	}
}

// Attributes
void DomElement::set_attribute(const std::wstring& name, const std::wstring& value)
{
	set_attribute_node(DomAttribute::create(name, value));
}

DomAttributePtr DomElement::set_attribute_node(DomAttributePtr attribute)
{
	if (!attribute)
		throw std::runtime_error("Invalid node or node type.");

	if (attribute->_parent == this)
		return attribute;

	if (attribute->_parent != 0)
		dynamic_cast<DomElement*>(attribute->_parent)->remove_attribute_node(attribute);

	std::map<const std::wstring, size_t>::iterator itr =
			_attributes_names.find(attribute->get_node_name());
	if (itr != _attributes_names.end())
	{
		_attributes.erase(_attributes.begin() + itr->second);
		_attributes_names.erase(itr);
	}
	_attributes.push_back(attribute);
	_attributes_names[attribute->get_node_name()] = _attributes.size() - 1;
	attribute->_parent = this;
	attribute->set_document(_document);
	return attribute;
}

const std::wstring DomElement::get_attribute(const std::wstring& name) const
{
	DomAttributePtr attribute = get_attribute_node(name);
	if (attribute)
		return attribute->get_node_value();
	else
		return std::wstring();
}

DomAttributePtr DomElement::get_attribute_node(const std::wstring& name) const
{
	NameMap::const_iterator itr = _attributes_names.find(trim_copy(name));
	if (itr != _attributes_names.end())
	{
		return _attributes[itr->second];
	}
	else
		return DomAttributePtr(0);
}

void DomElement::remove_attribute(const std::wstring& name)
{
	NameMap::iterator itr = _attributes_names.find(trim_copy(name));
	if (itr != _attributes_names.end())
	{
		_attributes[itr->second]->_parent = 0;
		_attributes[itr->second]->set_document(0);
		_attributes.erase(_attributes.begin() + itr->second);
		_attributes_names.erase(itr);
	}
}

void DomElement::remove_attribute_node(DomAttributePtr attribute)
{
	if (!attribute || attribute->_parent != this)
		throw std::runtime_error("Node is not this element's attribute.");
	remove_attribute(attribute->get_node_name());
}

DomAttributes DomElement::get_attributes() const
{
	return DomAttributes(_attributes);
}

void DomElement::clear_attributes()
{
	for (size_t i = 0; i < _attributes.size(); i++)
	{
		_attributes[i]->_parent = 0;
		_attributes[i]->set_document(0);
	}
	_attributes.clear();
	_attributes_names.clear();
}



// Appends the node newChild at the end of the child nodes for this node
DomNodePtr DomElement::append_child(DomNodePtr new_node)
{
	if (!new_node || new_node->get_node_type() == node_document
			|| new_node->get_node_type() == node_attribute)
		throw std::runtime_error(_err_pos);
	if (new_node->_parent)
		new_node->_parent->remove_child(new_node);
	new_node->_parent = this;
	new_node->set_document(_document);
	_children.push_back(new_node);
	return new_node;
}

// Inserts a new node, newNode, before the existing node, refNode
DomNodePtr DomElement::insert_before(DomNodePtr new_node, DomNodePtr child)
{
	if (!new_node || new_node->get_node_type() == node_document
			|| new_node->get_node_type() == node_attribute)
		throw std::runtime_error(_err_pos);

	if (!child || child->_parent != this)
		throw std::runtime_error("The reference node is not this element's child.");

	if (new_node == child)
		return new_node;

	if (new_node->_parent)
		new_node->_parent->remove_child(new_node);

	std::list<DomNodePtr>::iterator itr = _children.begin();
	for (; itr != _children.end(); itr++)
	{
		if (*itr == child)
		{
			new_node->_parent = this;
			new_node->set_document(_document);
			_children.insert(itr, new_node);
			break;
		}
	}
	return new_node;
}

DomNodePtr DomElement::replace_child(DomNodePtr new_node, DomNodePtr child)
{

	if (!new_node || new_node->get_node_type() == node_document
			|| new_node->get_node_type() == node_attribute)
		throw std::runtime_error(_err_pos);

	if (!child || child->_parent != this)
		throw std::runtime_error("The reference node is not this element's child.");

	if (new_node == child)
		return new_node;

	if (new_node->_parent)
		new_node->_parent->remove_child(new_node);

	std::list<DomNodePtr>::iterator itr = _children.begin();
	for (; itr != _children.end(); itr++)
	{
		if (*itr == child)
		{
			new_node->_parent = this;
			new_node->set_document(_document);
			_children.insert(itr, new_node);
			_children.erase(itr);
			break;
		}
	}
	return new_node;

}

// Removes the specified node by nodeName
void DomElement::remove_child_by_name(const std::wstring& node_name)
{
	std::list<DomNodePtr>::iterator itr = _children.begin();
	while (itr != _children.end())
	{
		if ((*itr)->get_node_name() == node_name)
		{
			(*itr)->_parent = 0;
			(*itr)->set_document(0);
			_children.erase(itr++);
		}
		else
			itr++;
	}
}



void DomElement::set_document(DomDocument* document)
{
	_document = document;
	std::list<DomNodePtr>::const_iterator itr = _children.begin();
	for (; itr != _children.end(); itr++)
	{
		(*itr)->set_document(_document);
	}
	for (size_t i = 0; i < _attributes.size(); i++)
	{
		_attributes[i]->set_document(_document);
	}
}









/* Attribute node. */
DomAttribute::DomAttribute(const std::wstring& name)
: DomNode(), _node_name(name)
{
	if (is_trim_empty(_node_name))
		throw std::runtime_error("Node name can't be empty.");
	trim(_node_name);

	lex::Regex<wchar_t> name_check_regex(name_check_bc,
				name_check_bc_length, true);
	if (lex::regex_find(_node_name, name_check_regex) != _node_name.npos)
		throw std::runtime_error("Invalid node name.");
}

DomAttribute::DomAttribute(const std::wstring& name, const std::wstring& value)
: DomNode(), _node_name(name), _node_value(value)
{
	if (is_trim_empty(_node_name))
		throw std::runtime_error("Node name can't be empty.");
	trim(_node_name);

	lex::Regex<wchar_t> name_check_regex(name_check_bc,
				name_check_bc_length, true);
	if (lex::regex_find(_node_name, name_check_regex) != _node_name.npos)
		throw std::runtime_error("Invalid node name.");
}

DomAttributePtr DomAttribute::create(const std::wstring& name)
{
	DomAttributePtr attribute(new DomAttribute(name));
	return attribute;
}

DomAttributePtr DomAttribute::create(const std::wstring& name, const std::wstring& value)
{
	DomAttributePtr attribute(new DomAttribute(name, value));
	return attribute;
}

DomNodePtr DomAttribute::clone_node() const
{
	return create(_node_name, _node_value);
}

const DomNode::NodeType DomAttribute::get_node_type() const
{
	return node_attribute;
}

const std::wstring DomAttribute::get_node_name() const
{
	return _node_name;
}
const std::wstring DomAttribute::get_node_value() const
{
	return _node_value;
}
const std::wstring DomAttribute::get_text() const
{
	return _node_value;
}

void DomAttribute::set_node_value(const std::wstring& value)
{
	_node_value = value;
}

void DomAttribute::set_document(DomDocument* document)
{
	_document = document;
}








DomDocument::DomDocument()
: DomContainer()
{
	_document = this;
}

DomDocumentPtr DomDocument::create()
{
	return DomDocumentPtr(new DomDocument());
}

DomNodePtr DomDocument::clone_node() const
{
	DomDocumentPtr document = create();
	std::list<DomNodePtr>::const_iterator itr;
	for (itr = _children.begin(); itr != _children.end(); itr++)
		document->append_child((*itr)->clone_node());
	return document;
}

const DomNode::NodeType DomDocument::get_node_type() const
{
	return node_document;
}
const std::wstring DomDocument::get_node_name() const
{
	return L"#document";
}
const std::wstring DomDocument::get_node_value() const
{
	return L"";
}
const std::wstring DomDocument::get_text() const
{
	std::wstring text;
	std::list<DomNodePtr>::const_iterator itr;
	for (itr = _children.begin(); itr != _children.end(); itr++)
	{
		if ((*itr)->get_node_type() == node_element)
		{
			text += (*itr)->get_text();
		}
	}
	return text;
}

DomElementPtr DomDocument::get_root_node() const
{
	std::list<DomNodePtr>::const_iterator itr;
	for (itr = _children.begin(); itr != _children.end(); itr++)
	{
		if ((*itr)->get_node_type() == node_element)
		{
			return DomElementPtr::cast_dynamic(*itr);
		}
	}
	return DomElementPtr(0);
}

DomNodePtr DomDocument::append_child(DomNodePtr new_node)
{
	if (!new_node || new_node->get_node_type() == node_document
			|| new_node->get_node_type() == node_attribute
			|| new_node->get_node_type() == node_text
			|| new_node->get_node_type() == node_cdata)
		throw std::runtime_error(_err_pos);
	if (new_node->_parent)
		new_node->_parent->remove_child(new_node);
	new_node->_parent = this;
	new_node->set_document(_document);
	_children.push_back(new_node);
	return new_node;

}
DomNodePtr DomDocument::insert_before(DomNodePtr new_node, DomNodePtr child)
{
	if (!new_node || new_node->get_node_type() == node_document
			|| new_node->get_node_type() == node_attribute
			|| new_node->get_node_type() == node_text
			|| new_node->get_node_type() == node_cdata)
		throw std::runtime_error(_err_pos);

	if (!child || child->_parent != this)
		throw std::runtime_error("The reference node is not this element's child.");

	if (new_node == child)
		return new_node;

	if (new_node->_parent)
		new_node->_parent->remove_child(new_node);

	std::list<DomNodePtr>::iterator itr = _children.begin();
	for (; itr != _children.end(); itr++)
	{
		if (*itr == child)
		{
			new_node->_parent = this;
			new_node->set_document(_document);
			_children.insert(itr, new_node);
			break;
		}
	}
	return new_node;

}
void DomDocument::set_document(DomDocument* document)
{
}








DomContent::DomContent()
: DomNode()
{
}
DomContent::DomContent(const std::wstring& text)
: DomNode(), _node_value(text)
{
}
DomContent::~DomContent()
{
}
const std::wstring DomContent::get_node_value() const
{
	return _node_value;
}
const std::wstring DomContent::get_text() const
{
	return _node_value;
}
void DomContent::set_node_value(const std::wstring& text)
{
	_node_value = text;
}
void DomContent::set_document(DomDocument* document)
{
	_document = document;
}








DomText::DomText()
: DomContent()
{
}
DomText::DomText(const std::wstring& text)
: DomContent(text)
{
}
DomTextPtr DomText::create()
{
	return DomTextPtr(new DomText());
}
DomTextPtr DomText::create(const std::wstring& text)
{
	return DomTextPtr(new DomText(text));
}
DomNodePtr DomText::clone_node() const
{
	return DomTextPtr(new DomText(_node_value));
}
const DomNode::NodeType DomText::get_node_type() const
{
	return node_text;
}
const std::wstring DomText::get_node_name() const
{
	return L"#text";
}





DomComment::DomComment()
: DomContent()
{
}
DomComment::DomComment(const std::wstring& text)
: DomContent(text)
{
}
DomCommentPtr DomComment::create()
{
	return DomCommentPtr(new DomComment());
}
DomCommentPtr DomComment::create(const std::wstring& text)
{
	return DomCommentPtr(new DomComment(text));
}
DomNodePtr DomComment::clone_node() const
{
	return DomCommentPtr(new DomComment(_node_value));
}
const DomNode::NodeType DomComment::get_node_type() const
{
	return node_comment;
}
const std::wstring DomComment::get_node_name() const
{
	return L"#comment";
}






DomCdata::DomCdata()
: DomContent()
{
}
DomCdata::DomCdata(const std::wstring& text)
: DomContent(text)
{
}
DomCdataPtr DomCdata::create()
{
	return DomCdataPtr(new DomCdata());
}
DomCdataPtr DomCdata::create(const std::wstring& text)
{
	return DomCdataPtr(new DomCdata(text));
}
DomNodePtr DomCdata::clone_node() const
{
	return DomCdataPtr(new DomCdata(_node_value));
}
const DomNode::NodeType DomCdata::get_node_type() const
{
	return node_cdata;
}
const std::wstring DomCdata::get_node_name() const
{
	return L"#cdata";
}








DomParser::DomParser()
: _sax_parser(this)
{
}
DomParser::~DomParser()
{
}

void DomParser::on_start_document()
{
	_document = DomDocument::create();
	_path.push(_document);
}
void DomParser::on_processing_instruction(const std::wstring& text)
{
}
void DomParser::on_start_element(const std::wstring& name, const SaxAttributes& attributes)
{
	DomElementPtr element = DomElement::create(name);
	for (size_t i = 0; i < attributes.size(); i++)
	{
		DomAttributePtr attribute = DomAttribute::create(attributes[i].name, attributes[i].value);
		element->set_attribute_node(attribute);
	}
	_path.top()->append_child(element);
	_path.push(element);
}
void DomParser::on_element(const std::wstring& name, const SaxAttributes& attributes)
{
	DomElementPtr element = DomElement::create(name);
	for (size_t i = 0; i < attributes.size(); i++)
	{
		DomAttributePtr attribute = DomAttribute::create(attributes[i].name, attributes[i].value);
		element->set_attribute_node(attribute);
	}
	_path.top()->append_child(element);
}
void DomParser::on_text(const std::wstring& text)
{
	DomTextPtr content = DomText::create(text);
	_path.top()->append_child(content);
}
void DomParser::on_entity(const std::wstring& entity)
{
}
void DomParser::on_cdata(const std::wstring& text)
{
	DomCdataPtr content = DomCdata::create(text);
	_path.top()->append_child(content);
}
void DomParser::on_comment(const std::wstring& text)
{
	DomCommentPtr content = DomComment::create(text);
	_path.top()->append_child(content);
}
void DomParser::on_end_element(const std::wstring& name)
{
	_path.pop();
}
void DomParser::on_end_document()
{
	_path.pop();
}


}
}

