#include <libxml/tree.h>
#include "support.hpp"
#include "transform.hpp"

#include <iostream>

static const xmlpp::Element* get_root(const xmlpp::Node *_node)
{
    const xmlpp::Element *retval = nullptr;
    for(const xmlpp::Element *parent = _node->get_parent(); parent; parent = parent->get_parent()) retval = parent;
    return retval;
}

void remove_node(xmlpp::Node *_node)
{
    if(_node)
    {
	auto *parent = _node->get_parent();
	if(parent) parent->remove_child(_node);
    }
}

bool is_link_point(const xmlpp::Node *_node)
{
    bool retval = false;
    if(_node && _node->cobj()->type == XML_ELEMENT_NODE)
    {
	const auto *element = static_cast<const xmlpp::Element*>(_node);
	auto *attr = element->get_attribute("mode", "ksa");
	if(attr && attr->get_value() == "link-point") retval = true;
    }

    return retval;
}

bool get_center(const xmlpp::Element *_element, double &_cx, double &_cy)
{
    bool retval = false;    
    if(is_link_point(_element))
    {
	_cx = std::atof(_element->get_attribute_value("cx").c_str());
	_cy = std::atof(_element->get_attribute_value("cy").c_str());
	retval = true;
    }
    return retval;
}

bool has_pair(const xmlpp::Element *_element)
{
    bool retval = false;
    if(is_link_point(_element)) retval = !_element->get_attribute_value("pairId", "ksa").empty();
    return retval;
};

xmlpp::Element* get_pair(const xmlpp::Element *_element)
{
    xmlpp::Element *pair = nullptr;
    const xmlpp::Element *root = get_root(_element);
    if(has_pair(_element) && root)
    {
	Glib::ustring pair_id = _element->get_attribute_value("pairId", "ksa");
	auto nodes = root->find("./descendant::*[@ksa:mode='link-point' and @id='" + pair_id + "']", ns_map);
	if(!nodes.empty()) pair = static_cast<xmlpp::Element*>(nodes.front());
    }
    return pair;
}

xmlpp::Element* get_link(const xmlpp::Element *_link_point)
{
    xmlpp::Element *link = nullptr;
    const xmlpp::Element *parent = _link_point->get_parent();
    if(has_pair(_link_point) && parent)
    {
	Glib::ustring title = _link_point->get_attribute_value("title", "ksa");
	auto nodes = parent->find("./ksa:link[@ksa:linkFrom = '" +  title + "']", ns_map);
	if(!nodes.empty()) link = static_cast<xmlpp::Element*>(nodes.front());
    }
    return link;
}

bool is_link(const xmlpp::Node *_node)
{
    return (_node && 
	    _node->cobj()->type == XML_ELEMENT_NODE && 
	    _node->get_namespace_prefix() == "ksa" &&
	    _node->get_name() == "link");
}

void get_points(xmlpp::Element *_link, xmlpp::Element *&_start_point, xmlpp::Element *&_end_point)
{
    _start_point = _end_point = nullptr;
    if(is_link(_link))
    {
	Glib::ustring start_title = _link->get_attribute_value("linkFrom", "ksa");
	Glib::ustring end_title = _link->get_attribute_value("linkTo", "ksa");
	Glib::ustring object_id = _link->get_attribute_value("object", "ksa");
	const xmlpp::Element *start_parent = _link->get_parent();

	if(start_parent)
	{
	    auto nodes = start_parent->find("./*[@ksa:mode='link-point' and @ksa:title='" + start_title + "']", ns_map);
	    if(!nodes.empty())
	    {
		_start_point = static_cast<xmlpp::Element*>(nodes.front());
		_end_point = get_pair(_start_point);
	    }
	}

    }
}
