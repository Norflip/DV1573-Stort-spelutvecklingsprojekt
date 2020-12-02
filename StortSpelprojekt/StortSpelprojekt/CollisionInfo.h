#pragma once

class Object;

struct CollisionInfo
{
	Object* main;
	Object* other;
	bool remove;
	int amount = 0;
};