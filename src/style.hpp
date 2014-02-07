#ifndef KLINK_STYLE_HPP_INCLUDED
#define KLINK_STYLE_HPP_INCLUDED

#include <map>
#include <libxml++/nodes/element.h>

typedef std::map<Glib::ustring, Glib::ustring> Style;
Style get_style(xmlpp::Element const &_element);
Style parse_style(Glib::ustring const &_str);
Glib::ustring style_to_str(Style const &_style);

#endif //KLINK_STYLE_HPP_INCLUDED
