#include <object/mdlHelper.hpp>
#include <cafel/utils/warp.hpp>

CAFEL_NAMESPACE_BEGIN

MdlHelperApi* MdlHelperApi::ins = new MdlHelper();

//编译一个关联的材质的表达式
void MdlHelper::Mdl_compile_result::compile_expression(
	const std::string &expression_path,
	const std::string &res_function_name,
	bool class_compilation,
	bool use_builtin_texture_runtime)
{
	mi::Sint32 result;

	std::string func_name;
	if (res_function_name.empty()) {
		func_name.assign(expression_path);
		std::replace(func_name.begin(), func_name.end(), '.', '_');
	}
	else {
		func_name.assign(res_function_name);
	}

	if (use_builtin_texture_runtime)
		check_success(m_mdl_helper.m_be_native->set_option("use_builtin_resource_handler", "off") == 0,
			"set use_builtin_resource_handler_off fail");

	MIHandle<const ITarget_code> code_native(
		m_mdl_helper.m_be_native->translate_material_expression(//使用translate_material_expression
			m_mdl_helper.m_transaction.get(), m_compiled_material.get(),
			expression_path.c_str(), func_name.c_str(), &result));

	Texture_handler tex_handler;
	if (use_builtin_texture_runtime)
	{
		// 建立材质传递组件
		std::vector<mdlTexture> textures;
		check_success(
			m_mdl_helper.prepare_textures(textures, code_native.get()),
			"Prepare textures fail");

		tex_handler.vtable = &tex_vtable;//设定用户定义的运行时纹理查找函数
		tex_handler.num_textures = code_native->get_texture_count() - 1;
		tex_handler.textures = textures.data();
	}
	m_expression_code = code_native;
	tex_handler_ptr = &tex_handler;//用运行时材质时，传入handler
}

//编译一个df函数
void MdlHelper::Mdl_compile_result::compile_df(
	const std::string &expression_path,
	const std::string &res_function_name_base,
	bool class_compilation,
	bool use_builtin_texture_runtime)
{
	mi::Sint32 result;

	std::string func_name;
	if (res_function_name_base.empty()) {
		func_name.assign(expression_path);
		std::replace(func_name.begin(), func_name.end(), '.', '_');
	}
	else {
		func_name.assign(res_function_name_base);
	}

	if (use_builtin_texture_runtime)
		check_success(m_mdl_helper.m_be_native->set_option("use_builtin_resource_handler", "off") == 0,
			"set use_builtin_resource_handler_off fail");

	// 生成分布函数代码
	// 这将会在目标代码生成4个函数，init, sample, evaluate 和 pdf
	MIHandle<const ITarget_code> code_native(
		m_mdl_helper.m_be_native->translate_material_df(//使用translate_material_df
			m_mdl_helper.m_transaction.get(), m_compiled_material.get(),
			expression_path.c_str(), func_name.c_str(), true, &result));

	Texture_handler tex_handler;
	if (use_builtin_texture_runtime)
	{
		// 建立材质传递组件
		std::vector<mdlTexture> textures;
		check_success(
			m_mdl_helper.prepare_textures(textures, code_native.get()),
			"Prepare textures fail");

		tex_handler.vtable = &tex_vtable;//设定用户定义的运行时纹理查找函数
		tex_handler.num_textures = code_native->get_texture_count() - 1;
		tex_handler.textures = textures.data();
	}
	m_bsdf_code = code_native;
	tex_handler_ptr = &tex_handler;//用运行时材质时，传入handler
}

MdlHelper::~MdlHelper()
{
	m_environment_codes.clear();
	compile_result_vec.clear();
	tex_handler_ptr = nullptr;

	m_mdl_factory.reset();
	m_be_native.reset();
	m_mdl_compiler.reset();
	m_transaction->commit();
	m_transaction.reset();
	m_global_scope.reset();
	m_database.reset();
	m_image_api.reset();
	m_neuray->shutdown();
	m_neuray.reset();
	unload();
}

