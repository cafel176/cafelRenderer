#include <bsdf/mdlbsdf.hpp>
#include <cafel/utils/warp.hpp>

CAFEL_NAMESPACE_BEGIN

Ray MdlBSDF::sample(const Intersection &its, unsigned int depeth)
{
	using namespace mi::neuraylib;

	dVector o = its.ray.direction * -1;
	o.norm();
	dVector2 sample_ = Warp::get()->sampleCosineHemisphere();
	dVector w(sample_.x, sample_.y, sample_.z);
	w.norm();
	dVector n = its.normal_l;
	n.norm();
	dVector gn = its.normal_l;
	gn.norm();
	dVector u = its.u_dir;
	u.norm();
	dVector v = its.v_dir;
	v.norm();
	// 设置输入状态，内部只含有一个材质空间
	uint32_t num = num_tex_spaces;
	tct_float3 *texture_coords = new tct_float3[num];
	tct_float3 *texture_tangent_u = new tct_float3[num];//u-k1
	tct_float3 *texture_tangent_v = new tct_float3[num];//v-k2
	texture_coords[0].x = its.uv.x;
	texture_coords[0].y = its.uv.y;
	texture_tangent_u[0].x = u.x;
	texture_tangent_u[0].y = u.y;
	texture_tangent_u[0].z = u.z;
	texture_tangent_v[0].x = v.x;
	texture_tangent_v[0].y = v.y;
	texture_tangent_v[0].z = v.z;
	mi::Float32_4_4 identity(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	Shading_state_material mdl_state = {
		/*normal=*/{ n.x, n.y, n.z },
		/*geom_normal=*/{ gn.x, gn.y, gn.z },
		/*position=*/	{ its.hitPoint.x, its.hitPoint.y, its.hitPoint.z },
		/*animation_time=*/   its.t,
		/*texture_coords=*/   texture_coords,
		/*tangent_u=*/        texture_tangent_u,
		/*tangent_v=*/        texture_tangent_v,
		/*text_results=*/     NULL,
		/*ro_data_segment=*/  NULL,
		/*world_to_object=*/  &identity[0],
		/*object_to_world=*/  &identity[0],
		/*object_id=*/        0
	};

	//MdlHelperApi::get()->execute_bsdf_init(index, mdl_state);

	Bsdf_sample_data sample_data{
		// Input fields
		{ ior1.x, ior1.y, ior1.z },//IOR current medium
		{ ior2.x, ior2.y, ior2.z },//IOR other side
		{ o.x, o.y, o.z },//outgoing direction
		{ w.x, w.y, w.z },//pseudo-random sample number

		// Output fields
		{ 0.0f, 0.0f, 0.0f },//incoming direction
		0.0f,				//pdf (non-projected hemisphere)
		{ 0.0f, 0.0f, 0.0f },// bsdf * dot(normal, k2) / pdf
		BSDF_EVENT_ABSORB//the type of event for the generated sample
	};

	MdlHelperApi::get()->execute_bsdf_sample(index, &sample_data, mdl_state);

	dVector d(sample_data.k2.x, sample_data.k2.y, sample_data.k2.z);
	if (d == Vector_zero(double, 3))
	{
		dVector2 r = Warp::get()->sampleCosineHemisphere();
		double r1 = r.x, r2 = r.y, r2s = sqrt(r2);

		dVector u = ((fabs(its.normal_l.x) > .1 ? dVector(0, 1) : dVector(1)) % its.normal_l).norm(), v = its.normal_l % u;
		d = (u*cos(r1)*r2s + v * sin(r1)*r2s + its.normal_l * sqrt(1 - r2)).norm();
	}
	d.norm();
	return Ray(its.hitPoint, d);
}

Color MdlBSDF::eval(const Intersection &its, unsigned int depth)
{
	using namespace mi::neuraylib;

	dVector o = its.ray.direction * -1;
	dVector i = its.out_ray.direction;
	o.norm();
	i.norm();

	dVector u = its.u_dir;
	dVector v = its.v_dir;
	u.norm();
	v.norm();

	dVector n = its.normal_l;
	dVector gn = its.normal_l;
	n.norm();
	gn.norm();

	// 设置输入状态，内部只含有一个材质空间
	uint32_t num = num_tex_spaces;
	tct_float3 *texture_coords = new tct_float3[num];
	tct_float3 *texture_tangent_u = new tct_float3[num];//u-k1
	tct_float3 *texture_tangent_v = new tct_float3[num];//v-k2
	texture_coords[0].x = its.uv.x;
	texture_coords[0].y = its.uv.y;
	texture_tangent_u[0].x = u.x;
	texture_tangent_u[0].y = u.y;
	texture_tangent_u[0].z = u.z;
	texture_tangent_v[0].x = v.x;
	texture_tangent_v[0].y = v.y;
	texture_tangent_v[0].z = v.z;
	mi::Float32_4_4 identity(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Shading_state_material mdl_state = {
		/*normal=*/{ n.x, n.y, n.z },
		/*geom_normal=*/{ gn.x, gn.y, gn.z },
		/*position=*/	{ its.hitPoint.x, its.hitPoint.y, its.hitPoint.z },
		/*animation_time=*/   its.t,
		/*texture_coords=*/   texture_coords,
		/*tangent_u=*/        texture_tangent_u,
		/*tangent_v=*/        texture_tangent_v,
		/*text_results=*/     NULL,
		/*ro_data_segment=*/  NULL,
		/*world_to_object=*/  &identity[0],
		/*object_to_world=*/  &identity[0],
		/*object_id=*/        0
	};

	dVector re;
	//MdlHelperApi::get()->execute_bsdf_init(index, mdl_state);

	Bsdf_evaluate_data eval_data{
		// Input fields
		{ ior1.x, ior1.y, ior1.z },//IOR current medium
		{ ior2.x, ior2.y, ior2.z },//IOR other side
		{ o.x, o.y, o.z },//outgoing direction
		{ i.x, i.y, i.z },//incoming direction

		// Output fields
		{ 0.0f, 0.0f, 0.0f },//bsdf * dot(normal, incoming direction)
		0.0f				//pdf (non-projected hemisphere)
	};

	MdlHelperApi::get()->execute_bsdf_eval(index, &eval_data, mdl_state);
	Color c(eval_data.bsdf.x, eval_data.bsdf.y, eval_data.bsdf.z);
	c = c / eval_data.pdf;
	return c;
}

void MdlBSDF::generateCode(std::ostringstream &oss, int lightNum)
{
	BSDF::generateCode(oss, lightNum);
	oss << "void main(){" << std::endl
		<< "    vec3 color = ";
	sampler->generateCode(oss);
	oss << ";" << std::endl;
	oss << "    vec3 normal = normalize(fs_in.Normal);" << std::endl
		<< "    float diff = 0;" << std::endl
		<< "    for (int i = 0; i < NR_POINT_LIGHTS; i++)" << std::endl
		<< "    {" << std::endl
		<< "        vec3 lightDir = normalize(lightPos[i] - fs_in.FragPos);" << std::endl
		<< "        diff += max(dot(lightDir, normal), 0.0);" << std::endl
		<< "    }" << std::endl
		<< "    vec3 diffuse = diff * color;" << std::endl
		<< "    FragColor = vec4(diffuse, 1.0);" << std::endl
		<< "}" << std::endl;
}

CAFEL_NAMESPACE_END