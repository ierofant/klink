#ifndef KLINK_LINK_RENDERER_HPP_INCLUDED
#define KLINK_LINK_RENDERER_HPP_INCLUDED

#include <libxml++/document.h>
#include <cairomm/context.h>

class LinkRenderer
{
    public:
	LinkRenderer();
	LinkRenderer(xmlpp::Document *_doc);

    public:
	void set_document(xmlpp::Document *_doc);
	void draw(Cairo::RefPtr<Cairo::Context> const &_cr);

    private:
	xmlpp::Document *doc;
};

#endif //KLINK_LINK_RENDERER_HPP_INCLUDED