void MdlHelper::init(SceneNode *node)
{
	MdlHelperApi::init(node);

	m_neuray = load_and_get_ineuray();
	check_success(m_neuray.is_valid_interface(),
#ifdef MI_PLATFORM_WINDOWS
		"Initialization of MDL SDK failed: libmdl_sdk.dll not found or wrong version."
#else
		"Initialization of MDL SDK failed: libmdl_sdk.so not found or wrong version."
#endif
	);
	m_mdl_compiler = m_neuray->get_api_component<IMdl_compiler>();
	check_success(m_mdl_compiler, "Initialization of MDL compiler failed!");

	if (!module_path.empty())
		//添加搜索路径来搜寻MDL模块和资源
		add_module_path(module_path);

	//加载需要的插件
#ifdef MI_PLATFORM_WINDOWS
	check_success(m_mdl_compiler->load_plugin_library("nvmdl_freeimage.dll"),
		"Couldn't load plugin nvmdl_freeimage.dll!");

	// Consider the dds plugin as optional
	check_success(m_mdl_compiler->load_plugin_library("dds.dll"),
		"Couldn't load plugin dds.dll!");
#else
	check_success(m_mdl_compiler->load_plugin_library("nvmdl_freeimage.so"),
		"Couldn't load plugin nvmdl_freeimage.so!");

	// Consider the dds plugin as optional
	m_mdl_compiler->load_plugin_library("dds.so");
#endif

	//开始SDK
	check_success(m_neuray->start(), "Starting MDL SDK failed!");
	//各种赋值
	m_database = m_neuray->get_api_component<IDatabase>();
	m_global_scope = m_database->get_global_scope();
	m_transaction = m_global_scope->create_transaction();

	m_mdl_factory = m_neuray->get_api_component<IMdl_factory>();

	m_be_native = m_mdl_compiler->get_backend(IMdl_compiler::MB_NATIVE);
	
	check_success(m_be_native->set_option(
		"num_texture_spaces", mto_string(num_texture_spaces).c_str()) == 0,
		"Setting option num_texture_spaces failed");
	/*
	check_success(m_be_native->set_option(
		"internal_space", std::string("object").c_str()) == 0,
		"Setting option internal_space failed");*/

	m_image_api = m_neuray->get_api_component<IImage_api>();
}

uint32_t MdlHelper::compile_environment(
	const std::string &module_name,
	const std::string &function_name,
	bool use_builtin_texture_runtime,
	const std::string &res_function_name,
	MIHandle<const IExpression_list> arguments)
{
	std::string func_name;
	if (res_function_name.empty()) {
		func_name.assign(function_name);
		std::replace(func_name.begin(), func_name.end(), '.', '_');
		size_t paren_idx = func_name.find('(');
		if (paren_idx != std::string::npos)
			func_name.erase(paren_idx);
	}
	else {
		func_name.assign(res_function_name);
	}

	{
		std::string mdl_module_name = "::" + module_name;
		load_module(mdl_module_name);

		std::string mdl_func_name = "mdl::" + module_name + "::" + function_name;
		MIHandle<const IFunction_definition> func_definition(
			m_transaction->access<IFunction_definition>(mdl_func_name.c_str()));
		check_success(func_definition, "Accessing function failed!");

		// Create a function call with only default parameters
		// 用默认参数创造一个函数调用
		mi::Sint32 result;
		MIHandle<IFunction_call> func_call(
			func_definition->create_function_call(arguments.get(), &result));
		check_success(result, "Creating MDL function call failed!");

		if (use_builtin_texture_runtime)
			check_success(m_be_native->set_option("use_builtin_resource_handler", "off") == 0,
				"set use_builtin_resource_handler_off fail");

		MIHandle<const ITarget_code> native_code(
			m_be_native->translate_environment(//使用translate_environment
				m_transaction.get(), func_call.get(),
				1.0f,    // mdl_meters_per_scene_unit
				380.0f,  // mdl_wavelength_min
				780.0f,  // mdl_wavelength_max
				func_name.c_str(), &result));
		check_success(result, "Compiling environment to native code failed!");

		Texture_handler tex_handler;
		if (use_builtin_texture_runtime)
		{
			// 建立材质传递组件
			std::vector<mdlTexture> textures;
			check_success(
				prepare_textures(textures, native_code.get()),
				"Prepare textures fail");

			tex_handler.vtable = &tex_vtable;//设定用户定义的运行时纹理查找函数
			tex_handler.num_textures = native_code->get_texture_count() - 1;
			tex_handler.textures = textures.data();
		}
		m_environment_codes.push_back(native_code);
		tex_handler_ptr = &tex_handler;//用运行时材质时，传入handler
	}
	return m_environment_codes.size() - 1;
}

