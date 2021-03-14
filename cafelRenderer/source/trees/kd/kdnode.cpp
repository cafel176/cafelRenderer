#include <trees/kd/kdnode.hpp>
#include <cafel/utils/warp.hpp>
#include <algorithm> 
#include <factory/mainfactory.hpp>

CAFEL_NAMESPACE_BEGIN

int obj_sort_main = 0;

bool objSort(const std::shared_ptr<ObjectApi>& s1,const std::shared_ptr<ObjectApi>& s2)
{
	dVector d1 = s1->getRealPosition();
	dVector d2 = s2->getRealPosition();
	d1.main = d2.main = obj_sort_main;
	return d1 < d2;
}

std::string KdNode::tostring() const
{
	std::string str = "\n\tTreeNode(";
	//str += "\n\t\trange:" + range->tostring();

	if (left != nullptr)
		str += "\n\t\tleft:" + left->tostring();
	if (right != nullptr)
		str += "\n\t\tright:" + right->tostring();
	if (left == nullptr && right == nullptr)
	{
		str += "\n\t\tobjects:";
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			str += "\n\t\t" + objects.at(i)->tostring();
		}
	}

	str += "\n)\n";
	return str;
}

KdNode::KdNode(std::vector<std::shared_ptr<ObjectApi>> objects_, TreeNode *parent_) :TreeNode(objects_, parent_)
{
	unsigned int num = objects_.size();
	if (num > 0)
	{
		dVector min = objects_.at(0)->getMin(), max = objects_.at(0)->getMax();
		int t = 0;
		if (parent != nullptr)
		{
			std::shared_ptr<XYZSplit> pr = std::static_pointer_cast<XYZSplit>(parent->getRange());
			t = pr->getChildType();
		}

		for (unsigned int i = 1; i < num; i++)
		{
			min = Warp::get()->minVec(min, objects_.at(i)->getMin());
			max = Warp::get()->maxVec(max, objects_.at(i)->getMax());
		}

		std::shared_ptr<TreeFactory> fac(MainFactory::getTreeFactory(TreeType::KD));
		std::shared_ptr<XYZSplit> range_ = std::static_pointer_cast<XYZSplit>(fac->getNodeRange(KdRangeType::XYZ, AABB(min, max)));
		range_->setType(t);
		range = range_;

		if (num >= 3)
		{
			std::vector<std::shared_ptr<ObjectApi>> objs;
			for (unsigned int i = 0; i < num; i++)
			{
				objs.push_back(objects_.at(i));
			}

			obj_sort_main = t;
			std::sort(objs.begin(), objs.end(), objSort);
			objects = objs;

			std::vector<std::shared_ptr<ObjectApi>> objs_left;
			std::vector<std::shared_ptr<ObjectApi>> objs_right;
			for (unsigned int i = 0; i < num; i++)
			{
				unsigned int index = objs.size() / 2;
				if(i < index)
					objs_left.push_back(objs.at(i));
				else if(i > index)
					objs_right.push_back(objs.at(i));
				else
				{
					double value = objs.at(index)->getRealPosition()[t];
					double left_value = objs.at(index - 1)->getRealPosition()[t];
					double right_value = objs.at(index + 1)->getRealPosition()[t];
					if (left_value > right_value)
						objs_left.push_back(objs.at(i));
					else
						objs_right.push_back(objs.at(i));
				}
			}

			left = fac->getNode(objs_left, this);
			right = fac->getNode(objs_right, this);
		}
		else
		{
			objects = objects_;
		}
	}
}

std::shared_ptr<ObjectApi> KdNode::intersect(double &t, const Ray &r) const
{
	if (left == nullptr)
	{
		t = inf;
		int index = -1;
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			Intersection its = objects.at(i)->intersect(r);
			double t_ = its.t;
			if (t_ <= t && abs(t_ - t_nohit)>eps)
			{
				// 相同t时优先取光源
				if (t_ < t || (objects.at(i)->hasEmitter() && !objects.at(index)->hasEmitter()))
				{
					t = t_;
					index = i;
				}
			}
		}
		if (index != -1)
			return objects.at(index);
	}
	else 
	{
		bool left_b = left->intersectAABB(r);
		bool right_b = right->intersectAABB(r);

		if (left_b && right_b)
		{
			double t_l, t_r;
			std::shared_ptr<ObjectApi> obj_l = left->intersect(t_l,r);
			std::shared_ptr<ObjectApi> obj_r = right->intersect(t_r,r);

			if (abs(t_l - t_nohit) > eps && abs(t_r - t_nohit) < eps)
			{
				t = t_l;
				return obj_l;
			}
			else if (abs(t_l - t_nohit) < eps && abs(t_r - t_nohit) > eps)
			{
				t = t_r;
				return obj_r;
			}
			else if (abs(t_l - t_nohit) > eps && abs(t_r - t_nohit) > eps)
			{
				if (t_l < t_r)
				{
					t = t_l;
					return obj_l;
				}
				else if (t_l > t_r)
				{
					t = t_r;
					return obj_r;
				}
				else 
				{
					if(obj_l->hasEmitter())
					{
						t = t_l;
						return obj_l;
					}
					else
					{
						t = t_r;
						return obj_r;
					}
				}
			}
		}
		else if (!left_b && right_b)
		{
			std::shared_ptr<ObjectApi> obj = right->intersect(t,r);
			return obj;
		}
		else if (!right_b && left_b)
		{
			std::shared_ptr<ObjectApi> obj = left->intersect(t,r);
			return obj;
		}
	}

	t = t_nohit;
	return nullptr;
}

CAFEL_NAMESPACE_END