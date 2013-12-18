#ifndef MARKER_RENDERER_HPP_INCLUDED
#define MARKER_RENDERER_HPP_INCLUDED

#include <cairomm/context.h>

class MarkerRenderer
{
    public:
	void draw(Cairo::RefPtr<Cairo::Context> const &_cr);

    private:
	void draw_current_element_marker(Cairo::RefPtr<Cairo::Context> const &_cr);
};

#endif //MARKER_RENDERER_HPP_INCLUDED
