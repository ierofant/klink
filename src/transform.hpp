#ifndef TRANSFORM_HPP_INCLUDED
#define TRANSFORM_HPP_INCLUDED

#include <glibmm/ustring.h>
#include <cairomm/matrix.h>
#include <libxml++/nodes/element.h>

class Transform
{
    public:
	Transform() = default;
	Transform(Glib::ustring const &_str);
	Transform(xmlpp::Element const &_element, bool _inherit = true);

    public:
	Glib::ustring str() const;
	void get_translate(double &_tx, double &_ty) const;
	void get_scale(double &_tx, double &_ty) const;
	Cairo::Matrix get_matrix() const;
	void read(Glib::ustring const &_str);
	void reset();
	void merge(Transform const &_transform);
	void matrix(double _a, double _b, double _c, double _d, double _e, double _f);
	void translate(double _t);
	void translate(double _tx, double _ty);
	void scale(double _s);
	void scale(double _sx, double _sy);
	void convert(double &_x, double &_y);

    private:
	double a = 1, b = 0, c = 0 , d = 1, e = 0, f = 0;
};

#endif //TRANSFORM_HPP_INCLUDED
