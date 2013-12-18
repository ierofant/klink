#include "svg_widget.hpp"
#include "app.hpp"
#include "transform.hpp"

SvgWidget::SvgWidget()
    : Glib::ObjectBase(typeid(*this)),
      property_current_element_(*this, "current-group", nullptr),
      property_root_group_(*this, "root-group", nullptr)
{
    add_events(Gdk::BUTTON_PRESS_MASK | 
	       Gdk::BUTTON1_MOTION_MASK |
	       Gdk::SCROLL_MASK |
	       Gdk::SMOOTH_SCROLL_MASK);
    signal_change_current_element().connect(sigc::mem_fun(*this, &SvgWidget::on_change_current_element));
}

xmlpp::Element* SvgWidget::get_current_element()
{
    return property_current_element_;
}

void SvgWidget::set_current_element(xmlpp::Element *_element)
{
    return property_current_element_.set_value(_element);
}

xmlpp::Element* SvgWidget::get_root_group()
{
    return property_root_group_;
}

void SvgWidget::set_root_group(xmlpp::Element *_element)
{
    property_root_group_.set_value(_element);
}

Glib::SignalProxyProperty SvgWidget::signal_change_current_element()
{
    return property_current_element().signal_changed();
}

Glib::SignalProxyProperty SvgWidget::signal_change_root_group()
{
    return property_root_group().signal_changed();
}

Glib::PropertyProxy<xmlpp::Element*> SvgWidget::property_current_element()
{
    return property_current_element_.get_proxy();
}

Glib::PropertyProxy<xmlpp::Element*> SvgWidget::property_root_group()
{
    return property_root_group_.get_proxy();
}

bool SvgWidget::on_button_press_event(GdkEventButton *_event)
{
    geser::SvgWidget::on_button_press_event(_event);
    auto app = App::get();
    if(app && _event)
    {
	switch(app->get_mode())
	{
	    case CURSOR:
		{
	    	    auto elements = get_elements_at(_event->x / get_scale(), _event->y / get_scale());
	    	    if(!elements.empty())
	    	    {
			xmlpp::Element *minimal = elements.front();
			for(auto *element:elements)
			    if(get_bounds(minimal).area() > get_bounds(element).area())
				minimal = element;
			set_current_element(minimal);
		    }
		}
		break;

	    case LINK_POINT:
		{
		    double x = _event->x / get_scale(), y = _event->y / get_scale();
		    auto nodes = link_renderer.get_elements_at(x, y);
		    auto *root_group = get_root_group();
		    if(nodes.empty() && root_group) add_link_point(root_group, x, y);
		    else set_current_element(nodes.front());
		}
		break;
	}
	x_pos = _event->x;
	y_pos = _event->y;
    }
    return false;
}

#include <iostream>
bool SvgWidget::on_motion_notify_event(GdkEventMotion *_event)
{
    geser::SvgWidget::on_motion_notify_event(_event);

    if(_event)
    {
	auto *current_element = get_current_element();
	if(current_element && 
	   current_element->get_namespace_prefix() == "ksa" &&
	   current_element->get_name() == "link-point")
	{
	    double dx = (_event->x - x_pos) / get_scale(), dy = (_event->y - y_pos) / get_scale();
	    x_pos = _event->x;
	    y_pos = _event->y;	    

	    Transform t1(current_element->get_attribute_value("transform")), t2;
std::cout << t1.str() << '\t' << t2.str() << std::endl;
	    t2.translate(dx, dy);
	    t2.merge(t1);
std::cout << '\t' << t2.str() << std::endl;
	    if(!t2.str().empty()) 
	    {
		current_element->set_attribute("transform", t2.str());
		refresh();
	    }
	}
    }
    return false;
}

bool SvgWidget::on_key_press_event(GdkEventKey *_event)
{
    geser::SvgWidget::on_key_press_event(_event);
    if(_event && _event->keyval == GDK_KEY_Delete)
    {
	xmlpp::Element *current_element = get_current_element();
	if(current_element)
	{
	    xmlpp::Node *parent = current_element->get_parent();
	    if(parent) 
	    {
		parent->remove_child(current_element);
		set_current_element(nullptr);
		refresh();
	    }
	}
    }
    return false;
}

bool SvgWidget::on_draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    geser::SvgWidget::on_draw(_cr);

    link_renderer.set_document(get_document());
    link_renderer.draw(_cr);
    marker_renderer.draw(_cr);

    return false;
}

SvgWidget::ElementSet SvgWidget::get_elements_at_vfunc(int _x, int _y) const
{
    SvgWidget::ElementSet elements;
    auto elements1 = geser::SvgWidget::get_elements_at_vfunc(_x, _y);
    auto elements2 = link_renderer.get_elements_at(_x, _y);
    
    std::merge(elements1.begin(), elements1.end(), elements2.begin(), elements2.end(), std::back_inserter(elements));
    return elements;
}

geser::Bounds SvgWidget::get_bounds_vfunc(xmlpp::Element *_element) const
{
    geser::Bounds bounds;
    if(_element && _element->get_namespace_prefix() == "ksa") bounds = link_renderer.get_bounds(_element);
    else bounds = geser::SvgWidget::get_bounds_vfunc(_element);
    return bounds;
}

void SvgWidget::on_change_current_element()
{
    queue_draw();
}

void SvgWidget::add_link_point(xmlpp::Node *_parent, double _x, double _y)
{
    if(_parent)
    {
	auto *link_point = _parent->add_child("link-point", "ksa");
	link_point->set_attribute("id", link_renderer.unique_id());

	std::ostringstream out;
	out << _x;
	link_point->set_attribute("cx", out.str());

	out.str("");
	out << _y;
	link_point->set_attribute("cy", out.str());

	set_current_element(link_point);
    }
}
