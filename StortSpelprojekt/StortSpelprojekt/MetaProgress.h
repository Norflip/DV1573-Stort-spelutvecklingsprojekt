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
	std::string GetInfo();
	void SaveScore(); // saves score when player dies.. and then resets
	void SaveProgress(SaveState& theSave);
	void LoadSave(SaveState & theSave); //load when enter gamescene.. OnActive
	void Reset();


	void EnemyKilled(const std::string& enemytype); //done
	void SetNodeReached(const int& currentNode);
	unsigned int GetNodesReached()const { return this->nodesReached; };
	void SetLevelsCleared(const int& currentLevel);
	unsigned int GetLevelsCleared()const { return this->levelsCleared; };

	void SetKilledBy(std::string reason) { this->deathReason = reason; };
	std::string GetKilledBy()const { return this->deathReason; };
	
	unsigned int GetHighScore()const { return highscore; }
	unsigned int GetScore()const { return score; }
	unsigned int GetCurrencyTotal()const { return currencyTotal; }
	unsigned int GetCurrencyEarned()const { return currencyEarned; }
	unsigned int GetNrOfGameWins()const { return nrOfGameWins; }

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
	bool highScoreBeaten;
	bool segmentBeaten;

	//track stuff
	unsigned int bEnemiesKilled;
	unsigned int cEnemiesKilled;
	unsigned int nodesReached; //how far house traveled
	unsigned int levelsCleared; //how many segments cleared
	unsigned int levelsRecord;

	unsigned int fuelUsed; 
	unsigned int foodUsed;
	unsigned int healUsed;
	unsigned int puzzleSolved;

	std::string deathReason;
	

};