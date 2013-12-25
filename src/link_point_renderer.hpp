#ifndef KLINK_LINK_POINT_RENDERER_HPP_INCLUDED
#define KLINK_LINK_POINT_RENDERER_HPP_INCLUDED

#include <libxml++/document.h>
#include <cairomm/context.h>
#include <geser/bounds.hpp>

class LinkPointRenderer
{
    public:
	typedef std::vector<xmlpp::Element*> ElementSet;

    public:
	LinkPointRenderer();
	LinkPointRenderer(xmlpp::Document *_doc);

    public:
	Glib::ustring unique_id() const;
	ElementSet get_elements_at(double _x, double _y) const;
	geser::Bounds get_bounds(xmlpp::Element *_element) const;
	xmlpp::NodeSet get_link_points() const;
	void set_document(xmlpp::Document *_doc);
	void draw(Cairo::RefPtr<Cairo::Context> const &_cr);

    private:
	static double constexpr const link_point_r = 5;
	xmlpp::Document *doc;
};

#endif //KLINK_LINK_POINT_RENDERER_HPP_INCLUDED
