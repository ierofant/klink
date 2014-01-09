#include "link_renderer.hpp"
#include "app.hpp"
#include "support.hpp"

xmlpp::Node::PrefixNsMap LinkRenderer::ns_map(
{
    std::make_pair<Glib::ustring, Glib::ustring>("ksa", "http://nilksa.ru/namespaces/ksa_inkscape_extension")
});

LinkRenderer::ElementSet LinkRenderer::get_links() const
{
    ElementSet elements;
    auto app = App::get();
    if(app)
    {
	auto &svg_widget = app->get_svg_widget();
	auto *doc = svg_widget.get_document();

	if(doc)
	{
	    auto *root = doc->get_root_node();
	    if(root)
	    {
		auto nodes = root->find("/descendant::ksa:link", ns_map);

		for(auto *node:nodes)
		    if(is_link(node))
			elements.push_back(static_cast<xmlpp::Element*>(node));
	    }
	}
    }
    return elements;
}

LinkRenderer::ElementSet LinkRenderer::get_elements_at(double _x, double _y) const
{
    ElementSet elements;
    auto nodes = get_links();
    for(auto *node:nodes)
    {
	auto *element = static_cast<xmlpp::Element*>(node);
	geser::Bounds bounds = get_bounds(element);
	if(bounds.inside(_x, _y)) elements.push_back(element);
    }
    return elements;
}

geser::Bounds LinkRenderer::get_bounds(xmlpp::Element *_element) const
{
    geser::Bounds bounds;
    if(is_link(_element))
    {
	xmlpp::Element *start_point = nullptr, *end_point = nullptr;
	get_points(_element, start_point, end_point);
	if(start_point && end_point)
	{
	    double cx1 = std::atof(start_point->get_attribute_value("cx").c_str());
	    double cy1 = std::atof(start_point->get_attribute_value("cy").c_str());
	    double cx2 = std::atof(end_point->get_attribute_value("cx").c_str());
	    double cy2 = std::atof(end_point->get_attribute_value("cy").c_str());

	    if(cx1 < cx2)
	    {
		bounds.x1 = cx1 - 2;
		bounds.x2 = cx2 + 2;
	    }
	    else
	    {
		bounds.x1 = cx2 - 2;
		bounds.x2 = cx1 + 2;
	    }

	    if(cy1 < cy2)
	    {
		bounds.y1 = cy1 - 2;
		bounds.y2 = cy2 + 2;
	    }
	    else
	    {
		bounds.y1 = cy2 - 2;
		bounds.y2 = cy1 + 2;
	    }
	}
    }
    return bounds;
}

void LinkRenderer::draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    auto app = App::get();
    if(app && _cr)
    {
	auto &svg_widget = app->get_svg_widget();
	auto *doc = svg_widget.get_document();
	if(doc && doc->get_root_node())
	{
	    auto links = get_links();
	    for(auto *link:links)
	    {
		double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		Glib::ustring link_from = link->get_attribute_value("link-from", "ksa");
		Glib::ustring link_to = link->get_attribute_value("link-to", "ksa");
		Glib::ustring object_id = link->get_attribute_value("object-id", "ksa");

		auto *link_point = get_link_point(link->get_parent(), link_from);
		get_center(link_point, x1, y1);

		Glib::ustring id = object_id.substr(object_id.find('@') + 1);
		auto nodes = doc->get_root_node()->find("/descendant::*[@id = '" + id + "']", ns_map);
		if(!nodes.empty())
		{
		    link_point = get_link_point(static_cast<xmlpp::Element*>(nodes.front()), link_to);
		    get_center(link_point, x2, y2);

		    _cr->save();
		    _cr->move_to(x1, y1);
		    _cr->line_to(x2, y2);
		    _cr->set_source_rgb(0, 255, 255);
		    _cr->set_line_width(4);
		    _cr->stroke();
		    _cr->restore();
		}
	    }
	}
    }
}

xmlpp::Element* LinkRenderer::get_link_point(const xmlpp::Element *_parent, Glib::ustring const &_title)
{
    xmlpp::Element *link_point = nullptr;
    if(_parent)
    {
	auto link_points = _parent->find("./ksa:link-point[@ksa:title = '" + _title + "']", ns_map);
	if(!link_points.empty()) link_point = static_cast<xmlpp::Element*>(link_points.front());
    }
    return link_point;
}

