#include "marker_renderer.hpp"
#include "app.hpp"
#include "transform.hpp"
#include "support.hpp"

#include <iostream>

xmlpp::Element* MarkerRenderer::get_active_link_point()
{
    return active_link_point;
}

void MarkerRenderer::set_active_link_point(xmlpp::Element *_active_link_point)
{
    active_link_point = _active_link_point;
}

void MarkerRenderer::draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    draw_current_element_marker(_cr);
    draw_active_link_point_marker(_cr);
    draw_link_hint_marker(_cr);
}

void MarkerRenderer::draw_current_element_marker(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    auto app = App::get();
    if(app && _cr) 
    {
	auto &svg_widget = app->get_svg_widget();
	auto *current_element = svg_widget.get_current_element();
	if(current_element)
	{
	    auto bounds = svg_widget.get_bounds(current_element);
	    if(bounds.area() > 0)
	    {
		_cr->save();
		_cr->set_source_rgb(0, 0, 0);
		_cr->set_line_width(line_width);
		_cr->set_dash(std::vector<double>({5, 5}), 0);
		_cr->rectangle(bounds.x1 - indent, bounds.y1 - indent, bounds.x2 - bounds.x1 + 2 * indent , bounds.y2 - bounds.y1 + 2 * indent);
		_cr->stroke();
		_cr->restore();
	    }
	}
    }
}

void MarkerRenderer::draw_active_link_point_marker(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    auto app = App::get();
    if(_cr && app && app->get_mode() == LINK && active_link_point)
    {
	Transform transform(*active_link_point);
	double cx = std::atof(active_link_point->get_attribute_value("cx").c_str());
	double cy = std::atof(active_link_point->get_attribute_value("cy").c_str());
	double r = std::atof(active_link_point->get_attribute_value("r").c_str());

	_cr->save();
	_cr->set_source_rgb(255, 0, 0);
	_cr->transform(transform.get_matrix());
	_cr->arc(cx, cy, r, 0, 2 * M_PI);
	_cr->fill();
	_cr->restore();
    }
}

void MarkerRenderer::draw_link_hint_marker(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    auto app = App::get();
    if(app && _cr)
    {
	auto &svg_widget = app->get_svg_widget();
	auto *start_point = svg_widget.get_start_point();
	if(start_point)
	{
	    double scale = svg_widget.get_scale();
	    double cx = std::atof(start_point->get_attribute_value("cx").c_str());
	    double cy = std::atof(start_point->get_attribute_value("cy").c_str());
	    Transform transform(*start_point);

	    _cr->save();
	    _cr->set_source_rgb(0, 0, 0);
	    _cr->set_line_width(line_width);

	    _cr->save();
	    _cr->transform(transform.get_matrix());
	    _cr->move_to(cx, cy);
	    _cr->restore();


	    _cr->line_to(svg_widget.get_x_pos() / scale, svg_widget.get_y_pos() / scale);
	    _cr->stroke();
	    _cr->restore();
	}
    }
}
