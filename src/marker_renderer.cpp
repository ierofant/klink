#include "marker_renderer.hpp"
#include "app.hpp"

void MarkerRenderer::draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    auto app = App::get();
    if(app && _cr)
    {
	draw_current_element_marker(_cr);
    }
}

void MarkerRenderer::draw_current_element_marker(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    auto app = App::get();
    auto &svg_widget = app->get_svg_widget();
    auto *current_element = svg_widget.get_current_element();
    if(current_element)
    {
	auto bounds = svg_widget.get_bounds(current_element);
	if(bounds.area() > 0)
	{
	    _cr->set_source_rgb(0, 0, 0);
	    _cr->set_line_width(0.5);
	    _cr->set_dash(std::vector<double>({5, 5}), 0);
	    _cr->rectangle(bounds.x1 - 1, bounds.y1 - 1, bounds.x2 - bounds.x1 + 2, bounds.y2 - bounds.y1 + 2);
	    _cr->stroke();
	}
    }
}