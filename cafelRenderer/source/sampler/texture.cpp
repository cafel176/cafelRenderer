#include <sampler/texture.hpp>

CAFEL_NAMESPACE_BEGIN

Color TextureSampler::eval(double u, double v)
{
	int index = (int(v * texture.height) + u) * texture.width;
	Color c(
		texture.data[index*texture.nrComponents] / 255.0,
		texture.data[index*texture.nrComponents + 1] / 255.0,
		texture.data[index*texture.nrComponents + 2] / 255.0);
	return c;
}

Color TextureSampler::eval(const dVector2 &uv)
{
	double u = uv.x;
	double v = uv.y;
	int index = (int(v * texture.height) + u) * texture.width;
	Color c(
		texture.data[index*texture.nrComponents] / 255.0,
		texture.data[index*texture.nrComponents + 1] / 255.0,
		texture.data[index*texture.nrComponents + 2] / 255.0);
	return c;
}

Color TextureSampler::eval(const Intersection &its)
{
	double u = its.uv.x;
	double v = its.uv.y;
	int index = (int(v * texture.height) + u) * texture.width;
	Color c(
		texture.data[index*texture.nrComponents]/255.0,
		texture.data[index*texture.nrComponents + 1] / 255.0,
		texture.data[index*texture.nrComponents + 2] / 255.0);
	return c;
}

void TextureSampler::generateCode(std::ostringstream &oss)
{
	oss << "texture(sampler, fs_in.TexCoords).rgb";
}

CAFEL_NAMESPACE_END