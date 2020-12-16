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
	unsigned int upgradeCurrency;
	unsigned int highscore;
	unsigned int nrOfGameWins;
	unsigned int segmentRecord;

	//reduction/boost
	float fuelLossRed;
	float foodLossRed;
	float hpLossInFogRed;
	float damageRed;
	float damageBoost;
	static SaveState Default ()
	{
		SaveState state;
		state.playerHealth = PLAYER_DEFAULT_HEALTH;
		state.playerFuel = PLAYER_DEFAULT_FUEL;
		state.playerFood = PLAYER_DEFAULT_FOOD;
		state.showTutortial = true;
		state.segment = 0;
		state.seed = rand();
		state.upgradeCurrency = 1000;
		state.highscore = 0;
		state.nrOfGameWins = 0;
		state.segmentRecord = 0;

		state.fuelLossRed = 1.f;
		state.foodLossRed = 1.f;
		state.hpLossInFogRed = 1.f;
		state.damageRed = 1.f;
		state.damageBoost = 1.f;
		return state;
	}

	inline int GetSegmentedSeed() const { return seed ^ std::hash<int>()(segment); }
};