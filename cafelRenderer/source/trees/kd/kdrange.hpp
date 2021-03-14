#pragma once

#if !defined(_CAFEL_KDRANGE_HPP_)
#define _CAFEL_KDRANGE_HPP_

#include <cafel/components/tree.h>

CAFEL_NAMESPACE_BEGIN

class KdRange : public TreeNodeRange
{
public:
	// �����಻���ó�ʼ���б��ʼ��
	KdRange(const AABB &aabb_):TreeNodeRange(aabb_){}
	virtual ~KdRange() {}
};

CAFEL_NAMESPACE_END

#endif 