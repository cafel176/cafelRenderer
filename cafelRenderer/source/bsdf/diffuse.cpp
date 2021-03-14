#include <bsdf/diffuse.hpp>
#include <cafel/utils/warp.hpp>

CAFEL_NAMESPACE_BEGIN

Ray Diffuse::sample(const Intersection &i, unsigned int depeth)
{
	dVector2 r = Warp::get()->sampleCosineHemisphere();
	double r1 = r.x, r2 = r.y, r2s = sqrt(r2);

	dVector u = ((fabs(i.normal_l.x) > .1 ? dVector(0, 1) : dVector(1)) % i.normal_l).norm(), v = i.normal_l % u;
	dVector d = (u*cos(r1)*r2s + v * sin(r1)*r2s + i.normal_l * sqrt(1 - r2)).norm();

	return Ray(i.hitPoint, d);
}

void Diffuse::generateCode(std::ostringstream &oss, int lightNum)
{
	BSDF::generateCode(oss, lightNum);
	oss << "void main(){" << std::endl
		<< "    vec3 color = ";
	sampler->generateCode(oss);
	oss << ";" << std::endl;
	oss	<< "    vec3 normal = normalize(fs_in.Normal);" << std::endl
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
		<< "    FragColor = vec4(result, 1.0);" << std::endl
		<< "}" << std::endl;
}

CAFEL_NAMESPACE_END