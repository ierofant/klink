#ifndef TRANSFORM_HPP_INCLUDED
#define TRANSFORM_HPP_INCLUDED

#include <glibmm/ustring.h>
#include <cairomm/matrix.h>

class Transform
{
    public:
	Transform() = default;
	Transform(Glib::ustring const &_str);

    public:
	Glib::ustring str() const;
//	Cairo::Matrix get_matrix() const;
	void get_translate(double &_tx, double &_ty) const;
	void read(Glib::ustring const &_str);
	void reset();
	void merge(Transform const &_transform);
	void matrix(double _a, double _b, double _c, double _d, double _e, double _f);
	void translate(double _t);
	void translate(double _tx, double _ty);
	void scale(double _s);
	void scale(double _sx, double _sy);

    private:
	double a = 1, b = 0, c = 0 , d = 0, e = 1, f = 0;
};

#endif //TRANSFORM_HPP_INCLUDED
