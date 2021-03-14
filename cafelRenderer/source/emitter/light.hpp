#pragma once

#if !defined(_CAFEL_LIGHT_HPP_)
#define _CAFEL_LIGHT_HPP_

#include <cafel/components/emitter.h>

CAFEL_NAMESPACE_BEGIN

class Light : public Emitter
{
public:
	Light(const Color &c_):Emitter(c_){}
	Light(SceneNode *node) :Emitter(node) {}
	void generateCode(std::ostringstream &oss);
};

CAFEL_NAMESPACE_END

#endif 