MdlHelper::Mdl_compile_result MdlHelper::compile(
	const std::string &module_name,
	const std::string &material_name,
	bool class_compilation,
	MIHandle<const IExpression_list> arguments)
{
	std::string mdl_module_name =
		(module_name.find("::") == 0) ? module_name : "::" + module_name;

	load_module(mdl_module_name);

	Mdl_compile_result compile_result(*this);

	//
	// Instantiate and compile the material
	//

	{
		std::string mdl_material_name = "mdl" + mdl_module_name + "::" + material_name;

		MIHandle<const IMaterial_definition> material_definition(
			m_transaction->access<IMaterial_definition>(mdl_material_name.c_str()));
		check_success(material_definition, "Material " + mdl_material_name + " not found!");

		// Instantiate the material
		// 实例化材质
		mi::Sint32 result;
		MIHandle<IMaterial_instance> material_instance(
			material_definition->create_material_instance(arguments.get(), &result));
		check_success(result, "Creating material instance failed!");

		mi::Uint32 flags = class_compilation
			? mi::neuraylib::IMaterial_instance::CLASS_COMPILATION
			: mi::neuraylib::IMaterial_instance::DEFAULT_OPTIONS;

		MIHandle<ICompiled_material> compiled_material(
			material_instance->create_compiled_material(
				flags,
				1.0f,    // mdl_meters_per_scene_unit
				380.0f,  // mdl_wavelength_min
				780.0f,  // mdl_wavelength_max
				&result));
		check_success(result, "Creating compiled material failed!");

		compile_result.set_compiled_material(mdl_module_name, compiled_material);
	}

	return compile_result;
}

//使用运行时材质时，对贴图进行调整
bool MdlHelper::prepare_textures(
	std::vector<mdlTexture>& textures,//材质向量，既用作输入，也用作输出
	const ITarget_code* target_code)
{
	//针对目标代码材质空间中的每一个材质
	for (mi::Size i = 1; i < target_code->get_texture_count(); ++i)
	{
		MIHandle<const ITexture> texture(
			m_transaction->access<const ITexture>(
				target_code->get_texture(i)));//创建一个ITexture对象
		MIHandle<const IImage> image(
			m_transaction->access<IImage>(texture->get_image()));//利用ITexture创建一个IImage
		MIHandle<const ICanvas> canvas(image->get_canvas());//利用IImage创建一个canvas
		char const *image_type = image->get_type();

		if (image->is_uvtile()) {
			printf("The example does not support uvtile textures!");
			return false;
		}

		if (canvas->get_tiles_size_x() != 1 || canvas->get_tiles_size_y() != 1) {
			printf("The example does not support tiled images!");
			return false;
		}

		// Convert to linear color space if necessary
		// 软件默认gamma校正系数是1，进行gamma校正
		if (texture->get_effective_gamma() != 1.0f) {
			// Copy/convert to float4 canvas and adjust gamma from "effective gamma" to 1.
			MIHandle<ICanvas> gamma_canvas(
				m_image_api->convert(canvas.get(), "Color"));
			gamma_canvas->set_gamma(texture->get_effective_gamma());
			m_image_api->adjust_gamma(gamma_canvas.get(), 1.0f);
			canvas = gamma_canvas;
		}
		else if (strcmp(image_type, "Color") != 0 && strcmp(image_type, "Float32<4>") != 0) {
			// Convert to expected format
			canvas = m_image_api->convert(canvas.get(), "Color");
		}
		else if (!strcmp(image_type, "Rgbe"))
		{
			canvas = m_image_api->convert(canvas.get(), "Rgb_fp");
		}
		else if (!strcmp(image_type, "Rgbea"))
		{
			// Convert to Color first
			canvas = m_image_api->convert(canvas.get(), "Color");
		}

		textures.push_back(mdlTexture(canvas));
	}
	return true;
}

CAFEL_NAMESPACE_END