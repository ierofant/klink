#include "svg_widget.hpp"
#include "app.hpp"
#include "support.hpp"
#include "transform.hpp"
#include "style.hpp"

SvgWidget::SvgWidget()
    : Glib::ObjectBase(typeid(*this)),
      property_current_element_(*this, "current-group", nullptr),
      property_root_group_(*this, "root-group", nullptr),
      property_start_point_(*this, "start-point", nullptr),
      property_end_point_(*this, "end-point", nullptr)
{
    add_events(Gdk::BUTTON_PRESS_MASK |
	       Gdk::BUTTON_RELEASE_MASK |
	       Gdk::POINTER_MOTION_MASK |
	       Gdk::SCROLL_MASK |
	       Gdk::SMOOTH_SCROLL_MASK);
    signal_change_current_element().connect(sigc::mem_fun(*this, &SvgWidget::on_change_current_element));
    signal_change_root_group().connect(sigc::mem_fun(*this, &SvgWidget::on_change_root_group));
    signal_change_start_point().connect(sigc::mem_fun(*this, &SvgWidget::on_change_start_point));
    signal_change_end_point().connect(sigc::mem_fun(*this, &SvgWidget::on_change_end_point));
}

double SvgWidget::get_x_pos() const
{
    return x_pos;
}

double SvgWidget::get_y_pos() const
{
    return y_pos;
}

void SvgWidget::set_source_file(Glib::ustring const &_filename)
{
    geser::SvgWidget::set_source_file(_filename);
    link_renderer.grab_links();
    set_root_group(get_root_node());
}

xmlpp::Element* SvgWidget::get_current_element()
{
    return property_current_element_;
}

void SvgWidget::set_current_element(xmlpp::Element *_element)
{
    property_current_element_.set_value(_element);
}

xmlpp::Element* SvgWidget::get_root_group()
{
    return property_root_group_;
}

void SvgWidget::set_root_group(xmlpp::Element *_element)
{
    property_root_group_.set_value(_element);
}

xmlpp::Element* SvgWidget::get_start_point()
{
    return property_start_point_;
}

void SvgWidget::set_start_point(xmlpp::Element *_element)
{
    property_start_point_.set_value(_element);
}

xmlpp::Element* SvgWidget::get_end_point()
{
    return property_end_point_;
}

void SvgWidget::set_end_point(xmlpp::Element *_element)
{
    property_end_point_.set_value(_element);
}

xmlpp::Element* SvgWidget::get_element_at(double _x, double _y)
{
    xmlpp::Element *element = nullptr;
    ElementSet set = get_elements_at(_x, _y);
    if(!set.empty())
    {
	element = set.front();
	auto func = [&](xmlpp::Element *_element)
	{
	    if(get_bounds(_element).area() < get_bounds(element).area()) element = _element;
	};

	std::for_each(set.begin(), set.end(), func);
    }
    return element;
}

Glib::SignalProxyProperty SvgWidget::signal_change_current_element()
{
    return property_current_element().signal_changed();
}

Glib::SignalProxyProperty SvgWidget::signal_change_root_group()
{
    return property_root_group().signal_changed();
}

Glib::SignalProxyProperty SvgWidget::signal_change_start_point()
{
    return property_start_point().signal_changed();
}

Glib::SignalProxyProperty SvgWidget::signal_change_end_point()
{
    return property_end_point().signal_changed();
}

Glib::PropertyProxy<xmlpp::Element*> SvgWidget::property_current_element()
{
    return property_current_element_.get_proxy();
}

Glib::PropertyProxy<xmlpp::Element*> SvgWidget::property_root_group()
{
    return property_root_group_.get_proxy();
}

Glib::PropertyProxy<xmlpp::Element*> SvgWidget::property_start_point()
{
    return property_start_point_.get_proxy();
}

Glib::PropertyProxy<xmlpp::Element*> SvgWidget::property_end_point()
{
    return property_end_point_.get_proxy();
}

bool SvgWidget::on_button_press_event(GdkEventButton *_event)
{
    geser::SvgWidget::on_button_press_event(_event);
    auto app = App::get();
    if(app && _event)
    {
	double x = _event->x / get_scale(), y = _event->y / get_scale();
	x_pos = _event->x;
	y_pos = _event->y;

	auto element = get_element_at(x, y);
	set_current_element(element);

	switch(app->get_mode())
	{
	    case LINK:
		if(is_link_point(element) && !has_pair(element)) set_start_point(element);
		break;
	}
    }
    return false;
}

