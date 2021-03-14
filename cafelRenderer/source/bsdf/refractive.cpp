#include <bsdf/refractive.hpp>
#include <cafel/utils/warp.hpp>

CAFEL_NAMESPACE_BEGIN

Ray Refractive::sample(const Intersection &its, unsigned int depth)
{
	dVector refl = its.ray.direction - its.normal * 2 * its.normal.dot(its.ray.direction);
	bool into = its.normal.dot(its.normal_l) > 0;                // Ray from outside going in?
	double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = its.ray.direction.dot(its.normal_l), cos2t = 1 - nnt * nnt*(1 - ddn * ddn);
	if (cos2t < 0)    // Total internal reflection
	{
		return Ray(its.hitPoint, refl);
	}
	else
	{
		dVector tdir = (its.ray.direction*nnt - its.normal * ((into ? 1 : -1)*(ddn*nnt + sqrt(cos2t)))).norm();
		double a = nt - nc, b = nt + nc, R0 = a * a / (b*b), c = 1 - (into ? -ddn : tdir.dot(its.normal));
		double Re = R0 + (1 - R0)*c*c*c*c*c, Tr = 1 - Re;

		double P = .25 + .5*Re;
		if (Warp::get()->drand() < P)   // Russian roulette
		{
			return Ray(its.hitPoint, refl, Re / P);
		}
		else
		{
			return Ray(its.hitPoint, tdir, Tr / (1 - P));
		}
	}
}

void Refractive::generateCode(std::ostringstream &oss, int lightNum)
{
	BSDF::generateCode(oss, lightNum);
	oss << "void main(){" << std::endl
		<< "    vec3 color = ";
	sampler->generateCode(oss);
	oss << ";" << std::endl;
	oss << "    vec3 normal = normalize(fs_in.Normal);" << std::endl
		<< "    vec3 viewDir = normalize(viewPos - fs_in.FragPos);" << std::endl
		<< "    float diff = 0,spec = 0;" << std::endl
		<< "    vec3 lightColor = vec3(0.3);" << std::endl
		<< "    for (int i = 0; i < NR_POINT_LIGHTS; i++)" << std::endl
		<< "    {" << std::endl
		<< "        vec3 lightDir = normalize(lightPos[i] - fs_in.FragPos);" << std::endl
		<< "        diff += max(dot(lightDir, normal), 0.0);" << std::endl
		<< "        vec3 halfwayDir = normalize(lightDir + viewDir);" << std::endl
		<< "        spec += pow(max(dot(normal, halfwayDir), 0.0), 64.0);" << std::endl
		<< "    }" << std::endl
		<< "    vec3 result = diff * color + spec * lightColor;" << std::endl
		<< "    FragColor = vec4(result, 0.5);" << std::endl
		<< "}" << std::endl;
}

CAFEL_NAMESPACE_END