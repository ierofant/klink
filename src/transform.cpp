#include <locale>
#include <algorithm>
#include "transform.hpp"

Transform::Transform(Glib::ustring const &_str)
{
    read(_str);
}

Glib::ustring Transform::str() const
{
    std::ostringstream out;
    if(b == 0 && d == 0)
    {
	if(c != 0 && f != 0) 
	{
	    out << "translate(" << c;
	    if(c != f) out << ", " << f;
	    out << ")";
	}
	if(a != 1 && e != 0)
	{
	    if(!out.str().empty()) out << ' ';
	    out << "scale(" << a;
	    if(a != e)  out << ", " << e;
	    out << ")";
	}
    }
    else out << "matrix(" << a << ", " << b << ", " << c << ", " << d << ", " << e << ", " << f << ")";
    return out.str(); 
}

void Transform::get_translate(double &_tx, double &_ty) const
{
    _tx = c;
    _ty = f;
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

		if(name == "mairix")
	        {    
		    char sym;
	    	    double a, b, c, d, e, f;
	    	    args >> a >> sym >> b >> sym >> c >> sym >> e >> sym >> f; 
		}
		else if(name == "translate" || name =="scale")
		{
		    char sym;
	    	    args >> c;
	    	    if(args.good()) args >> sym >> f;
	    	    else f = c;
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
    d = 0;
    e = 1;
    f = 0;
}

void Transform::merge(Transform const &_transform)
{
    a = a * _transform.a;
    b = b + _transform.b;
    c = c + _transform.c;
    d = d + _transform.d;
    e = e * _transform.e;
    f = f + _transform.f;
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
    c = f = _t;
}

void Transform::translate(double _tx, double _ty)
{
    c = _tx;
    f = _ty;
}

void Transform::scale(double _s)
{
    a = e = _s;
}

void Transform::scale(double _sx, double _sy)
{
    a = _sx;
    e = _sy;
}
