#pragma once

constexpr float PLAYER_DEFAULT_HEALTH	= 100.0f;
constexpr float PLAYER_DEFAULT_FUEL		= 50.0f;
constexpr float PLAYER_DEFAULT_FOOD		= 50.0f;

struct SaveState
{
	int seed;
	unsigned int segment;
	float playerHealth;
	float playerFuel;
	float playerFood;
	bool showTutortial;

	static SaveState Default ()
	{
		SaveState state;
		state.playerHealth = PLAYER_DEFAULT_HEALTH;
		state.playerFuel = PLAYER_DEFAULT_FUEL;
		state.playerFood = PLAYER_DEFAULT_FOOD;
		state.showTutortial = true;
		state.segment = 0;
		state.seed = rand();
		return state;
	}

	inline int GetSegmentedSeed() const { return seed ^ std::hash<int>()(segment); }
};