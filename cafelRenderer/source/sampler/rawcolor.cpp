#include <sampler/rawcolor.hpp>

CAFEL_NAMESPACE_BEGIN

void RawColor::setColor(const Color &c_)
{
	color = c_;
}

Color RawColor::getColor()
{
	return color;
}

Color RawColor::eval(double u, double v)
{
	return color;
}

Color RawColor::eval(const dVector2 &i)
{
	return color;
}

Color RawColor::eval(const Intersection &i)
{
	return color;
}

void RawColor::generateCode(std::ostringstream &oss)
{
	oss << "vec3(" << color.r << "," << color.g <<"," <<color.b<<")";
}

CAFEL_NAMESPACE_END