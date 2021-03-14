#include <trees/kd/XYZSplit.hpp>

CAFEL_NAMESPACE_BEGIN

SplitType XYZSplit::getChildType()
{ 
	//X0 Y1 Z2
	return type == SplitType::Z ? SplitType::X : static_cast<SplitType>(type + 1); //XYZ
	//return type == SplitType::X ? SplitType::Z : static_cast<SplitType>(type - 1); //XZY
}

CAFEL_NAMESPACE_END