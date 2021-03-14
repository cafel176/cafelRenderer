#include <cafel/utils/warp.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

CAFEL_NAMESPACE_BEGIN

Warp* Warp::ins = nullptr;

Warp::Warp()
{
	std::srand((unsigned)time(NULL));
	stbi_set_flip_vertically_on_load(true); //设置y轴翻转
}

unsigned char* Warp::stbLoad(char const *filename, int *x, int *y, int *comp, int req_comp)
{
	return stbi_load(filename, x, y, comp, req_comp);
}

void Warp::stbImageFree(void* data)
{
	stbi_image_free(data);
}

std::vector<std::string> Warp::split(std::string str, std::string pattern)
{
	int pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

triangleData Warp::intersectTriangle(const Ray& ray, const dVector& norm, const dVector& u, const dVector& v, const dVector& v0)
{
	triangleData data;
	// 计算射线与平面法向夹角
	double b = norm.dot(ray.direction);
	if (fabs(b) < eps)      // ray is parallel to triangle plane   
		return data;

	// 计算v0到射线起始点的向量
	dVector w0 = ray.origin - v0;
	double a = -(norm.dot(w0));

	// get intersect point of ray with triangle plane   
	double t = a / b;
	if (t < eps)                 // ray goes away from triangle   
		return data;                 // => no intersect   
	// for a segment, also test if (r > 1.0) => no intersect   

	// 计算射线与平面的交点
	dVector interPoint = ray.origin + ray.direction * t;

	// 计算交点是否在三角形内部?   
	double uu = u.dot(u);
	double uv = u.dot(v);
	double vv = v.dot(v);
	dVector w = interPoint - v0;
	double wu = w.dot(u);
	double wv = w.dot(v);
	double D = uv * uv - uu * vv;

	// get and test parametric coords   
	double s = (uv * wv - vv * wu) / D;//u
	if (s < 0.0 || s > 1.0)       // I is outside T   
		return data;

	double tt = (uv * wu - uu * wv) / D;//v
	if (tt < 0.0 || (s + tt) > 1.0) // I is outside T   
		return data;

	data.t = t;
	data.u = s;
	data.v = tt;
	return data;   // 交点在三角形内部 
}

void Warp::getTangent(const dVector& u, const dVector& v, const dVector2& uv10, const dVector2& uv20, const dVector& normal, dVector& tangent, dVector& bitangent)
{
	double du1 = uv10.x;
	double dv1 = uv10.y;
	double du2 = uv20.x;
	double dv2 = uv20.y;
	double f = 1.0 / (du1*dv2 - du2 * dv1);

	tangent.x = f * (dv2*u.x - dv1 * v.x);
	tangent.y = f * (dv2*u.y - dv1 * v.y);
	tangent.z = f * (dv2*u.z - dv1 * v.z);
	tangent.norm();

	dVector b = tangent % normal;
	b.norm();
	dVector other = (uv10.y > 0) ? u : u * -1;
	if (other.dot(b) > 0)
		bitangent = b * -1;
	else
		bitangent = b;
}

CAFEL_NAMESPACE_END