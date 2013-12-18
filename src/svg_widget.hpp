#ifndef KLINK_SVG_WIDGET_HPP
#define KLINK_SVG_WIDGET_HPP

#include <gtkmm/accelgroup.h>
#include <geser/svg_widget.hpp>
#include "link_renderer.hpp"
#include "marker_renderer.hpp"

class SvgWidget : public geser::SvgWidget
{
    public:
	SvgWidget();
	virtual ~SvgWidget() = default;

    public:
	xmlpp::Element* get_current_element();
	void set_current_element(xmlpp::Element *_element);
	xmlpp::Element* get_root_group();
	void set_root_group(xmlpp::Element *_element);
	Glib::SignalProxyProperty signal_change_current_element();
	Glib::SignalProxyProperty signal_change_root_group();
	Glib::PropertyProxy<xmlpp::Element*> property_current_element();
	Glib::PropertyProxy<xmlpp::Element*> property_root_group();

    protected:
	virtual bool on_button_press_event(GdkEventButton *_event) override;
	virtual bool on_motion_notify_event(GdkEventMotion *_event) override;
	virtual bool on_key_press_event(GdkEventKey *_event) override;
	virtual bool on_draw(Cairo::RefPtr<Cairo::Context> const &_cr) override;
	virtual ElementSet get_elements_at_vfunc(int _x, int _y) const override;
	virtual geser::Bounds get_bounds_vfunc(xmlpp::Element *_element) const override;
	virtual void on_change_current_element();

    private:
	void add_link_point(xmlpp::Node *_parent, double _x, double _y);

    private:
	double x_pos, y_pos;
	Glib::Property<xmlpp::Element*> property_current_element_;
	Glib::Property<xmlpp::Element*> property_root_group_;
	LinkRenderer link_renderer;
	MarkerRenderer marker_renderer;
};

#endif //KLINK_SVG_WIDGET_HPP
