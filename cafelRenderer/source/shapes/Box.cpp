#include <shapes/box.hpp>
#include <cafel/basic/intersection.hpp>
#include <math.h>

CAFEL_NAMESPACE_BEGIN

Box::Box(SceneNode *node) :Shape(node)
{
	std::string p = node->getChildFirst("extend")->getParam("value");
	std::vector<std::string> vec = Warp::get()->split(p, ",");
	extend = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));

	p = node->getChildFirst("axisX")->getParam("value");
	vec = Warp::get()->split(p, ",");
	axisX = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str())).norm();
	p = node->getChildFirst("axisY")->getParam("value");
	vec = Warp::get()->split(p, ",");
	axisY = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str())).norm();
	p = node->getChildFirst("axisZ")->getParam("value");
	vec = Warp::get()->split(p, ",");
	axisZ = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str())).norm();

	float v[288]{
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
	vertices = std::vector<float>(v, v + 288);

	float x = extend.x;
	float y = extend.y;
	float z = extend.z;
	for (unsigned int i = 0; i < 36; i++)
	{
		vertices[i * 8] *= x;
		vertices[i * 8 + 1] *= y;
		vertices[i * 8 + 2] *= z;
	}

	for (unsigned int i = 0; i < 36; i++)
		indices.push_back(i);
}

Intersection Box::intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale) const
{ // returns distance, -1 if nohit
	/*
	dVector pos = getRealPosition(position, rotation);
	dVector oc = pos - r.origin;
	dVector axis[3] = { axisX, axisY, axisZ };

	dVector dirDotAxis(r.direction.dot(axis[0]), r.direction.dot(axis[1]), r.direction.dot(axis[2]));
	dVector ocDotAxis(oc.dot(axis[0]), oc.dot(axis[1]), oc.dot(axis[2]));
	double t;

	dVector BottomT, TopT;
	for (int i = 0; i < 3; i++)
	{
		double es = (dirDotAxis[i] > eps) ? extend[i] : -extend[i];
		BottomT[i]=(ocDotAxis[i] - es)/ dirDotAxis[i];
		TopT[i]=(ocDotAxis[i] + es) / dirDotAxis[i];
	}
	const double LargestMinT = fmax(fmax(BottomT.x, BottomT.y), BottomT.z);
	const double LargestMaxT = fmin(fmin(TopT.x, TopT.y), TopT.z);

	if ((t = LargestMinT) < eps)
	{
		if ((t = LargestMaxT) < eps)
		{
			return Intersection(t_nohit, r);
		}
	}
	else if(LargestMinT > LargestMaxT)
		return Intersection(t_nohit, r);

	dVector d = (r.origin + r.direction * t) - pos;
	int index = -1;
	if (abs(abs(d.x) - abs(extend.x)) < eps)
		index = 0;
	else if (abs(abs(d.y) - abs(extend.y)) < eps)
		index = 1;
	else if (abs(abs(d.z) - abs(extend.z)) < eps)
		index = 2;
	else
		return Intersection(t_nohit, r);

	dVector a = axis[index];
	dVector n = d[index]*a[index] > 0 ? a : a * -1;
	*/

	double t = inf;
	dVector n, u_dir, v_dir;
	dVector2 uv;
	for (unsigned int i = 0; i < indices.size() / 3; i++)
	{
		dVector v0;
		v0.x = vertices.at(indices.at(i * 3) * 8);
		v0.y = vertices.at(indices.at(i * 3) * 8+1);
		v0.z = vertices.at(indices.at(i * 3) * 8+2);
		v0 = v0 * scale + position;
		dVector2 v0_uv;
		v0_uv.x = vertices.at(indices.at(i * 3) * 8 + 6);
		v0_uv.y = vertices.at(indices.at(i * 3) * 8 + 7);

		dVector n0;
		n0.x = vertices.at(indices.at(i * 3) * 8 + 3);
		n0.y = vertices.at(indices.at(i * 3) * 8 + 4);
		n0.z = vertices.at(indices.at(i * 3) * 8 + 5);

		dVector v1;
		v1.x = vertices.at(indices.at(i * 3 + 1) * 8);
		v1.y = vertices.at(indices.at(i * 3 + 1) * 8+1);
		v1.z = vertices.at(indices.at(i * 3 + 1) * 8+2);
		v1 = v1 * scale + position;
		dVector2 v1_uv;
		v1_uv.x = vertices.at(indices.at(i * 3 + 1) * 8 + 6);
		v1_uv.y = vertices.at(indices.at(i * 3 + 1) * 8 + 7);

		dVector v2;
		v2.x = vertices.at(indices.at(i * 3 + 2) * 8);
		v2.y = vertices.at(indices.at(i * 3 + 2) * 8 + 1);
		v2.z = vertices.at(indices.at(i * 3 + 2) * 8 + 2);
		v2 = v2 * scale + position;
		dVector2 v2_uv;
		v2_uv.x = vertices.at(indices.at(i * 3 + 2) * 8 + 6);
		v2_uv.y = vertices.at(indices.at(i * 3 + 2) * 8 + 7);

		// 射线与三角形平行? 
		dVector u = v1 - v0;    // edge1
		dVector v = v2 - v0;    // edge2
		dVector norm = u % v;  // normal
		norm.norm();
		dVector norm_l = (n0 == Vector_zero(double, 3) || norm.dot(n0)) < 0 ? norm : norm * -1;

		triangleData re = Warp::get()->intersectTriangle(r, norm_l, u, v, v0);

		if (re.t > 0 && re.t < t)
		{
			t = re.t;
			n = norm_l;

			dVector2 uv10 = v1_uv - v0_uv;
			dVector2 uv20 = v2_uv - v0_uv;
			uv = uv10 * re.u + uv20 * re.v + v0_uv;
			Warp::get()->getTangent(u,v,uv10, uv20, norm_l, u_dir, v_dir);
		}
	}

	if (abs(t - inf) < eps)
		t = t_nohit;
	return Intersection(t, r, n, u_dir,v_dir,uv);
}

dVector Box::getMin(const dVector &position, const dVector &rotation, const dVector &scale) const
{
	dVector pos = getRealPosition(position, rotation);
	dVector e = extend * getRealScale(scale);
	double len = e.length();
	return pos - Vector_single(double,len, 3);
}

dVector Box::getMax(const dVector &position, const dVector &rotation, const dVector &scale) const
{
	dVector pos = getRealPosition(position, rotation);
	dVector e = extend * getRealScale(scale);
	double len = e.length();
	return pos + Vector_single(double, len,3);
}

void Box::generateCode(std::ostringstream &oss)
{
	Shape::generateCode(oss);
	oss << "void main(){" << std::endl
		<< "    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));" << std::endl
		<< "    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;" << std::endl
		<< "    vs_out.TexCoords = aTexCoords;" << std::endl
		<< "    gl_Position = projection * view * model * vec4(aPos, 1.0);" << std::endl
		<< "}" << std::endl;
}

CAFEL_NAMESPACE_END