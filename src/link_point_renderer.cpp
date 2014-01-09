#include <ctime>
#include "link_point_renderer.hpp"
#include "transform.hpp"
#include "support.hpp"

LinkPointRenderer::LinkPointRenderer()
    : doc(nullptr)
{

}

LinkPointRenderer::LinkPointRenderer(xmlpp::Document *_doc)
    : doc(_doc)
{

}

Glib::ustring LinkPointRenderer::unique_id() const
{
    auto nodes = get_link_points();
    std::time_t t = std::time(nullptr);
    std::ostringstream out;
    out << t << nodes.size();;
    return "link" + out.str();
}

LinkPointRenderer::ElementSet LinkPointRenderer::get_elements_at(double _x, double _y) const
{
    ElementSet elements;

    auto nodes = get_link_points();
    for(auto *node:nodes)
    {
	auto *element = static_cast<xmlpp::Element*>(node);
	geser::Bounds bounds = get_bounds(element);
	if(bounds.inside(_x, _y)) elements.push_back(element);
    }
    return elements;
}

geser::Bounds LinkPointRenderer::get_bounds(xmlpp::Element *_element) const
{
    geser::Bounds bounds;
    if(_element && _element->get_namespace_prefix() == "ksa")
    {
	if(_element->get_name() == "link-point")
	{    
	    double cx, cy;
	    get_center(_element, cx, cy);

	    bounds.x1 = std::floor(cx - link_point_r);
	    bounds.y1 = std::floor(cy - link_point_r);
	    bounds.x2 = std::ceil(cx + link_point_r);
	    bounds.y2 = std::ceil(cy + link_point_r);
	}
    }
    return bounds;
}

xmlpp::NodeSet LinkPointRenderer::get_link_points() const
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

void LinkPointRenderer::set_document(xmlpp::Document *_doc)
{
    doc = _doc;
}

void LinkPointRenderer::draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    if(_cr)
    {
        auto nodes = get_link_points();
        for(auto *node:nodes)
        {
	    double cx, cy;
	    xmlpp::Element *element = static_cast<xmlpp::Element*>(node);
	    get_center(element, cx, cy);

	    _cr->save();
	    _cr->set_source_rgb(0, 255, 255);
	    _cr->arc(cx, cy, link_point_r, 0, 2 * M_PI);

	    if(has_pair(element))
	    {
		_cr->stroke_preserve();
		_cr->fill();
	    }
	    else _cr->stroke();
	    
	    _cr->restore();
	}
    }
}
