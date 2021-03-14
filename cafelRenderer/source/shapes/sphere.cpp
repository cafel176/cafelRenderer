#include <shapes/sphere.hpp>
#include <cafel/basic/intersection.hpp>
#include <math.h>

CAFEL_NAMESPACE_BEGIN

std::vector<float> Sphere::getPoint(double u, double v)
{
	std::vector<float> fs;
	double x = sin(M_PI*v)*cos(2 * M_PI*u)*radius;
	double z = sin(M_PI*v)*sin(2 * M_PI*u)*radius;
	double y = cos(M_PI*v)*radius;
	fs.push_back(x);
	fs.push_back(y);
	fs.push_back(z);
	fs.push_back(x);
	fs.push_back(y);
	fs.push_back(z);
	fs.push_back(-u);
	fs.push_back(v);
	return fs;
}

Sphere::Sphere(SceneNode *node) :Shape(node)
{
	std::string radius_ = node->getChildFirst("radius")->getParam("value");
	radius = atof(radius_.c_str());

	uStepsNum = int(radius) / 2;
	if (uStepsNum < 5)
		uStepsNum = 5;
	vStepsNum = uStepsNum;

	double vstep = 1.0 / vStepsNum;
	double ustep = 1.0 / uStepsNum;
	std::vector<std::vector<float>> re;
	for (int i = 0; i < uStepsNum; i++)
	{
		re.clear();
		double v = 1;

		std::vector<float> fs = getPoint(0, 1);
		re.push_back(fs);
		for (int j = 0; j < vStepsNum - 1; j++)
		{
			fs = getPoint(ustep*i, v - vstep);
			re.push_back(fs);
			fs = getPoint(ustep*i + ustep, v - vstep);
			re.push_back(fs);
			v -= vstep;
		}
		fs = getPoint(0, 0);
		re.push_back(fs);

		for (int j = 0; j < re.size(); j++)
		{
			vertices.insert(vertices.end(), re.at(j).begin(), re.at(j).end());

			if (j < re.size() - 2)
			{
				indices.push_back(i*re.size() + j);
				indices.push_back(i*re.size() + j + 1);
				indices.push_back(i*re.size() + j + 2);
			}
		}
	}
}

Intersection Sphere::intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale) const
{
	// returns distance, -1 if nohit
	dVector pos = getRealPosition(position, rotation);
	dVector op = pos - r.origin; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	double t = inf,
		b = op.dot(r.direction),
		det = b * b - op.dot(op) + (radius * scale.x * localScale.x) * (radius * scale.x * localScale.x);

	if (det < eps)
		return Intersection(t_nohit, r);

	det = sqrt(det);
	t = (b - det) > eps ? b - det : ((b + det) > eps ? b + det : t_nohit);
	dVector hitpos = r.origin + r.direction * t;
	dVector n = hitpos - pos;
	n.norm();

	dVector other = dVector(0, 1, 0);
	dVector u_dir = n % other;
	u_dir.norm();
	dVector v_dir = u_dir % n;
	v_dir.norm();

	dVector2 uv;
	uv.y = -acos(n.y) / M_PI;
	uv.x = acos(n.x / sin(M_PI*uv.y)) / (2 * M_PI);

	if (uv.x < 0 || uv.y < 0)
		int p = 0;

	return Intersection(t, r, n, u_dir, v_dir, uv);
}

dVector Sphere::getMin(const dVector &position, const dVector &rotation, const dVector &scale) const
{
	dVector pos = getRealPosition(position, rotation);
	return pos - radius * getRealScale(scale).x;
}

dVector Sphere::getMax(const dVector &position, const dVector &rotation, const dVector &scale) const
{
	dVector pos = getRealPosition(position, rotation);
	return pos + radius*getRealScale(scale).x;
}

void Sphere::generateCode(std::ostringstream &oss)
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