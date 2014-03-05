#include "link_renderer.hpp"
#include "app.hpp"
#include "transform.hpp"
#include "style.hpp"
#include "support.hpp"

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
    for(auto itr = bm.begin(); itr != bm.end(); ++itr)
	if(itr->second.inside(_x, _y))
	    elements.push_back(itr->first);
    return elements;
}

geser::Bounds LinkRenderer::get_bounds(xmlpp::Element *_element) const
{
    geser::Bounds bounds;
    auto itr = bm.find(_element);
    if(itr != bm.end())bounds = itr->second;
    return bounds;
}

void LinkRenderer::grab_links()
{
    bm.clear();

    auto links = get_links();
    for(auto itr = links.begin(); itr != links.end(); ++itr)
    {
	geser::Bounds bounds;
	xmlpp::Element *element = *itr, *start_point = nullptr, *end_point = nullptr;
	get_points(element, start_point, end_point);
	if(start_point && end_point)
	{
	    double cx1 = std::atof(start_point->get_attribute_value("cx").c_str());
	    double cy1 = std::atof(start_point->get_attribute_value("cy").c_str());
	    double cx2 = std::atof(end_point->get_attribute_value("cx").c_str());
	    double cy2 = std::atof(end_point->get_attribute_value("cy").c_str());

	    Transform transform1(*start_point), transform2(*end_point);
	    transform1.convert(cx1, cy1);
	    transform2.convert(cx2, cy2);

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
	    bm[element] = bounds;
	}
    }
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
		Glib::ustring link_from = link->get_attribute_value("linkFrom", "ksa");
		Glib::ustring link_to = link->get_attribute_value("linkTo", "ksa");
		Glib::ustring object_id = link->get_attribute_value("object", "ksa");

		auto *link_point = get_link_point(link->get_parent(), link_from);
		double x1 = std::atof(link_point->get_attribute_value("cx").c_str());
		double y1 = std::atof(link_point->get_attribute_value("cy").c_str());
		Transform transform1(*link_point);
		Style style = get_style(*link_point);
		Gdk::Color color(style["fill"]);

		Glib::ustring id = object_id.substr(object_id.find('@') + 1);
		auto nodes = doc->get_root_node()->find("/descendant::*[@id = '" + id + "']", ns_map);
		if(!nodes.empty())
		{
		    link_point = get_link_point(static_cast<xmlpp::Element*>(nodes.front()), link_to);
		    double x2 = std::atof(link_point->get_attribute_value("cx").c_str());
		    double y2 = std::atof(link_point->get_attribute_value("cy").c_str());
		    Transform transform2(*link_point);

		    _cr->save();

		    _cr->set_source_rgb(color.get_red_p(), color.get_green_p(), color.get_blue_p());
		    _cr->set_line_width(4);

		    _cr->save();
		    _cr->transform(transform1.get_matrix());
		    _cr->move_to(x1, y1);
		    _cr->restore();

		    _cr->save();
		    _cr->transform(transform2.get_matrix());
		    _cr->line_to(x2, y2);
		    _cr->restore();

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
	auto link_points = _parent->find("./*[@ksa:mode='link-point' and @ksa:title = '" + _title + "']", ns_map);
	if(!link_points.empty()) link_point = static_cast<xmlpp::Element*>(link_points.front());
    }
    return link_point;
}
