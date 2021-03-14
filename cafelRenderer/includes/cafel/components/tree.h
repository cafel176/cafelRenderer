#pragma once
#if !defined(_CAFEL_TREE_H_)
#define _CAFEL_TREE_H_

#include <cafel/object/object.h>
#include <cafel/basic/AABB.hpp>
#include <vector>

CAFEL_NAMESPACE_BEGIN

class TreeNodeRange : public CAFEL_API
{
public:
	// �����಻���ó�ʼ���б��ʼ��
	TreeNodeRange(const AABB &aabb_) { aabb = aabb_; }
	virtual ~TreeNodeRange() {}

	virtual bool intersect(const Ray &r)
	{
		double t0, t1;
		return aabb.intersect(r,t0,t1);
	}

	virtual std::string tostring() const
	{
		return "TreeNodeRange(" + std::string(aabb) +")";
	}

protected:
	AABB aabb;
};

// �̳�std::enable_shared_from_this�������shared_from_this�����Լ�������ָ�룬���ǲ����ڹ��캯����ʹ��
class TreeNode : public CAFEL_API ,public std::enable_shared_from_this<TreeNode>
{
public:
	TreeNode(std::vector<std::shared_ptr<ObjectApi>> objects_, TreeNode *parent_) { parent = parent_; }
	virtual ~TreeNode() {}

	virtual void addObject(std::shared_ptr<ObjectApi> obj) { if (obj != nullptr) objects.push_back(obj); }
	virtual std::shared_ptr<TreeNodeRange> getRange() const { return range; }
	virtual bool intersectAABB(const Ray &r) const { return range->intersect(r); }

	virtual std::shared_ptr<ObjectApi> intersect(double &t, const Ray &r) const = 0;
	virtual std::shared_ptr<TreeNode> getChild(int child) const = 0;

	virtual std::vector<std::shared_ptr<ObjectApi>> getObjects() { return objects; }

	virtual std::string tostring() const
	{
		return "TreeNode(\nrange:" + range->tostring() + ")";
	}

protected:
	std::vector<std::shared_ptr<ObjectApi>> objects;
	std::shared_ptr<TreeNodeRange> range;
	TreeNode *parent;
};

class Tree : public CAFEL_API
{
public:
	Tree(){}
	virtual ~Tree() {}// ���ں�virtual����ʱ��Ϊ������virtual��������

	virtual void buildTree(std::vector<std::shared_ptr<ObjectApi>> &objects) = 0;
	virtual Interaction interact(std::shared_ptr<ObjectApi> nowObj, Intersection &i, unsigned int depth) const
	{ 
		return nowObj->interact(i, depth);
	}

	virtual Intersection intersect(std::shared_ptr<ObjectApi> &nowObj, const Ray &r)
	{ 
		double t;
		nowObj = root->intersect(t, r);
		if (nowObj == nullptr)
		{
			return Intersection(t_nohit, r);
		}
		else
		{
			return nowObj->intersect(r);
		}
	}

	virtual std::string tostring() const
	{
		return "Tree(root:" + root->tostring() + ")";
	}

	virtual std::vector<std::shared_ptr<ObjectApi>> getObjects() { return root->getObjects(); }

protected:
	std::shared_ptr<TreeNode> root;

};

CAFEL_NAMESPACE_END

#endif 
