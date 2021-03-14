#pragma once

#if !defined(_CAFEL_TEXTURE_HPP_)
#define _CAFEL_TEXTURE_HPP_

#include <cafel/components/sampler.h>
#include <cafel/utils/warp.hpp>
#include <cstdlib>

CAFEL_NAMESPACE_BEGIN

struct Texture
{
	unsigned int id = 0;
	std::string type;
	std::string path;
	int width, height, nrComponents;
	unsigned char *data;
};

class TextureSampler : public Sampler
{
public:
	// 构造和析构函数
	TextureSampler() {}
	TextureSampler(const Texture &c_) : texture(c_) {}
	TextureSampler(SceneNode *node) : Sampler(node)
	{
		texture.path = node->getChildFirst("path")->getParam("value");
		if (node->hasChild("type"))
		{
			texture.type = node->getChildFirst("type")->getParam("value");
		}
		else
			texture.type = "texture_diffuse";

		texture.data = Warp::get()->stbLoad(texture.path.c_str(), &texture.width, &texture.height, &texture.nrComponents, 0);
	}

	virtual ~TextureSampler(){ Warp::get()->stbImageFree(texture.data); }

	//继承函数
	Color eval(double u, double v);
	Color eval(const dVector2 &i);
	Color eval(const Intersection &i);

	// 独有函数
	Texture getTexture() const{return texture;}
	void setTextureID(unsigned int id){texture.id = id;}

	std::string tostring() const { return "Texture:()"; }
	void generateCode(std::ostringstream &oss);

protected:
	Texture texture;		
};

CAFEL_NAMESPACE_END

#endif 