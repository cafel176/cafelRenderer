#pragma once

#if !defined(_CAFEL_OBJECT_API_H_)
#define _CAFEL_OBJECT_API_H_

#include <cafel/basic/interaction.hpp>
#include <cafel/components/bsdf.h>
#include <cafel/components/emitter.h>
#include <cafel/components/shape.h>
#include <cafel/utils/warp.hpp>
#include <factory/bsdffactory.hpp>
#include <factory/emitterfactory.hpp>
#include <factory/shapefactory.hpp>

#include <sampler/texture.hpp>

CAFEL_NAMESPACE_BEGIN

class ObjectApi : public CAFEL_ELEMENT
{
public:
	ObjectApi(SceneNode *node):Element(node) 
	{
		std::string p = node->getChildFirst("position")->getParam("value");
		std::vector<std::string> vec = Warp::get()->split(p, ",");
		position = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		
		if (node->hasChild("rotation"))
		{
			std::string r = node->getChildFirst("rotation")->getParam("value");
			vec = Warp::get()->split(r, ",");
			rotation = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		}
		else
			rotation = Vector_zero(double,3);
		if (node->hasChild("scale"))
		{
			std::string s = node->getChildFirst("scale")->getParam("value");
			vec = Warp::get()->split(s, ",");
			scale = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		}
		else
			scale = Vector_one(double, 3);

		SceneNode* n = node->getChildFirst("shape");
		shape = std::shared_ptr<Shape>(static_cast<Shape*>(ShapeFactory::get(n)));
		n = node->getChildFirst("bsdf");
		bsdf = std::shared_ptr<BSDF>(static_cast<BSDF*>(BSDFFactory::get(n)));

		if (node->hasChild("emitter"))
		{
			n = node->getChildFirst("emitter");
			emitter = std::shared_ptr<Emitter>(static_cast<Emitter*>(EmitterFactory::get(n)));
		}
	}

	ObjectApi(const dVector &position_, const dVector &rotation_ = Vector_zero(double, 3), const dVector &scale_ = Vector_one(double, 3))
	{
		position = position_; rotation = rotation_; scale = scale_;
	}
	virtual ~ObjectApi() {}

	void setShape(std::shared_ptr<Shape> shape_){shape = shape_;}
	void setBSDF(std::shared_ptr<BSDF> bsdf_) { bsdf = bsdf_; }
	void setEmitter(std::shared_ptr<Emitter> emitter_) { emitter = emitter_; }
	bool hasEmitter() { if (emitter != nullptr) return true; else return false; }

	virtual Intersection intersect(const Ray &r) = 0;
	virtual Interaction interact(Intersection &i, unsigned int depth) const = 0;
	virtual Color emitterEval(const Intersection &i) const = 0;
	virtual Color bsdfEval(const Intersection &i, unsigned int depth) const = 0;
	virtual dVector getRealPosition() const = 0;
	virtual dVector getRealScale() const = 0;
	virtual dVector getMin() const = 0;
	virtual dVector getMax() const = 0;

	const float* getTriangles(unsigned int &num, unsigned int &size) const { return shape->getTriangles(num, size); }
	const unsigned int* getIndices(unsigned int &num, unsigned int &size) const { return shape->getIndices(num, size); }

	virtual std::vector<Texture> getTextures() const
	{
		std::vector<Texture> vs;

		std::shared_ptr<Model> m = std::dynamic_pointer_cast<Model>(shape);
		if (m != nullptr)
			return m->getTextures();
		else
		{
			std::shared_ptr<TextureSampler> s = std::dynamic_pointer_cast<TextureSampler>(bsdf->getSampler());
			if (s != nullptr)
			{
				vs.push_back(s->getTexture());
			}
		}

		return vs;
	}

	virtual void setTextureID(unsigned int id, unsigned int index = 0)
	{ 
		std::static_pointer_cast<TextureSampler>(bsdf->getSampler())->setTextureID(id);
	}

	std::string getVertexShader()
	{
		std::ostringstream oss;
		shape->generateCode(oss);
		return oss.str();
	}

	std::string getFragmentShader(int lightNum)
	{
		std::ostringstream oss;
		if (emitter != nullptr)
			emitter->generateCode(oss);
		else if (bsdf != nullptr)
			bsdf->generateCode(oss, lightNum);
		return oss.str();
	}

	virtual std::string tostring() const = 0;
protected:
	dVector position;
	dVector rotation;
	dVector scale;

	std::shared_ptr<Shape> shape;
	std::shared_ptr<BSDF> bsdf;
	std::shared_ptr<Emitter> emitter;
};

CAFEL_NAMESPACE_END

#endif 
