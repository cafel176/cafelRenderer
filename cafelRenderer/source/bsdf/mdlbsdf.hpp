#pragma once

#if !defined(_CAFEL_MDL_BSDF_HPP_)
#define _CAFEL_MDL_BSDF_HPP_

#include <cafel/components/bsdf.h>
#include <cafel/object/mdlHelper.h>

CAFEL_NAMESPACE_BEGIN

class MdlBSDF : public BSDF
{
public:
	MdlBSDF(SceneNode *node) :BSDF(node) 
	{
		m_module_name = node->getChildFirst("moduleName")->getParam("value");
		m_material_name = node->getChildFirst("materialName")->getParam("value");
		m_df_path = node->getChildFirst("dfPath")->getParam("value");

		if (node->hasChild("classCompilation"))
			class_compilation = true;
		else
			class_compilation = false;

		if (node->hasChild("textureRuntime"))
			use_texture_runtime = true;
		else
			use_texture_runtime = false;

		if (node->hasChild("ior1"))
		{
			std::string v = node->getChildFirst("ior1")->getParam("value");
			std::vector<std::string> vec = Warp::get()->split(v, ",");
			ior1 = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		}
		else
			ior1 = dVector(1.0, 1.0, 1.0);

		if (node->hasChild("ior2"))
		{
			std::string v = node->getChildFirst("ior2")->getParam("value");
			std::vector<std::string> vec = Warp::get()->split(v, ",");
			ior2 = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		}
		else
			ior2 = dVector(-1.0, 0.0, 0.0);

		if (node->hasChild("functionName"))
			res_function_name = node->getChildFirst("functionName")->getParam("value");

		num_tex_spaces = MdlHelperApi::get()->get_num_texture_spaces();
		index = MdlHelperApi::get()->compile_df(m_module_name, m_material_name, m_df_path, res_function_name, class_compilation, use_texture_runtime);
	}
	MdlBSDF(std::shared_ptr<Sampler> sampler_) :BSDF(sampler_) {}

	Color eval(const Intersection &i, unsigned int depth);
	Ray sample(const Intersection &i, unsigned int depth);
	std::string tostring() const { return "MdlBSDF:(" + sampler->tostring() + ")"; }
	void generateCode(std::ostringstream &oss, int lightNum);
private:
	std::string m_module_name, m_material_name, m_df_path, res_function_name;

	//控制是否使用运行时材质而不是内置的材质
	bool use_texture_runtime, class_compilation;

	uint32_t index, num_tex_spaces;

	dVector ior1, ior2;
};

CAFEL_NAMESPACE_END

#endif 