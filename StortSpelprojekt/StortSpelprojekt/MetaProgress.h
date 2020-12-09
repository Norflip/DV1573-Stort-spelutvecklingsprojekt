#pragma once
#include "SaveState.h"

constexpr unsigned int SCORE_C_ENEMY = 100;  
constexpr unsigned int SCORE_B_ENEMY = 80;
constexpr unsigned int SCORE_NODE_REACHED = 200;
constexpr unsigned int SCORE_LEVEL_CLEARED = 1000;

constexpr unsigned int SCORE_FUEL_USED = 80;
constexpr unsigned int SCORE_FOOD_USED = 120;
constexpr unsigned int SCORE_HEAL_USED = 70;
constexpr unsigned int SCORE_PUZZLE_SOLVED = 1500;

constexpr float SCORE_TO_CURRENCY = 0.05f;


class MetaProgress
{
public:
	MetaProgress();
	~MetaProgress();
	//void Initialize();

	void Print();
	void SaveScore(); // saves score when player dies.. and then resets
	void SaveProgress(SaveState& theSave);
	void LoadSave(SaveState & theSave); //load when enter gamescene.. OnActive
	void Reset();


	void EnemyKilled(const std::string& enemytype); //done
	void SetNodeReached(const int& currentNode);
	void SetLevelsCleared(const int& currentLevel);

	void IncFuelUsed(int value); //done
	void IncFoodUsed(int value); //done
	void IncHealUsed(int value); //done
	void IncPuzzleSolved(int value); //collision callback scuffed reslut in multiple solved 
	
	//Returns a reference to the singleton
	static MetaProgress& Instance() // singleton
	{
		static MetaProgress instance;
		return instance;
	}
private:
	//load from save
	unsigned int currencyTotal;
	unsigned int highscore;
	unsigned int nrOfGameWins; //
	
	//variables keeping track of this turn/session
	unsigned int score;
	unsigned int currencyEarned;

	//track stuff
	unsigned int bEnemiesKilled;
	unsigned int cEnemiesKilled;
	unsigned int nodesReached; //how far house traveled
	unsigned int levelsCleared; //how many segments cleared

	unsigned int fuelUsed; 
	unsigned int foodUsed;
	unsigned int healUsed;
	unsigned int puzzleSolved;

	//boost stats
	//(give enemies higher stats depending on nrofgame wins and level recached)

	//Reason of Death

	//hold GUI?
	//hold calc?
};