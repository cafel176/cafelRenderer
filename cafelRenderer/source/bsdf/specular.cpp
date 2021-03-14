#include <bsdf/specular.hpp>

CAFEL_NAMESPACE_BEGIN

Ray Specular::sample(const Intersection &i, unsigned int depeth)
{
	dVector d = i.ray.direction - i.normal * 2 * i.normal.dot(i.ray.direction);

	return Ray(i.hitPoint, d);
}

void Specular::generateCode(std::ostringstream &oss, int lightNum)
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
		<< "    FragColor = vec4(result, 1.0);" << std::endl
		<< "}" << std::endl;
}

CAFEL_NAMESPACE_END