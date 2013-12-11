#include "svg_widget.hpp"

SvgWidget::SvgWidget()
    : Glib::ObjectBase(typeid(*this))
{

}

bool SvgWidget::on_draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    geser::SvgWidget::on_draw(_cr);

    link_renderer.set_document(get_document());
    link_renderer.draw(_cr);
}