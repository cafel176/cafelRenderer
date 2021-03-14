#include <integrators/path.hpp>

CAFEL_NAMESPACE_BEGIN

Color Path::radiance(const Ray &r, const std::shared_ptr<Tree> tree)
{
	Ray ray = r;
	std::vector<Color> emissions, colors;

	for(unsigned int depth = 0;depth < maxDepth;depth++)
	{
		std::shared_ptr<ObjectApi> nowObj;
		Intersection its = tree->intersect(nowObj, ray);
		if (abs(its.t - t_nohit) < eps)
		{
			emissions.push_back(Color_zero);
			colors.push_back(Color_zero);
			break;
		}

		Interaction itr = tree->interact(nowObj, its, depth);

		Color f = itr.color;
		double p = f.getMax();
		if (depth > 5)
			if (Warp::get()->drand() < p)
				f = f * (1. / p);
			else
			{
				emissions.push_back(itr.emission);
				colors.push_back(Color_zero);
				break;
			}

		emissions.push_back(itr.emission);
		colors.push_back(f * ray.priority);
		ray = itr.ray;
	}

	Color re = Color_one;
	for (int i = colors.size()-1; i >= 0 ; i--)
	{
		re = emissions.at(i) + colors.at(i) * re;
	}

	return re;
}

CAFEL_NAMESPACE_END