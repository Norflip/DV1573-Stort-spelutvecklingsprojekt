#include "stdafx.h"
#include "MetaProgress.h"

MetaProgress::MetaProgress()
{
	this->currencyTotal = 00;
	this->highscore = 0;
	this->nrOfGameWins = 0;

	this->score = 0;
	this->currencyEarned = 0;
	this->highScoreBeaten = false;
	this->segmentBeaten = false;

	this->bEnemiesKilled = 0;
	this->cEnemiesKilled = 0;
	this->nodesReached = 0;
	this->levelsCleared = 0;

	this->fuelUsed = 0;
	this->foodUsed = 0;
	this->healUsed = 0;
	this->puzzleSolved = 0;
	this->deathReason = "unknown.";



	this->fuelLossRed = 1.f;
	this->foodLossRed = 1.f;
	this->hpLossInFogRed = 1.f;
	this->damageRed = 1.f;
	this->damageBoost = 1.f;

}

MetaProgress::~MetaProgress()
{
}

void MetaProgress::Print()
{
	std::cout << "Meta Info:" << std::endl
		<< "score: " << score << ", highScore: " << highscore << std::endl
		<< "currency earned:" << currencyEarned << ", currency total: " << currencyTotal << std::endl
		<< "[Progress] current node: " << nodesReached << ", segment cleared: " << levelsCleared << ", game wins: " << nrOfGameWins << std::endl
		<< "[Kills] base: " << bEnemiesKilled << ", charge: " << cEnemiesKilled << std::endl
		<< "[Items] fuel: " << fuelUsed << ", food: " << foodUsed << ", heals: " << healUsed << std::endl
		<< "[Misc n shet] puzzles: " << puzzleSolved << std::endl
		<< "===========" << std::endl << std::endl;
}

std::string MetaProgress::GetInfo()
{
	//int totalNodes = MetaProgress::Instance().GetNodesReached() + MetaProgress::Instance().GetLevelsCleared() * 19;
	std::string scoreInfo = "Results\n";
	std::string space = "     ";
	scoreInfo += "[Scores]";
	if (highScoreBeaten)
		scoreInfo += "  -->[ New Highscore!! ]<--";
	scoreInfo += "\n" + space + "Score: " + std::to_string(score) + ", High-Score: " + std::to_string(highscore) + "\n";
	scoreInfo += "[Upgrade-currency]\n"+space+ "Earned: " + std::to_string(currencyEarned) + " --> Total: " + std::to_string(currencyTotal) + "\n";
	scoreInfo += "[Game-progress]";
	if (segmentBeaten)
		scoreInfo += "  -->[ New Record!! ]<--";
	scoreInfo += "\n" + space + "Level Reached:" + std::to_string(levelsCleared) + ", Record-Level: " + std::to_string(levelsRecord) + "\n";
	scoreInfo += space+"Last CheckPoint: " + std::to_string(nodesReached) + ", Game Wins: " + std::to_string(nrOfGameWins) +"\n";

	scoreInfo += "[Kills]\n"+space+"Regular-Enemy: " + std::to_string(bEnemiesKilled) + ", Charger-Enemy: " + std::to_string(cEnemiesKilled) + "\n";
	scoreInfo += "[Items]\n"+space+"Fuel: " + std::to_string(fuelUsed) + ", Food: " + std::to_string(foodUsed) + ", Healthpacks: " + std::to_string(healUsed) + "\n";
	scoreInfo += "[Misc]\n"+space+"Puzzles: " + std::to_string(puzzleSolved) + "\n";
	

	return scoreInfo;
}

void MetaProgress::SaveScore()
{
	if (this->score > this->highscore)
	{
		this->highscore = this->score;
		this->highScoreBeaten = true;
	}
	this->currencyTotal += this->currencyEarned;

	if (this->levelsCleared > levelsRecord)
	{
		this->levelsRecord = this->levelsCleared;
		this->segmentBeaten = true;
	}
}

void MetaProgress::SaveProgress(SaveState& theSave)
{
	theSave.highscore = this->highscore;
	theSave.upgradeCurrency = this->currencyTotal;
	theSave.nrOfGameWins = this->nrOfGameWins;
	theSave.segment = this->levelsCleared;
	theSave.segmentRecord = this->levelsRecord;

	theSave.fuelLossRed = this->fuelLossRed;
	theSave.foodLossRed = this->foodLossRed;
	theSave.hpLossInFogRed = this->hpLossInFogRed;
	theSave.damageRed = this->damageRed;
	theSave.damageBoost = this->damageBoost;

	LoadSave(theSave);
}

void MetaProgress::LoadSave(SaveState& theSave)
{
	this->currencyTotal = theSave.upgradeCurrency;
	this->highscore = theSave.highscore;
	this->nrOfGameWins = theSave.nrOfGameWins;
	this->levelsCleared = theSave.segment;
	this->levelsRecord = theSave.segmentRecord;

	this->fuelLossRed = theSave.fuelLossRed;
	this->foodLossRed = theSave.foodLossRed;
	this->hpLossInFogRed = theSave.hpLossInFogRed;
	this->damageRed = theSave.damageRed;
	this->damageBoost = theSave.damageBoost;
}

void MetaProgress::Reset()
{
	this->score = 0;
	this->currencyEarned = 0;
	this->highScoreBeaten = false;
	this->segmentBeaten = false;

	this->bEnemiesKilled = 0;
	this->cEnemiesKilled = 0;
	this->nodesReached = 0;
	this->levelsCleared = 0;
	this->levelsRecord = 0;
	
	this->fuelUsed = 0;
	this->foodUsed = 0;
	this->healUsed = 0;
	this->puzzleSolved = 0;
	//this->nrOfGameWins = 0;
	//this->deathReason = "unknown.";
}

void MetaProgress::EnemyKilled(const std::string& enemytype)
{
	//std::cout << "type: " << enemytype << std::endl;
	if (enemytype == "chargerEnemy")
		cEnemiesKilled++;
	else if (enemytype == "baseEnemy")
		bEnemiesKilled++;
}

void MetaProgress::SetNodeReached(const int& currentNode)
{
	this->nodesReached = currentNode;
}

void MetaProgress::SetLevelsCleared(const int& currentLevel)
{
	this->levelsCleared = currentLevel;
}

void MetaProgress::IncFuelUsed(int value)
{
	this->score += SCORE_FUEL_USED + value;
	this->currencyEarned += ((SCORE_FUEL_USED + value) * SCORE_TO_CURRENCY);
	this->fuelUsed++;
}

void MetaProgress::IncFoodUsed(int value)
{
	this->score += SCORE_FOOD_USED + value;
	this->currencyEarned += ((SCORE_FOOD_USED + value) * SCORE_TO_CURRENCY);
	this->foodUsed++;
}

void MetaProgress::IncHealUsed(int value)
{
	this->score += SCORE_HEAL_USED + value;
	this->currencyEarned += ((SCORE_HEAL_USED + value) * SCORE_TO_CURRENCY);
	this->healUsed++;
}

void MetaProgress::IncPuzzleSolved(int value)
{
	this->score += SCORE_PUZZLE_SOLVED + value;
	this->currencyEarned += ((SCORE_PUZZLE_SOLVED + value) * SCORE_TO_CURRENCY);
	this->puzzleSolved++;
}
