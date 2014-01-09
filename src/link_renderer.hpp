#ifndef LINK_RENDERER_HPP_INCLUDED
#define LINK_RENDERER_HPP_INCLUDED

#include <libxml++/document.h>
#include <cairomm/context.h>
#include <geser/bounds.hpp>

class LinkRenderer
{
    public:
	typedef std::vector<xmlpp::Element*> ElementSet;

    public:
	ElementSet get_links() const;
	ElementSet get_elements_at(double _x, double _y) const;
	geser::Bounds get_bounds(xmlpp::Element *_element) const;
	void draw(Cairo::RefPtr<Cairo::Context> const &_cr);

    private:
	xmlpp::Element* get_link_point(const xmlpp::Element *_parent, Glib::ustring const &_title);

    private:
	static xmlpp::Node::PrefixNsMap ns_map;
};

#endif //LINK_RENDERER_HPP_INCLUDED
