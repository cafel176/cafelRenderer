#pragma once

#if !defined(_CAFEL_MDL_HELPER_HPP_)
#define _CAFEL_MDL_HELPER_HPP_

#include <cafel/object/mdlHelper.h>
#include <mi/texture_support.h>
#include <mi/neuray_loader.h>
#include <vector>

CAFEL_NAMESPACE_BEGIN

using namespace mi::neuraylib;

class MdlHelper : public MdlHelperApi {
public:

	// ������ʧ��ʱ�׳�һ������������Ϣ���쳣
	static void check_success(bool expr, std::string errMsg)
	{
		if (expr) return;
		printf(errMsg.c_str());
	}

	// ������ʧ��ʱ�׳�һ������������Ϣ���쳣������ʾ������ʾ��ֵ
	static void check_success(mi::Sint32 errVal, std::string errMsg)
	{
		if (errVal >= 0) return;

		printf((errMsg + "(" + mto_string(errVal) + ")").c_str());
	}

	//���캯����ʵ��
	MdlHelper(){}

	//����������ʵ��
	~MdlHelper();

	void init(SceneNode *node);

	//�����������������
	class Mdl_compile_result
	{
	public:
		Mdl_compile_result(
			MdlHelper &mdl_helper)
			: m_mdl_helper(mdl_helper)
		{
		}

		~Mdl_compile_result() {

		}

		// ����ģ�����ͱ���õĲ��ʸ������
		void set_compiled_material(std::string module_name, MIHandle<const ICompiled_material> compiled_material)
		{
			m_module_name = module_name;
			m_compiled_material = compiled_material;
		}

		// ���ؽ����ʹ�õı���õĲ���
		MIHandle<const ICompiled_material> get_compiled_material() const
		{
			return m_compiled_material;
		}

		//����MDL helper.
		MdlHelper *get_mdl_helper() const { return &m_mdl_helper; }

		//�������������õ���ģ����
		std::string get_module_name() const { return m_module_name; }

		//����һ�������Ĳ��ʵı��ʽ
		void compile_expression(
			const std::string &expression_path,
			const std::string &res_function_name = std::string(),
			bool class_compilation = false,
			bool use_builtin_texture_runtime = false);

		//����һ��df����
		void compile_df(
			const std::string &expression_path,
			const std::string &res_function_name_base = std::string(),
			bool class_compilation = false,
			bool use_builtin_texture_runtime = false);

		//�����ͨ��execute��ñ��ʽ���
		inline MDL_Result execute_native(
			const Shading_state_material& mdl_state,
			const ITarget_argument_block *cap_args = nullptr)
		{
			MDL_Result execute_result = { 0 };
			check_success(m_expression_code.get()->execute(0, mdl_state, tex_handler_ptr, cap_args, &execute_result) == 0,
				"Execute expression fail");
			return execute_result;
		}

		//����bsdf��init����
		inline void execute_bsdf_init(
			Shading_state_material& mdl_state,
			const ITarget_argument_block *cap_args = nullptr)
		{
			check_success(m_bsdf_code.get()->execute_bsdf_init(0, mdl_state, tex_handler_ptr, cap_args) == 0,
				"Execute bsdf init fail");
		}

		//����bsdf��eval����
		inline void execute_bsdf_eval(
			Bsdf_evaluate_data *eval,
			const Shading_state_material& mdl_state,
			const ITarget_argument_block *cap_args = nullptr)
		{
			check_success(m_bsdf_code.get()->execute_bsdf_evaluate(2, eval, mdl_state, tex_handler_ptr, cap_args) == 0,
				"Execute bsdf eval fail");
		}

		//����bsdf��pdf����
		inline void execute_bsdf_pdf(
			Bsdf_pdf_data *pdf,
			const Shading_state_material& mdl_state,
			const ITarget_argument_block *cap_args = nullptr)
		{
			check_success(m_bsdf_code.get()->execute_bsdf_pdf(3, pdf, mdl_state, tex_handler_ptr, cap_args) == 0,
				"Execute bsdf pdf fail");
		}

		//����bsdf��sample����
		inline void execute_bsdf_sample(
			Bsdf_sample_data *sample,
			const Shading_state_material& mdl_state,
			const ITarget_argument_block *cap_args = nullptr)
		{
			check_success(m_bsdf_code.get()->execute_bsdf_sample(1, sample, mdl_state, tex_handler_ptr, cap_args) == 0,
				"Execute bsdf sample fail");
		}

		inline ITarget_argument_block* get_expression_block()
		{
			ITarget_resource_callback *func = new Target_resource_callback();
			if (m_expression_code->get_argument_block_count() == 0)
				return m_expression_code->create_argument_block(0, m_compiled_material.get(), func);
			else
				return m_expression_code->get_argument_block(0)->clone();
		}

		inline ITarget_argument_block* get_bsdf_block()
		{
			ITarget_resource_callback *func = new Target_resource_callback();
			if (m_bsdf_code->get_argument_block_count() == 0)
				return m_bsdf_code->create_argument_block(0, m_compiled_material.get(), func);
			else
				return m_bsdf_code->get_argument_block(0)->clone();
		}

