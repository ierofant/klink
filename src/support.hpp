#ifndef SUPPORT_HPP_INCLUDED
#define SUPPORT_HPP_INCLUDED

#include <libxml++/nodes/element.h>

static const xmlpp::Node::PrefixNsMap ns_map =
{
    std::make_pair<Glib::ustring, Glib::ustring>("ksa", "http://nilksa.ru/namespaces/ksa_inkscape_extension")
};

void remove_node(xmlpp::Node *_node);

bool is_link_point(const xmlpp::Node *_node);
bool get_center(const xmlpp::Element *_element, double &_cx, double &_cy);
bool has_pair(const xmlpp::Element *_element);
xmlpp::Element* get_pair(const xmlpp::Element *_element);
xmlpp::Element* get_link(const xmlpp::Element *_link_point);

bool is_link(const xmlpp::Node *_node);
void get_points(xmlpp::Element *_link, xmlpp::Element *&_start_point, xmlpp::Element *&_end_point);

#endif //SUPPORT_HPP_INCLUDED
