#pragma once

#if !defined(_CAFEL_MDL_HELPER_H_)
#define _CAFEL_MDL_HELPER_H_

#include <cafel/system/element.h>
#include <cafel/basic/color.hpp>

#include <mi/mdl_sdk.h>

CAFEL_NAMESPACE_BEGIN

//对MDL SDK中的目录引用和定义
#define MIHandle mi::base::Handle

template<typename T>
std::string mto_string(T val)
{
	std::ostringstream stream;
	stream << val;
	return stream.str();
}

//execute返回的结果结构体
union MDL_Result
{
	int                     int_val;
	float                   float_val;
	double                  double_val;
	mi::Float32_3_struct    float3_val;
	mi::Float32_4_struct    float4_val;
	mi::Float32_4_4_struct  float4x4_val;
	mi::Float64_3_struct    double3_val;
	mi::Float64_4_struct    double4_val;
	mi::Float64_4_4_struct  double4x4_val;
};

class Target_resource_callback : public
	mi::base::Interface_implement<mi::neuraylib::ITarget_resource_callback>
{
public:
	Target_resource_callback()
	{
	}

	mi::Uint32 get_resource_index(mi::neuraylib::IValue_resource const *resource) {
		return 0;
	}

	mi::Uint32 get_string_index(mi::neuraylib::IValue_string const *s) {
		return 0;
	}
};

class MdlHelperApi : public CAFEL_ELEMENT
{
public:
	static MdlHelperApi* get()
	{
		return ins;
	}

	virtual void init(SceneNode *node)
	{
		module_path = node->getParam("path");
		uint32_t texture_num = 1;
		if (node->hasParam("textureNum"))
		{
			std::string v = node->getParam("textureNum");
			texture_num = atoi(v.c_str());
		}

		num_texture_spaces = texture_num + 1;
	}

	virtual ~MdlHelperApi(){}

	uint32_t get_num_texture_spaces() {
		return num_texture_spaces - 1;
	}

	//输入后通过execute获得结果
	virtual inline MDL_Result execute_native(
		const uint32_t index,
		const mi::neuraylib::Shading_state_material& mdl_state,
		const mi::neuraylib::ITarget_argument_block *cap_args = nullptr) = 0;

	//调用execute_environment得到结果，输入参数state和Texture_handler
	virtual inline Color execute_environment(
		const uint32_t index,
		const mi::neuraylib::Shading_state_environment& mdl_state) = 0;

	//调用bsdf的init
	virtual inline void execute_bsdf_init(
		const uint32_t index,
		mi::neuraylib::Shading_state_material& mdl_state,
		const mi::neuraylib::ITarget_argument_block *cap_args = nullptr) = 0;

	//调用bsdf的eval
	virtual inline void execute_bsdf_eval(
		const uint32_t index,
		mi::neuraylib::Bsdf_evaluate_data *eval,
		const mi::neuraylib::Shading_state_material& mdl_state,
		const mi::neuraylib::ITarget_argument_block *cap_args = nullptr) = 0;

	//调用bsdf的pdf
	virtual inline void execute_bsdf_pdf(
		const uint32_t index,
		mi::neuraylib::Bsdf_pdf_data *pdf,
		const mi::neuraylib::Shading_state_material& mdl_state,
		const mi::neuraylib::ITarget_argument_block *cap_args = nullptr) = 0;

	//调用bsdf的sample
	virtual inline void execute_bsdf_sample(
		const uint32_t index,
		mi::neuraylib::Bsdf_sample_data *sample,
		const mi::neuraylib::Shading_state_material& mdl_state,
		const mi::neuraylib::ITarget_argument_block *cap_args = nullptr) = 0;

	virtual uint32_t compile_df(
		const std::string &module_name,
		const std::string &material_name,
		const std::string &expression_path,
		const std::string &res_function_name_base = std::string(),
		bool class_compilation = false,
		bool use_builtin_texture_runtime = false,
		MIHandle<const mi::neuraylib::IExpression_list> arguments = MIHandle<const mi::neuraylib::IExpression_list>()) = 0;

	virtual uint32_t compile_expression(
		const std::string &module_name,
		const std::string &material_name,
		const std::string &expression_path,
		const std::string &res_function_name = std::string(),
		bool class_compilation = false,
		bool use_builtin_texture_runtime = false,
		MIHandle<const mi::neuraylib::IExpression_list> arguments = MIHandle<const mi::neuraylib::IExpression_list>()) = 0;

protected:
	//材质空间
	std::string module_path;
	uint32_t num_texture_spaces;

	MdlHelperApi(){}

private:
	// 在头文件中定义后，应在类的声明之外，重新定义一次。
	static MdlHelperApi* ins;
};

CAFEL_NAMESPACE_END

#endif 