		inline void set_expression_argument(uint32_t parameter_index, IValue *value, ITarget_argument_block *block)
		{
			ITarget_resource_callback *func = new Target_resource_callback();
			if (parameter_index >= 0)
			{
				MIHandle<const ITarget_value_layout> layout(m_expression_code->get_argument_block_layout(0));
				check_success(layout->set_value(block->get_data(), value, func, layout->get_nested_state(parameter_index)),
					"Set expression argument fail");
			}
		}

		inline void set_bsdf_argument(uint32_t parameter_index, IValue *value, ITarget_argument_block* block)
		{
			ITarget_resource_callback *func = new Target_resource_callback();
			if (parameter_index >= 0)
			{
				MIHandle<const ITarget_value_layout> layout(m_bsdf_code->get_argument_block_layout(0));
				check_success(layout->set_value(block->get_data(), value, func, layout->get_nested_state(parameter_index)),
					"Set expression argument fail");
			}
		}

		uint32_t get_parameter_index(std::string parameter_name)
		{
			uint32_t num = m_compiled_material->get_parameter_count();
			for (uint32_t i = 0; i < num; i++)
			{
				std::string txt = m_compiled_material->get_parameter_name(i);
				if (txt == parameter_name)
				{
					return i;
				}
			}
		}

		const IValue* get_parameter(uint32_t parameter_index) {
			return m_compiled_material->get_argument(parameter_index);
		}

	private:

		//MDL helper.
		MdlHelper &m_mdl_helper;

		//�õ���ģ����
		std::string m_module_name;

		//����õ���ͼ����ָ��
		Texture_handler *tex_handler_ptr;

		/// The compiled material.
		MIHandle<const ICompiled_material> m_compiled_material;

		/// The expression factory
		MIHandle<const IExpression_factory> m_expr_factory;

		//���������������code
		MIHandle<const ITarget_code> m_expression_code, m_bsdf_code;
	};

	MIHandle<IValue_factory> getValueFactory() {
		MIHandle<IValue_factory> value_factory(
			m_mdl_factory->create_value_factory(m_transaction.get()));
		return value_factory;
	}

	inline ITarget_argument_block* get_expression_block(const uint32_t index) {
		return compile_result_vec[index]->get_expression_block();
	}

	inline ITarget_argument_block* get_bsdf_block(const uint32_t index) {
		return compile_result_vec[index]->get_bsdf_block();
	}

	inline void set_expression_argument(const uint32_t index, const uint32_t parameter_index,
		IValue *value, ITarget_argument_block* block) {
		return compile_result_vec[index]->set_expression_argument(parameter_index, value, block);
	}

	inline void set_bsdf_argument(const uint32_t index, const uint32_t parameter_index,
		IValue *value, ITarget_argument_block* block) {
		compile_result_vec[index]->set_bsdf_argument(parameter_index, value, block);
	}

	uint32_t get_parameter_index(const uint32_t index, std::string parameter_name) {
		return compile_result_vec[index]->get_parameter_index(parameter_name);
	}

	const IValue* get_parameter(const uint32_t index, const uint32_t parameter_index) {
		return compile_result_vec[index]->get_parameter(parameter_index);
	}

	//�������·������ѰMDLģ�����Դ
	void add_module_path(std::string &module_path)
	{
		uint32_t length = m_mdl_compiler->get_module_paths_length();
		bool exist = false;

		for (uint32_t i = 0; i < length; i++) {
			if (strcmp(m_mdl_compiler->get_module_path(i)->get_c_str(), module_path.c_str()) == 0) {
				exist = true; break;
			}
		}
		if (!exist)
			check_success(m_mdl_compiler->add_module_path(module_path.c_str()),
				"Adding module path failed!");
	}

	//���Ӳ��ʿռ���
	void add_texture_space(const uint32_t add_texture_num) {
		num_texture_spaces += add_texture_num;
		check_success(m_be_native->set_option(
			"num_texture_spaces", mto_string(num_texture_spaces).c_str()) == 0,
			"Setting option num_texture_spaces failed");
	}

	// ������ʽ������module����material����expression·����res_function���Ͳ���
	uint32_t compile_expression(
		const std::string &module_name,
		const std::string &material_name,
		const std::string &expression_path,
		const std::string &res_function_name = std::string(),
		bool class_compilation = false,
		bool use_builtin_texture_runtime = false,
		MIHandle<const IExpression_list> arguments = MIHandle<const IExpression_list>())
	{
		Mdl_compile_result *expression_compile = new Mdl_compile_result(compile(module_name, material_name, class_compilation, arguments));
		expression_compile->compile_expression(expression_path, res_function_name, class_compilation, use_builtin_texture_runtime);
		compile_result_vec.push_back(expression_compile);
		return compile_result_vec.size() - 1;
	}

	// ����environment��ͨ��module����function����res_function���Ͳ���
	uint32_t compile_environment(
		const std::string &module_name,
		const std::string &function_name,
		bool use_builtin_texture_runtime = false,
		const std::string &res_function_name = std::string(),
		MIHandle<const IExpression_list> arguments = MIHandle<const IExpression_list>());

