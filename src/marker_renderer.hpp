#ifndef MARKER_RENDERER_HPP_INCLUDED
#define MARKER_RENDERER_HPP_INCLUDED

#include <libxml++/nodes/element.h>
#include <cairomm/context.h>

class MarkerRenderer
{
    public:
	xmlpp::Element* get_active_link_point();
	void set_active_link_point(xmlpp::Element *_active_link_point);
	void draw(Cairo::RefPtr<Cairo::Context> const &_cr);

    public:
	static double constexpr indent = 1;
	static double constexpr line_width = 0.5;
	static double constexpr delta = indent + line_width;

    private:
	void draw_current_element_marker(Cairo::RefPtr<Cairo::Context> const &_cr);
	void draw_active_link_point_marker(Cairo::RefPtr<Cairo::Context> const &_cr);
	void draw_link_hint_marker(Cairo::RefPtr<Cairo::Context> const &_cr);

    private:
	xmlpp::Element *active_link_point = nullptr;
};

#endif //MARKER_RENDERER_HPP_INCLUDED
