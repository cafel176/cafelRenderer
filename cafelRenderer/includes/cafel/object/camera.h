#pragma once

#if !defined(_CAFEL_CAMERA_API_H_)
#define _CAFEL_CAMERA_API_H_

#include <cafel/components/integrator.h>
#include <cstdlib>
#include <cafel/utils/warp.hpp>
#include <cafel/basic/matrix.hpp>

CAFEL_NAMESPACE_BEGIN

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	NONE
};

enum Camera_Rotate
{
	Yaw_More,
	Yaw_Less,
	Pitch_More,
	Pitch_Less,
	None
};

typedef void(*OUT_PROCESS)(double percent, unsigned char *data);

class CameraApi : public CAFEL_ELEMENT
{
public:
	CameraApi(SceneNode *node) :Element(node) 
	{
		std::string v = node->getChildFirst("position")->getParam("value");
		std::vector<std::string> vec = Warp::get()->split(v, ",");
		position = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));

		worldUp = dVector(0, 1, 0);
		if (node->hasChild("worldUp"))
		{
			v = node->getChildFirst("worldUp")->getParam("value");
			vec = Warp::get()->split(v, ",");
			worldUp = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
		}

		if (node->hasChild("front"))
		{
			v = node->getChildFirst("front")->getParam("value");
			vec = Warp::get()->split(v, ",");
			front = dVector(atof(vec.at(0).c_str()), atof(vec.at(1).c_str()), atof(vec.at(2).c_str()));
			updateVectors(true);
		}
		else
		{
			v = node->getParam("yaw");
			yaw = atof(v.c_str());
			v = node->getParam("pitch");
			pitch = atof(v.c_str());
			updateVectors(false);
		}

		if (node->hasParam("zoom"))
		{
			v = node->getParam("zoom");
			zoom = atof(v.c_str());
		}
		else
			zoom = 45;

		if (node->hasParam("speed"))
		{
			v = node->getParam("speed");
			speed = atof(v.c_str());
		}
		else 
		{
			speed = 10;
		}

		if (node->hasParam("rspeed"))
		{
			v = node->getParam("rspeed");
			rspeed = atof(v.c_str());
		}
		else
		{
			rspeed = 10;
		}

		if (node->hasParam("width"))
		{
			v = node->getParam("width");
			width = atoi(v.c_str());
		}
		else
		{
			width = 512;
		}

		if (node->hasParam("height"))
		{
			v = node->getParam("height");
			height = atoi(v.c_str());
		}
		else
			height = 384;

		if (node->hasParam("samples"))
		{
			v = node->getParam("samples");
			samples = atoi(v.c_str()) / (subpixelX*subpixelY);
		}
		else
			samples = 100;

		data = new unsigned char[width * height * 3];

		dMatrix4 projection = dMatrix4::perspective(Warp::get()->radians(zoom), (double)height/(double)width, 0.1f, 1000.0f);
		cx = right * 0.3 * projection[0][0];
		cy = up * 0.3 * projection[1][1];
		cz = front;
	}

	virtual ~CameraApi() { delete[] data; }

	virtual void render(const std::shared_ptr<Integrator> integrator, const std::shared_ptr<Tree> tree, OUT_PROCESS out = nullptr) = 0;
	void clearData()
	{
		data = new unsigned char[width * height * 3];
	}

	unsigned char* output() { return data; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	double getZoom() { return zoom; }
	std::string tostring() const
	{
		std::string str = "camera:(";
		str += "position:" + std::string(position);
		str += ",";
		str += "front:" + std::string(front);
		str += ")";
		return str;
	}

	void move(Camera_Movement *m, double deltaTime)
	{
		dVector direction = Vector_zero(double,3);
		if (m != nullptr)
		{
			for (int i = 0; i < 3; i++)
			{
				switch (m[i])
				{
				case FORWARD: direction = direction + front; break;
				case BACKWARD: direction = direction - front; break;
				case UP: direction = direction + up; break;
				case DOWN: direction = direction - up; break;
				case RIGHT: direction = direction + right; break;
				case LEFT: direction = direction - right; break;
				}
			}

			position = position + direction * speed * deltaTime;
		}
	}

	void rotate(Camera_Rotate *r, double deltaTime)
	{
		for (int i = 0; i < 2; i++)
		{
			switch (r[i])
			{
			case Yaw_More: yaw += rspeed * deltaTime; break;
			case Yaw_Less: yaw -= rspeed * deltaTime; break;
			case Pitch_More: pitch += rspeed * deltaTime; break;
			case Pitch_Less: pitch -= rspeed * deltaTime; break;
			}
		}

		updateVectors(false);
	}

	dMatrix4 getViewMatrix()
	{
		return dMatrix4::lookAt(position, position + front, up);
	}

	dVector getPos() { return position; }

protected:
	dVector position;
	dVector front;
	dVector up;
	dVector right;
	dVector worldUp;

	double yaw;
	double pitch;
	double zoom;
	double speed;
	double rspeed;

	int samples;
	int width;
	int height;

	int subpixelX = 2, subpixelY = 2;
	dVector cx, cy, cz;

	unsigned char *data;

	void updateVectors(bool hasFront)
	{
		if (hasFront)
		{
			front.norm();
			pitch = Warp::get()->degree(asin(front.y));
			double a = cos(Warp::get()->radians(pitch));
			yaw = Warp::get()->degree(asin(front.z))/a;
		}
		else 
		{
			front.x = cos(Warp::get()->radians(yaw)) * cos(Warp::get()->radians(pitch));
			front.y = sin(Warp::get()->radians(pitch));
			front.z = sin(Warp::get()->radians(yaw)) * cos(Warp::get()->radians(pitch));
			front.norm();
		}
		
		right = front % worldUp;
		right.norm();

		up = right % front;
		up.norm();
	}
};

CAFEL_NAMESPACE_END

#endif 
