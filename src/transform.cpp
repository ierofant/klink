#include <locale>
#include <algorithm>
#include <deque>
#include "transform.hpp"

Transform::Transform(Glib::ustring const &_str)
{
    read(_str);
}

Transform::Transform(xmlpp::Element const &_element, bool _inherit)
{
    if(_inherit)
    {
	typedef std::deque<Transform> Tlist;

	Tlist tlist;
	tlist.push_front(Transform(_element.get_attribute_value("transform")));
	for(const xmlpp::Element *parent = _element.get_parent(); parent; parent = parent->get_parent()) tlist.push_front(Transform(*parent, false));

	*this = tlist.front();
	
	Tlist::iterator itr = tlist.begin();
	++itr;
	for(; itr != tlist.end(); ++itr) merge(*itr);
    }
    else read(_element.get_attribute_value("transform"));
}

Glib::ustring Transform::str() const
{
    std::ostringstream out;
    if(b == 0 && d == 0)
    {
	if(e != 0 && f != 0) 
	{
	    out << "translate(" << c;
	    if(c != f) out << ", " << f;
	    out << ")";
	}
	if(a != 1 && d != 1)
	{
	    if(!out.str().empty()) out << ' ';
	    out << "scale(" << a;
	    if(a != d)  out << ", " << e;
	    out << ")";
	}
    }
    else out << "matrix(" << a << ", " << b << ", " << c << ", " << d << ", " << e << ", " << f << ")";
    return out.str(); 
}

void Transform::get_translate(double &_tx, double &_ty) const
{
    _tx = e;
    _ty = f;
}

void Transform::get_scale(double &_sx, double &_sy) const
{
    _sx = a;
    _sy = d;
}

Cairo::Matrix Transform::get_matrix() const
{
    return Cairo::Matrix(a, b, c, d, e, f);
}

void Transform::read(Glib::ustring const &_str)
{
    if(!_str.empty())
    {
	std::istringstream in(_str);
        in >> std::skipws;
        while(in)
        {
	    std::string func;
	    std::getline(in, func, ')');
	
	    if(!func.empty())
	    {
		auto pos = func.find("(");
	        std::string name = func.substr(0, pos);
	        std::istringstream args(func.substr(pos + 1));

		if(name == "matrix")
	        {    
		    char sym;
	    	    args >> a >> sym >> b >> sym >> c >> sym >> d >> sym >> e >> sym >> f;
		}
		else if(name == "translate" || name =="scale")
		{
		    char sym;
		    double x, y;
	    	    args >> x;
	    	    if(args.good()) args >> sym >> y;
	    	    else x = y;
		    if(name == "translate") translate (x, y);
		    else scale(x, y);
		}
	    }
	}
    }
    else reset();
}

void Transform::reset()
{
    a = 1;
    b = 0;
    c = 0;
    d = 1;
    e = 0;
    f = 0;
}

void Transform::merge(Transform const &_transform)
{
    double new_a = a * _transform.a + c * _transform.b;
    double new_b = b * _transform.a + d * _transform.b;
    double new_c = a * _transform.c + c * _transform.d;
    double new_d = b * _transform.c + d * _transform.d;
    double new_e = a * _transform.e + c * _transform.f + e;
    double new_f = b * _transform.e + d * _transform.f + f;

    a = new_a;
    b = new_b;
    c = new_c;
    d = new_d;
    e = new_e;
    f = new_f;
}

void Transform::matrix(double _a, double _b, double _c, double _d, double _e, double _f)
{
    a = _a;
    b = _b;
    c = _c;
    d = _d;
    e = _e;
    f = _f;
}

void Transform::translate(double _t)
{
    e = f = _t;
}

void Transform::translate(double _tx, double _ty)
{
    e = _tx;
    f = _ty;
}

void Transform::scale(double _s)
{
    a = d = _s;
}

void Transform::scale(double _sx, double _sy)
{
    a = _sx;
    d = _sy;
}

void Transform::convert(double &_x, double &_y)
{
    _x = a * _x + c * _y + e;
    _y = b * _x + d * _y + f;
}
