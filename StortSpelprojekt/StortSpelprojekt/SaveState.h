#pragma once
#include <functional>
#include "Random.h"

//const int SLOTS = 1;
//const int REGISTRY_MAX_SIZE = 64;
//
//struct SaveData
//{
//	size_t slot;
//	size_t levelIndex;
//	int seed;
//
//	size_t keyCount;
//	size_t keys[REGISTRY_MAX_SIZE];
//	float value[REGISTRY_MAX_SIZE];
//};
//
//class SaveState
//{
//public:
//	SaveState();
//	SaveState(size_t slot, bool loaded, const SaveStateData& data);
//
//	SaveStateData& GetData() { return this->data; }
//
//	static SaveState Load(size_t slot = 0);
//	static void Save(SaveState state, size_t slot = 0);
//	static bool Contains(size_t slot = 0);
//	static void DeleteSlot(size_t slot = 0);
//
//private:
//	size_t slot;
//	bool loaded;
//	SaveStateData data;
//};
//

struct SaveState
{
	int seed;
	size_t segment;
	std::vector<int> m_generatedSeeds;

	inline int GetSegmentedSeed() const
	{
		return 	seed ^ std::hash<int>()(segment);
	}
};