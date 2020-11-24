#include "stdafx.h"
#include "NodeClass.h"

NodeClass::NodeClass(int x, int y)
	: x(x), y(y), gCost(0), hCost(0), fCost(0), openSet(false), closedSet(false), correctPath(false), previous(nullptr)
{

}
