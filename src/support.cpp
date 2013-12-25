#include "support.hpp"
#include "transform.hpp"

bool is_link_point(const xmlpp::Element *_element)
{
    return (_element && _element->get_namespace_prefix() == "ksa" && _element->get_name() == "link-point");
}

void get_center(const xmlpp::Element *_element, double &_cx, double &_cy)
{
    Transform transform(_element->get_attribute_value("transform"));    
    double tx, ty;
    transform.get_translate(tx, ty);

    _cx = std::atof(_element->get_attribute_value("cx").c_str()) + tx;
    _cy = std::atof(_element->get_attribute_value("cy").c_str()) + ty;
}
