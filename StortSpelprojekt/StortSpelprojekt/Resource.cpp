#include "stdafx.h"
size_t Resource::idCounter = 10;

Resource::Resource() : id(idCounter++)
{
}

Resource::~Resource() {}
