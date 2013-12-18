#include <ctime>
#include "link_renderer.hpp"
#include "transform.hpp"

LinkRenderer::LinkRenderer()
    : doc(nullptr)
{

}

LinkRenderer::LinkRenderer(xmlpp::Document *_doc)
    : doc(_doc)
{

}

Glib::ustring LinkRenderer::unique_id() const
{
    auto nodes = get_link_points();
    std::time_t t = std::time(nullptr);
    std::ostringstream out;
    out << t << nodes.size();;
    return "link" + out.str();
}

LinkRenderer::ElementSet LinkRenderer::get_elements_at(double _x, double _y) const
{
    ElementSet elements;

    auto nodes = get_link_points();
    for(auto *node:nodes)
    {
	auto *el = static_cast<xmlpp::Element*>(node);
	std::stringstream ss;
	ss << el->get_attribute_value("cx") << ' ' << el->get_attribute_value("cy");

	double cx, cy;
	ss >> cx >> cy;
	if((cx - link_point_r) < _x &&
	   (cx + link_point_r) > _x &&
	   (cy - link_point_r) < _y &&
	   (cy + link_point_r) > _y) elements.push_back(el);
    }
    return elements;
}

geser::Bounds LinkRenderer::get_bounds(xmlpp::Element *_element) const
{
    geser::Bounds bounds;
    if(_element && _element->get_namespace_prefix() == "ksa")
    {
	if(_element->get_name() == "link-point")
	{
	    std::stringstream ss;
	    ss << _element->get_attribute_value("cx") << ' ' << _element->get_attribute_value("cy");
	    
	    double cx, cy;
	    ss >> cx >> cy;
	    bounds.x1 = cx - link_point_r;
	    bounds.y1 = cy - link_point_r;
	    bounds.x2 = cx + link_point_r;
	    bounds.y2 = cy + link_point_r;
	}
    }
    return bounds;
}

xmlpp::NodeSet LinkRenderer::get_link_points() const
{
    xmlpp::NodeSet nodes;
    if(doc)
    {
	auto *root = doc->get_root_node();
	if(root)
	{
	    xmlpp::Node::PrefixNsMap map;
	    map["ksa"] = "http://nilksa.ru/namespaces/ksa_inkscape_extension";
	    nodes = root->find("/descendant::ksa:link-point", map);
	}
    }
    return nodes;
}

void LinkRenderer::set_document(xmlpp::Document *_doc)
{
    doc = _doc;
}

void LinkRenderer::draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    if(_cr)
    {
        auto nodes = get_link_points();
        for(auto *node:nodes)
        {
	    double cx, cy;
	    xmlpp::Element *el = static_cast<xmlpp::Element*>(node);
	    std::stringstream in;
	    in << el->get_attribute_value("cx") << ' ' << el->get_attribute_value("cy");
	    in >> cx >> cy;

	    double tx, ty;
	    Transform t(el->get_attribute_value("transform"));
	    t.get_translate(tx, ty);

	    _cr->save();
	    if(tx != 0 || ty != 0) _cr->translate(tx, ty);
	    _cr->arc(cx, cy, link_point_r, 0, 2 * M_PI);
	    _cr->stroke();
	    _cr->restore();
	}
    }
}
