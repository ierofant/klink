#ifndef KLINK_SVG_WIDGET_HPP
#define KLINK_SVG_WIDGET_HPP

#include <geser/svg_widget.hpp>
#include "link_renderer.hpp"

class SvgWidget : public geser::SvgWidget
{
    public:
	SvgWidget();
	virtual ~SvgWidget() = default;

    protected:
	virtual bool on_draw(Cairo::RefPtr<Cairo::Context> const &_cr) override;

    private:
	LinkRenderer link_renderer;
};

#endif //KLINK_SVG_WIDGET_HPP
