#include <object/camera.hpp>
#include <cafel/utils/warp.hpp>
#include <stdlib.h> // Make : g++ -O3 -fopenmp smallpt.cpp -o smallpt 
#include <stdio.h> // Remove "-fopenmp" for g++ version < 4.2 

CAFEL_NAMESPACE_BEGIN

void printProcess(double num)
{
	printf("\rRendering %5.2f%%", num);
}

Color Camera::sample(const std::shared_ptr<Integrator> integrator, const std::shared_ptr<Tree> tree, int x, int y, int sx, int sy)
{
	Color r = Color_zero;
	for (int s = 0; s < samples; s++)
	{
		double r1 = subpixelX * Warp::get()->drand(), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(subpixelX - r1);
		double r2 = subpixelY * Warp::get()->drand(), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(subpixelY - r2);
		dVector d =  cx * (((sx + .5 + dx) / subpixelX + x) / width - .5) +
			cy * (((sy + .5 + dy) / subpixelY + y) / height - .5) + cz;
		r = r + integrator->radiance(Ray(position + d, d.norm()), tree)*(1. / samples);
	}
	return r;
}

Color Camera::subPixel(const std::shared_ptr<Integrator> integrator, const std::shared_ptr<Tree> tree, int x, int y)
{
	Color color = Color_zero;
	for (int sy = 0; sy < subpixelY; sy++) // 2x2 subpixel rows
	{
		for (int sx = 0; sx < subpixelX; sx++)
		{
			Color c = sample(integrator, tree, x, y, sx, sy);
			color = color + Color(Warp::get()->clamp(c.r), Warp::get()->clamp(c.g), Warp::get()->clamp(c.b))*(1.0 / (subpixelX*subpixelY));
		}
	}
	return color;
}

void Camera::render(const std::shared_ptr<Integrator> integrator, const std::shared_ptr<Tree> tree, OUT_PROCESS out)
{
	const int a[]{ -1,1 };
	const int half = height / 2;
	#pragma omp parallel for schedule(dynamic,1)      // OpenMP
	for (int y = 0; y <= half; y++)
	{   // Loop over image rows
		for (int u = 0; u < ((y==0||y+half>=height)?1:2); u++)
		{
			int now = half + y*a[u];
			for (int x = 0; x < width; x++)   // Loop cols
			{
				Color color = subPixel(integrator, tree, x, now);

				int i = ((height - now - 1) * width + x) * 3;
				data[i] = Warp::get()->rgbToInt(color.r);
				data[i + 1] = Warp::get()->rgbToInt(color.g);
				data[i + 2] = Warp::get()->rgbToInt(color.b);
			}
			if (out == nullptr)
				printProcess(100.*now / (height - 1));
			else
				out(100.*now / (height - 1), data);
		}
	}
}

CAFEL_NAMESPACE_END