	// ����bsdf������module����material����expression·����res_function���Ͳ���
	uint32_t compile_df(
		const std::string &module_name,
		const std::string &material_name,
		const std::string &expression_path,
		const std::string &res_function_name_base = std::string(),
		bool class_compilation = false,
		bool use_builtin_texture_runtime = false,
		MIHandle<const IExpression_list> arguments = MIHandle<const IExpression_list>())
	{
		Mdl_compile_result *bsdf_compile = new Mdl_compile_result(compile(module_name, material_name, class_compilation, arguments));
		bsdf_compile->compile_df(expression_path, res_function_name_base, class_compilation, use_builtin_texture_runtime);
		compile_result_vec.push_back(bsdf_compile);
		return compile_result_vec.size() - 1;
	}

	//�����ͨ��execute��ý��
	inline MDL_Result execute_native(
		const uint32_t index,
		const Shading_state_material& mdl_state,
		const ITarget_argument_block *cap_args = nullptr)
	{
		return compile_result_vec[index]->execute_native(mdl_state, cap_args);
	}

	//����execute_environment�õ�������������state��Texture_handler
	inline Color execute_environment(
		const uint32_t index,
		const Shading_state_environment& mdl_state)
	{
		mi::Spectrum_struct execute_result;
		check_success(m_environment_codes[index].get()->execute_environment(0, mdl_state, tex_handler_ptr, &execute_result) == 0,
			"Execute environment fail");
		Color result(execute_result.c[0], execute_result.c[1], execute_result.c[2]);
		return result;
	}

	//����bsdf��init
	inline void execute_bsdf_init(
		const uint32_t index,
		Shading_state_material& mdl_state,
		const ITarget_argument_block *cap_args = nullptr)
	{
		compile_result_vec[index]->execute_bsdf_init(mdl_state, cap_args);
	}

	//����bsdf��eval
	inline void execute_bsdf_eval(
		const uint32_t index,
		Bsdf_evaluate_data *eval,
		const Shading_state_material& mdl_state,
		const ITarget_argument_block *cap_args = nullptr)
	{
		compile_result_vec[index]->execute_bsdf_eval(eval, mdl_state, cap_args);
	}

	//����bsdf��pdf
	inline void execute_bsdf_pdf(
		const uint32_t index,
		Bsdf_pdf_data *pdf,
		const Shading_state_material& mdl_state,
		const ITarget_argument_block *cap_args = nullptr)
	{
		compile_result_vec[index]->execute_bsdf_pdf(pdf, mdl_state, cap_args);
	}

	//����bsdf��sample
	inline void execute_bsdf_sample(
		const uint32_t index,
		Bsdf_sample_data *sample,
		const Shading_state_material& mdl_state,
		const ITarget_argument_block *cap_args = nullptr)
	{
		compile_result_vec[index]->execute_bsdf_sample(sample, mdl_state, cap_args);
	}

protected:

	//������һ���ܱ�������transaction���������ַ�������
	std::string get_next_unique_dbname()
	{
		return "_mdlhelper_" + mto_string(m_next_name_id++);
	}

	//����module
	void load_module(
		const std::string &module_name)
	{
		check_success(m_mdl_compiler->load_module(m_transaction.get(), module_name.c_str()),
			"Loading " + module_name + ".mdl failed!");
	}

	//���벢�õ�compile_result
	Mdl_compile_result compile(
		const std::string &module_name,
		const std::string &material_name,
		bool class_compilation = false,
		MIHandle<const IExpression_list> arguments = MIHandle<const IExpression_list>());

	//ʹ������ʱ����ʱ������ͼ���е���
	bool prepare_textures(
		std::vector<mdlTexture>& textures,//�������������������룬Ҳ�������
		const ITarget_code* target_code);

	//����õ���ͼ����ָ��
	Texture_handler *tex_handler_ptr = nullptr;

	//�洢��������vector
	std::vector<Mdl_compile_result*> compile_result_vec;

	//�洢environment�ı���code
	std::vector<MIHandle<const ITarget_code>> m_environment_codes;

	/// The Neuray interface of the MDL SDK.
	MIHandle<INeuray> m_neuray;

	/// The MDL compiler.
	MIHandle<IMdl_compiler> m_mdl_compiler;

	/// The Neuray database of the MDL SDK.
	MIHandle<IDatabase> m_database;

	/// The global scope of the data base used to create transactions.
	MIHandle<IScope> m_global_scope;

	MIHandle<ITransaction> m_transaction;

	/// The MDL factory.
	MIHandle<IMdl_factory> m_mdl_factory;

	/// The CUDA PTX backend of the MDL compiler.
	MIHandle<IMdl_backend> m_be_native;

	/// The Image API for converting image to other formats.
	MIHandle<IImage_api> m_image_api;

	/// The next ID to use for unique database names.
	int m_next_name_id;
	};

CAFEL_NAMESPACE_END

#endif 