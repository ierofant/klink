#ifndef SUPPORT_HPP_INCLUDED
#define SUPPORT_HPP_INCLUDED

#include <libxml++/nodes/element.h>

bool is_link_point(const xmlpp::Element *_element);
void get_center(const xmlpp::Element *_element, double &_cx, double &_cy);

#endif //SUPPORT_HPP_INCLUDED