bool SvgWidget::on_button_release_event(GdkEventButton *_event)
{
    geser::SvgWidget::on_button_release_event(_event);

    auto app = App::get();
    if(_event && app)
    {
	double x = _event->x / get_scale();
	double y = _event->y / get_scale();

	switch(app->get_mode())
	{
	    case LINK:
		if(get_start_point()) 
		{
		    auto *point = get_element_at(x, y);
		    if(is_link_point(point))
		    {
			set_end_point(point);
			refresh();
		    }

		    set_start_point(nullptr);
		    queue_draw();
		}
		break;
	}
    }
    return false;
}

bool SvgWidget::on_motion_notify_event(GdkEventMotion *_event)
{
    geser::SvgWidget::on_motion_notify_event(_event);

    if(_event)
    {
	auto app = App::get();
	auto *current_element = get_current_element();
	double scale = get_scale();
	double dx = (_event->x - x_pos) / scale, dy = (_event->y - y_pos) / scale;
	x_pos = _event->x;
	y_pos = _event->y;

	switch(app->get_mode())
	{
	    case CURSOR:
	    case LINK:
		{
		    auto element = get_element_at(x_pos / scale, y_pos / scale);

		    if(element != marker_renderer.get_active_link_point() &&
		       (element == nullptr || is_link_point(element)))
		    {
			marker_renderer.set_active_link_point(has_pair(element) ? nullptr : element);
			queue_draw();
		    }

		    if(is_link_point(current_element))
		    {
			if(!get_start_point() && _event->state == GDK_BUTTON1_MASK)
			{
			    auto *pair = get_pair(current_element);
			    remove_element(get_link(current_element));
			    remove_element(get_link(pair));
			    set_start_point(pair);
			}
			queue_draw();
		    }
		}
		break;
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
	    remove_element(current_element);
	    set_current_element(nullptr);
	}
    }
    return false;
}

bool SvgWidget::on_draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    geser::SvgWidget::on_draw(_cr);
    link_renderer.draw(_cr);
    marker_renderer.draw(_cr);
    return false;
}

SvgWidget::ElementSet SvgWidget::get_elements_at_vfunc(int _x, int _y) const
{
    SvgWidget::ElementSet elements;
    auto app = App::get();

    switch(app->get_mode())
    {
	case CURSOR:
	    elements = geser::SvgWidget::get_elements_at_vfunc(_x, _y);
	    break;

	case LINK:
	    {
		auto link_points = geser::SvgWidget::get_elements_at_vfunc(_x, _y);
		auto link = link_renderer.get_elements_at(_x, _y);
		std::merge(link_points.begin(), link_points.end(), link.begin(), link.end(), std::back_inserter(elements));
	    }
	    break;
    }
    return elements;
}

geser::Bounds SvgWidget::get_bounds_vfunc(xmlpp::Element *_element) const
{
    geser::Bounds bounds;
    if(_element && _element->get_namespace_prefix() == "ksa")
    {
	if(_element->get_name() == "link") bounds = link_renderer.get_bounds(_element);
    }
    else bounds = geser::SvgWidget::get_bounds_vfunc(_element);
    return bounds;
}

void SvgWidget::on_change_mode()
{
    auto app = App::get();
    switch(app->get_mode())
    {
	case CURSOR:
	    on_cursor_mode_activate();
	    break;

	case LINK:
	    on_link_mode_activate();
	    break;
    }
}

void SvgWidget::on_cursor_mode_activate()
{
    set_root_group(get_root_node());
}

void SvgWidget::on_link_mode_activate()
{
    auto *root = get_root_node();
    if(root)
    {
	auto nodes = root->find("./descendant::*[@ksa:mode='link-point']", ns_map);
	grab_items(nodes);
    }
}

void SvgWidget::on_change_current_element()
{
    queue_draw();
}

void SvgWidget::on_change_root_group()
{
    xmlpp::Element *root_group = get_root_group();
    if(root_group) grab_items(root_group->get_children());
}

void SvgWidget::on_change_start_point()
{

}

void SvgWidget::on_change_end_point()
{
    auto *start_point = get_start_point();
    auto *end_point = get_end_point();
    add_link(start_point, end_point);
}

