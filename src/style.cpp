#include "style.hpp"

Style get_style(xmlpp::Element const &_element)
{
    return parse_style(_element.get_attribute_value("style"));
}

Style parse_style(Glib::ustring const &_str)
{
    Style style;
    if(!_str.empty())
    {
	Glib::ustring::size_type start = 0, end, delim;
	do
	{
	    end = _str.find(';', start);
	    delim = _str.find(':', start);

	    Glib::ustring name = _str.substr(start, delim - start);
	    Glib::ustring value = _str.substr(delim + 1, end - delim - 1);
	    style[name] = value;

	    start = end + 1;

	} while(end != Glib::ustring::npos);
    }
    return style;
}

Glib::ustring style_to_str(Style const &_style)
{
    Glib::ustring str;
    for(Style::const_iterator itr = _style.begin(); itr != _style.end(); ++itr)
    {
	if(itr != _style.begin()) str += ';';
	str += itr->first;
	str += ':';
	str += itr->second;
    }
    return str;
}