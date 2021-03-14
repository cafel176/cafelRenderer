#pragma once

#if !defined(_CAFEL_SCENE_NODE_H_)
#define _CAFEL_SCENE_NODE_H_

#include <cafel/system/api.h>
#include <cctype>
#include <algorithm>
#include <string>
#include <map>
#include <memory>

CAFEL_NAMESPACE_BEGIN

class SceneNode : CAFEL_API
{
public:
	SceneNode(std::string name_)
	{
		std::string tmp = name_;
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
		name = tmp;
	}

	void addParam(std::string name, std::string value)
	{
		params.insert(std::make_pair(name, value));
	}

	void addChild(std::string name, SceneNode* node)
	{
		children.insert(std::make_pair(name, node));
	}

	std::string getParam(std::string name)
	{
		return params[name];
	}

	SceneNode* getChildFirst(std::string name)
	{
		return children.lower_bound(name)->second;
	}

	std::multimap<std::string, SceneNode*>::iterator getChildBegin(std::string name)
	{
		return children.lower_bound(name);
	}

	std::multimap<std::string, SceneNode*>::iterator getChildEnd(std::string name)
	{
		return children.upper_bound(name);
	}

	bool hasParam(std::string name)
	{
		return params.count(name)>0;
	}

	bool hasChild(std::string name)
	{
		return children.count(name)>0;
	}

	std::string getName()
	{
		return name;
	}

private:
	std::string name;
	std::map<std::string, std::string> params;
	std::multimap<std::string, SceneNode*> children;
};

CAFEL_NAMESPACE_END

#endif 