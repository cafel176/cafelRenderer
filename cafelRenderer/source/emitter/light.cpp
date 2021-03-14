#include <emitter/light.hpp>

CAFEL_NAMESPACE_BEGIN

void Light::generateCode(std::ostringstream &oss)
{
	Emitter::generateCode(oss);
	oss << "void main(){" << std::endl
		<< "    FragColor = vec4("<< 1.0 << "," << 1.0 << "," << 1.0 << ",1.0);" << std::endl
		<< "}" << std::endl;
}

CAFEL_NAMESPACE_END