#pragma once

#if !defined(_CAFEL_WARP_HPP_)
#define _CAFEL_WARP_HPP_

#include <cafel/basic/ray.hpp>

#include <stdlib.h>
#include <time.h> 
#include <vector>

CAFEL_NAMESPACE_BEGIN

struct triangleData
{
	double t = t_nohit;
	double u;
	double v;
};

class Warp
{
public:
	static Warp* get()
	{
		if (ins == nullptr)
			ins = new Warp();
		return ins;
	}

	double drand()
	{
		return std::rand() / double(RAND_MAX);
	}

	int rand(int number)
	{
		return std::rand() % number;
	}

	dVector2 sampleCosineHemisphere()
	{
		double r1 = 2 * M_PI*drand(), r2 = drand();
		return dVector2(r1, r2);
	}

	inline dVector minVec(const dVector &a, const dVector &b)
	{
		return dVector(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z), fmin(a.w, b.w));
	}

	inline dVector maxVec(const dVector &a, const dVector &b)
	{	
		return dVector(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z), fmax(a.w, b.w));
	}

	inline double clamp(double x) { return x < 0 ? 0 : x>1 ? 1 : x; }

	inline int rgbToInt(double x) { return int(pow(clamp(x), 1 / 2.2) * 255 + .5); }

	double static radians(double degree) { return degree * 0.01745329251994329576923690768489; }
	
	double static degree(double radians) { return radians / 0.01745329251994329576923690768489; }

	triangleData Warp::intersectTriangle(const Ray& ray, const dVector& norm, const dVector& u, const dVector& v, const dVector& v0);

	void getTangent(const dVector& u, const dVector& v, const dVector2& uv10, const dVector2& uv20, const dVector& normal, dVector& tangent, dVector& bitangent);

	std::vector<std::string> split(std::string str, std::string pattern);

	unsigned char* stbLoad(char const *filename, int *x, int *y, int *comp, int req_comp);

	void stbImageFree(void* data);

private:
	// 在头文件中定义后，应在类的声明之外，重新定义一次。
	static Warp* ins;

	Warp();
};

CAFEL_NAMESPACE_END

#endif 