void SvgWidget::add_link(xmlpp::Element *_start_point, xmlpp::Element *_end_point)
{
    if(_start_point && 
       _end_point &&
       _start_point != _end_point &&
       !has_pair(_start_point) &&
       !has_pair(_end_point))
    {
	auto *parent1 = _start_point->get_parent();
	auto *parent2 = _end_point->get_parent();
	if(parent1 && parent2)
	{

	    Glib::ustring start_id = _start_point->get_attribute_value("id");
	    Glib::ustring end_id = _end_point->get_attribute_value("id");

	    Style start_style = get_style(*_start_point);
	    Glib::ustring color = start_style["stroke"];
	    start_style["fill"] = color;
	    _start_point->set_attribute("pairId", end_id, "ksa");
	    _start_point->set_attribute("style", style_to_str(start_style));
	    
	    Style end_style = get_style(*_end_point);
	    end_style["fill"] = end_style["stroke"] = color;
	    _end_point->set_attribute("pairId", start_id, "ksa");
	    _end_point->set_attribute("style", style_to_str(end_style));

	    Glib::ustring root_id = get_document()->get_root_node()->get_attribute_value("id");
	    Glib::ustring id1 = parent1->get_attribute_value("id");
	    Glib::ustring id2 = parent2->get_attribute_value("id");
	    Glib::ustring title1 = _start_point->get_attribute_value("title", "ksa");
	    Glib::ustring title2 = _end_point->get_attribute_value("title", "ksa");

	    auto *link1 = parent1->add_child("link", "ksa");
	    link1->set_attribute("linkFrom", title1, "ksa");
	    link1->set_attribute("linkTo", title2, "ksa");
	    link1->set_attribute("object", root_id + '@' + id2, "ksa");

	    auto *link2 = parent2->add_child("link", "ksa");
	    link2->set_attribute("linkFrom", title2, "ksa");
	    link2->set_attribute("linkTo", title1, "ksa");
	    link2->set_attribute("object", root_id + '@' + id1, "ksa");

	    link_renderer.grab_links();
	}
    }
}

void SvgWidget::remove_element(xmlpp::Element *_element)
{
    if(is_link(_element)) remove_link(_element);
    else
    {
	remove_node(_element);

	auto app = App::get();
	if(app->get_mode() == CURSOR) grab_items(get_root_group()->get_children());
    }
    refresh();
}

void SvgWidget::remove_link(xmlpp::Element *_link)
{
    if(is_link(_link))
    {
	xmlpp::Element *start_point = nullptr;
	xmlpp::Element *end_point = nullptr;
	get_points(_link, start_point, end_point);
	if(start_point)
	{
	    Style style = get_style(*start_point);
	    style["fill"] = "none";
	    start_point->set_attribute("style", style_to_str(style));
	    start_point->remove_attribute("pairId", "ksa");	    
	}
	if(end_point)
	{
	    xmlpp::Element *link = get_link(end_point);
	    remove_node(link);

	    Style style = get_style(*end_point);
	    style["fill"] = "none";
	    end_point->set_attribute("style", style_to_str(style));
	    end_point->remove_attribute("pairId", "ksa");
	}
	remove_node(_link);
	link_renderer.grab_links();
    }
}

geser::Bounds SvgWidget::calculate_region(xmlpp::Element *_element, double _dx, double _dy)
{
    geser::Bounds bounds = get_bounds(_element);
    geser::Bounds region;
    if(_dx > 0)
    {
        region.x1 = std::floor(bounds.x1 - marker_renderer.delta) - 5;
        region.x2 = std::ceil(bounds.x1 + _dx + bounds.width() + marker_renderer.delta) + 5;
    }
    else
    {
        region.x1 = std::floor(bounds.x1 + _dx - marker_renderer.delta) - 5;
        region.x2 = std::ceil(bounds.x1 + bounds.width() + marker_renderer.delta) + 5;
    }

    if(_dy > 0)
    {
        region.y1 = std::floor(bounds.y1 - marker_renderer.delta) - 5;
        region.y2 = std::ceil(bounds.y1 + _dy + bounds.height() + marker_renderer.delta) + 5;
    }
    else
    {
        region.y1 = std::floor(bounds.y1 + _dy - marker_renderer.delta) - 5;
        region.y2 = std::ceil(bounds.y1 + bounds.height() + marker_renderer.delta) + 5;
    }
    return region;
}
