#include "link_renderer.hpp"

LinkRenderer::LinkRenderer()
    : doc(nullptr)
{

}

LinkRenderer::LinkRenderer(xmlpp::Document *_doc)
    : doc(_doc)
{

}

void LinkRenderer::set_document(xmlpp::Document *_doc)
{
    doc = _doc;
}

#include <iostream>
void LinkRenderer::draw(Cairo::RefPtr<Cairo::Context> const &_cr)
{
    if(_cr && doc)
    {
	auto root = doc->get_root_node();
	if(root)
	{
	    xmlpp::Node::PrefixNsMap map;
	    map["ksa"] = "http://nilksa.ru/namespaces/ksa_inkscape_extension";	    
	    auto nodes = root->find("/descendant::ksa:link-point", map);
	    for(auto *node:nodes)
	    {
		double cx, cy;
		std::stringstream in;
		in << static_cast<xmlpp::Element*>(node)->get_attribute_value("cx") << ' ' << static_cast<xmlpp::Element*>(node)->get_attribute_value("cy");
		in >> cx >> cy;

		_cr->set_source_rgb(0, 0, 0);
		_cr->arc(cx, cy, 5, 0, 2 * M_PI);
		_cr->stroke();
	    }
	}
    }
}
