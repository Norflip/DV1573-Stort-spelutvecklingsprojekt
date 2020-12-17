#include "stdafx.h"
#include "IntroScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"
#include "Config.h"

IntroScene::IntroScene() : Scene("IntroScene")
{

}

IntroScene::~IntroScene()
{
	delete guiManager;
}

void IntroScene::Initialize()
{
	InitializeGUI();
	InitializeObjects();
}

void IntroScene::InitializeObjects()
{
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(window->GetWidth(), window->GetHeight(), 60.0f);
	//cameraObject2->AddComponent<ControllerComponent>();
	this->player = cameraObject;
	AddObjectToRoot(cameraObject);
}

void IntroScene::InitializeGUI()
{
	float windowWidth = FCAST(window->GetWidth());
	//spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	unsigned int titleSpacing = 120;
	unsigned int titlePosH = 100;
	GUISprite* titleSprite = new GUISprite(*renderer, "Textures/OnceUponATime.png", windowWidth / 2 - 100, titlePosH, 0, DrawDirection::Default, ClickFunction::NotClickable);
	GUISprite* startSprite = new GUISprite(*renderer, "Textures/Start.png", 100, titlePosH, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* howToPlaySprite = new GUISprite(*renderer, "Textures/howToPlay.png", 20, titlePosH+titleSpacing, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* upgradesSprite = new GUISprite(*renderer, "Textures/upgrades.png", 90, titlePosH + titleSpacing*2, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* optionSprite = new GUISprite(*renderer, "Textures/Options.png", 100, titlePosH + titleSpacing*3, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* loreSprite = new GUISprite(*renderer, "Textures/Lore.png", 100, titlePosH + titleSpacing*4, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* credits = new GUISprite(*renderer, "Textures/credits.png", 100, titlePosH + titleSpacing*5, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quitSprite = new GUISprite(*renderer, "Textures/Exit.png", 100, titlePosH + titleSpacing*6, 0, DrawDirection::Default, ClickFunction::Clickable);

	
	GUISprite* backSprite = new GUISprite(*renderer, "Textures/BackButton.png", 100, 100, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::HowToPlay);
	GUISprite* loadSprite = new GUISprite(*renderer, "Textures/Loading.png", 0, 0, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Load);
	GUISprite* musicSprite = new GUISprite(*renderer, "Textures/Music.png", 110, 250, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);
	musicSprite->SetVisible(false);
	loadSprite->SetVisible(false);
	GUISprite* soundEffectsSprite = new GUISprite(*renderer, "Textures/SoundeffectsButton.png", 160, 400, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);
	soundEffectsSprite->SetVisible(false);

	GUISprite* sensitivitySprite = new GUISprite(*renderer, "Textures/sensButton.png", 140, 550, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);
	sensitivitySprite->SetVisible(false);



	GUISprite* volumeBarFillMusic = new GUISprite(*renderer, "Textures/volumeBarFill.png", 900, 250, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);
	GUISprite* lowerMusicSprite = new GUISprite(*renderer, "Textures/lowerVolume.png", 800, 250, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Options);
	GUISprite* volumeMusicSprite = new GUISprite(*renderer, "Textures/volumeButton.png", 900, 250, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);
	GUISprite* higherMusicSprite = new GUISprite(*renderer, "Textures/higherVolume.png", 1150, 250, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Options);
	lowerMusicSprite->SetVisible(false);
	volumeMusicSprite->SetVisible(false);
	higherMusicSprite->SetVisible(false);
	volumeBarFillMusic->SetVisible(false);
	volumeBarFillMusic->SetScale(AudioMaster::Instance().GetVolume(AudioTypes::Music), 1.0f);

	GUISprite* volumeBarFillSoundeffects = new GUISprite(*renderer, "Textures/volumeBarFill.png", 900, 400, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);
	GUISprite* lowerSoundEffectMusic = new GUISprite(*renderer, "Textures/lowerVolume.png", 800, 400, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Options);
	GUISprite* volumeSoundEffectMusic = new GUISprite(*renderer, "Textures/volumeButton.png", 900, 400, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);
	GUISprite* higherSoundEffectMusic = new GUISprite(*renderer, "Textures/higherVolume.png", 1150, 400, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Options);
	lowerSoundEffectMusic->SetVisible(false);
	volumeSoundEffectMusic->SetVisible(false);
	higherSoundEffectMusic->SetVisible(false);
	volumeBarFillSoundeffects->SetVisible(false);
	volumeBarFillSoundeffects->SetScale(AudioMaster::Instance().GetVolume(AudioTypes::Sound), 1.0f);

	GUISprite* lowerSensitivitySprite = new GUISprite(*renderer, "Textures/lowerVolume.png", 800, 550, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Options);
	GUISprite* higherSensitivitySprite = new GUISprite(*renderer, "Textures/higherVolume.png", 1150, 550, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Options);	

	lowerSensitivitySprite->SetVisible(false);
	higherSensitivitySprite->SetVisible(false);
	GUIFont* sensitivityDisplay = new GUIFont(*renderer, "sensitivity", windowWidth / 2 + 30, 585);
	sensitivityDisplay->SetFontSize({ 1.f,1.f });
	sensitivityDisplay->SetVisible(false);
	sensitivityDisplay->AddGroup(GuiGroup::Font);
	sensitivityDisplay->AddGroup(GuiGroup::Options);
	sensitivityDisplay->RemoveGroup(GuiGroup::Default);


	//upgrades
	GUIFont* upgradeTitle = new GUIFont(*renderer, "Upgrades", windowWidth / 2-350, 100);
	upgradeTitle->SetFontSize({ 1.2f,1.2f });
	upgradeTitle->SetVisible(false);
	upgradeTitle->AddGroup(GuiGroup::Upgrades);
	upgradeTitle->RemoveGroup(GuiGroup::Default);
	GUISprite* reset = new GUISprite(*renderer, "Textures/resetButton.png", windowWidth / 2 + 450, 100, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	reset->AddGroup(GuiGroup::Upgrades);
	reset->RemoveGroup(GuiGroup::Default);
	reset->SetVisible(false);
	GUIFont* currencyAmount = new GUIFont(*renderer, "U-Currency: ", windowWidth / 2 - 550, 170);
	currencyAmount->SetFontSize({ 0.8f,0.8f });
	currencyAmount->SetVisible(false);
	currencyAmount->AddGroup(GuiGroup::Upgrades);
	currencyAmount->RemoveGroup(GuiGroup::Default);
	unsigned int uSpacing = 150;
	unsigned int uPosBar = 35;
	unsigned int uPos = 80;
	float fontsize = 0.8f;
	GUISprite* barFillFuelLoss = new GUISprite(*renderer, "Textures/blueBarFill.png", 1000, uPosBar + uSpacing, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Upgrades);
	barFillFuelLoss->SetScale(MetaProgress::Instance().GetFuelLossRed()-SCALE, 1.f);// MetaProgress::Instance().GetFuelLossRed() - 0.5f, 1.f);
	GUISprite* lowerFuelLossSprite = new GUISprite(*renderer, "Textures/lowerVolume.png", 900, uPosBar +uSpacing, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUISprite* higherFuelLossSprite = new GUISprite(*renderer, "Textures/higherVolume.png", 1250, uPosBar + uSpacing, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUIFont* fuelLossDisplay = new GUIFont(*renderer, "Fuel-Loss Reduction lv. ", 50 , uPos + uSpacing);
	fuelLossDisplay->SetFontSize({ fontsize , fontsize });
	fuelLossDisplay->SetVisible(false);
	fuelLossDisplay->AddGroup(GuiGroup::Upgrades);
	fuelLossDisplay->RemoveGroup(GuiGroup::Default);

	GUISprite* barFillFoodLoss = new GUISprite(*renderer, "Textures/blueBarFill.png", 1000, uPosBar + uSpacing*2, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Upgrades);
	barFillFoodLoss->SetScale(MetaProgress::Instance().GetFoodLossRed()-SCALE, 1.f);// (MetaProgress::Instance().GetFoodLossRed() - 0.5f, 1.f);
	GUISprite* lowerFoodLossSprite = new GUISprite(*renderer, "Textures/lowerVolume.png", 900, uPosBar + uSpacing*2, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUISprite* higherFoodLossSprite = new GUISprite(*renderer, "Textures/higherVolume.png", 1250, uPosBar + uSpacing*2, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUIFont* foodLossDisplay = new GUIFont(*renderer, "Food-Loss Reduction lv. ", 50, uPos + uSpacing*2);
	foodLossDisplay->SetFontSize({ fontsize , fontsize });
	foodLossDisplay->SetVisible(false);
	foodLossDisplay->AddGroup(GuiGroup::Upgrades);
	foodLossDisplay->RemoveGroup(GuiGroup::Default);

	GUISprite* barFillHpInFog = new GUISprite(*renderer, "Textures/blueBarFill.png", 1000, uPosBar + uSpacing*3, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Upgrades);
	barFillHpInFog->SetScale(MetaProgress::Instance().GetHpLossInFogRed()-SCALE,1.f);// MetaProgress::Instance().GetHpLossInFogRed() - 0.5f, 1.f);
	GUISprite* lowerHpInFogSprite = new GUISprite(*renderer, "Textures/lowerVolume.png", 900, uPosBar + uSpacing*3, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUISprite* higherHpInFogSprite = new GUISprite(*renderer, "Textures/higherVolume.png", 1250, uPosBar + uSpacing*3, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUIFont* hpInFogDisplay = new GUIFont(*renderer, "Fog-Resistance lv. ", 50, uPos + uSpacing * 3);
	hpInFogDisplay->SetFontSize({ fontsize,fontsize });
	hpInFogDisplay->SetVisible(false);
	hpInFogDisplay->AddGroup(GuiGroup::Upgrades);
	hpInFogDisplay->RemoveGroup(GuiGroup::Default);

	GUISprite* barFillHpFromEnemies = new GUISprite(*renderer, "Textures/blueBarFill.png", 1000, uPosBar + uSpacing*4, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Upgrades);
	barFillHpFromEnemies->SetScale(MetaProgress::Instance().GetDamageRed()-SCALE, 1.f);//MetaProgress::Instance().GetDamageRed() - 0.5f, 1.f);
	GUISprite* lowerHpFromEnemiesSprite = new GUISprite(*renderer, "Textures/lowerVolume.png", 900, uPosBar + uSpacing*4, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUISprite* higherHpFromEnemiesSprite = new GUISprite(*renderer, "Textures/higherVolume.png", 1250, uPosBar + uSpacing*4, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUIFont* enemyDamageRedDisplay = new GUIFont(*renderer, "Enemy Damage Reduction lv.", 50, uPos + uSpacing * 4);
	enemyDamageRedDisplay->SetFontSize({ fontsize,fontsize });
	enemyDamageRedDisplay->SetVisible(false);
	enemyDamageRedDisplay->AddGroup(GuiGroup::Upgrades);
	enemyDamageRedDisplay->RemoveGroup(GuiGroup::Default);

	GUISprite* barFillPlayerDamage = new GUISprite(*renderer, "Textures/blueBarFill.png", 1000, uPosBar + uSpacing*5, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Upgrades);
	barFillPlayerDamage->SetScale(MetaProgress::Instance().GetDamageBoost() - SCALE, 1.f);//MetaProgress::Instance().GetDamageBoost() - 0.5f, 1.f);
	GUISprite* lowerPlayerDamageSprite = new GUISprite(*renderer, "Textures/lowerVolume.png", 900, uPosBar + uSpacing * 5, 0, DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUISprite* higherPlayerDamageSprite = new GUISprite(*renderer, "Textures/higherVolume.png", 1250, uPosBar + uSpacing * 5,0 , DrawDirection::Default, ClickFunction::Clickable, GuiGroup::Upgrades);
	GUIFont* playerDamageBoost = new GUIFont(*renderer, "Player Damage Boost lv.", 50, uPos + uSpacing * 5);
	playerDamageBoost->SetFontSize({ fontsize,fontsize });
	playerDamageBoost->SetVisible(false);
	playerDamageBoost->AddGroup(GuiGroup::Upgrades);
	playerDamageBoost->RemoveGroup(GuiGroup::Default);

	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	fpsDisplay->AddGroup(GuiGroup::Font);
	fpsDisplay->AddGroup(GuiGroup::Default);
	// TEXT FOR HOWTOPLAY	

	std::string howToPlayString = "";
	howToPlayString += "Wasd: move around\n";
	howToPlayString += "Mouse: look around\n";
	howToPlayString += "Space: jump\n";
	howToPlayString += "Esc: pause\n";
	//howToPlayString += "V = First Person / Flying Camera\n";
	//howToPlayString += "O = Show / Hide Cursor\n";
	//howToPlayString += "Zero = Reset player pos\n\n";
	howToPlayString += "E: pick up and throw items\n";
	howToPlayString += "LMB: attack\n";
	howToPlayString += "RMB: interact with door and fire\n\n";

	howToPlayString += "Your goal is to survive and bring the house to the end of the road\n";
	howToPlayString += "You can gather food, fuel and healthpacks which are scattered\n    throughout the woods\n";
	howToPlayString += "Put the fuel into the fire inside the house so that the house can keep moving\n";
	howToPlayString += "But be aware of the monster which resides in the forest\n";

	GUIFont* howToPlayText = new GUIFont(*renderer, howToPlayString, 100, 250);
	howToPlayText->SetFontSize({ 0.7f,0.7f });
	howToPlayText->SetVisible(false);
	howToPlayText->RemoveGroup(GuiGroup::Default);
	howToPlayText->AddGroup(GuiGroup::HowToPlay);
	howToPlayText->AddGroup(GuiGroup::Font);

	// TEXT FOR HOWTOPLAY	
	std::string loreString = "";
	loreString += "Far back in the ancient times of Katrineholm, there is a dark and mysterious forest.\n";
	loreString += "Not long ago, the forest was a peaceful place, with animals inhabiting the place.\n";
	loreString += "But now the forest has been enchanted by an unknown being.\n";
	loreString += "A thick strange mist lingers in every corner of the forest, with a toxic effect to all who dares to enter.\n";
	loreString += "Our hero, Maiya, is stranded in the forest with her ancestors enchanted walking house, a house which she will now need more than ever.\n";
	loreString += "In order to make it out of the forest, Maiya needs to keep the house moving by refuelling it with fuel scattered in the forest.\n";
	loreString += "But be aware, there a more horrors than the mist in the deep forest.\n";

	GUIFont* loreText = new GUIFont(*renderer, loreString, 100, 250);
	loreText->SetFontSize({ 0.5f,0.5f });
	loreText->SetVisible(false);
	loreText->RemoveGroup(GuiGroup::Default);
	loreText->AddGroup(GuiGroup::Lore);
	loreText->AddGroup(GuiGroup::Font);

	backSprite->SetVisible(false);
	backSprite->AddGroup(GuiGroup::Lore);
	backSprite->AddGroup(GuiGroup::HowToPlay);
	backSprite->AddGroup(GuiGroup::Options);
	backSprite->AddGroup(GuiGroup::Upgrades);
	guiManager = new GUIManager(renderer, 100);
	guiManager->AddGUIObject(loadSprite, "loading");
	guiManager->AddGUIObject(titleSprite, "title");
	guiManager->AddGUIObject(startSprite, "startSprite");
	guiManager->AddGUIObject(backSprite, "backSprite");
	guiManager->AddGUIObject(howToPlaySprite, "howToPlaySprite");
	guiManager->AddGUIObject(upgradesSprite, "upgradesSprite");
	guiManager->AddGUIObject(optionSprite, "optionSprite");
	guiManager->AddGUIObject(credits, "credits");
	guiManager->AddGUIObject(quitSprite, "quitSprite");
	guiManager->AddGUIObject(loreSprite, "loreSprite");
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(howToPlayText, "howToPlayText");
	guiManager->AddGUIObject(loreText, "loreText");
	

	/* Soundseffects */
	guiManager->AddGUIObject(soundEffectsSprite, "soundeffectText");
	guiManager->AddGUIObject(volumeBarFillSoundeffects, "volumeBarFillSoundeffects");
	guiManager->AddGUIObject(lowerSoundEffectMusic, "lowerSoundeffectSprite");
	guiManager->AddGUIObject(volumeSoundEffectMusic, "volumeSoundeffectSprite");
	guiManager->AddGUIObject(higherSoundEffectMusic, "higherSoundeffectSprite");

	/* Music */
	guiManager->AddGUIObject(musicSprite, "musicText");
	guiManager->AddGUIObject(volumeBarFillMusic, "volumeBarFillMusic");
	guiManager->AddGUIObject(lowerMusicSprite, "lowerMusicSprite");
	guiManager->AddGUIObject(volumeMusicSprite, "volumeMusicSprite");
	guiManager->AddGUIObject(higherMusicSprite, "higherMusicSprite");

	/* Sensitivity */
	guiManager->AddGUIObject(sensitivitySprite, "sensitivityText");
	//guiManager->AddGUIObject(sensitivityBarFill, "sensitivityBarFill");
	guiManager->AddGUIObject(lowerSensitivitySprite, "lowerSensitivitySprite");
	//guiManager->AddGUIObject(volumeSensitivtySprite, "guiManager->AddGUIObject");
	guiManager->AddGUIObject(higherSensitivitySprite, "higherSensitivitySprite");
	guiManager->AddGUIObject(sensitivityDisplay, "sensitivityDisplay");



	//upgrademenu
	guiManager->AddGUIObject(upgradeTitle, "upgradeTitle");
	guiManager->AddGUIObject(reset, "reset");
	guiManager->AddGUIObject(currencyAmount, "currencyAmount");
	//guiManager->AddGUIObject(upgradeReset, "upgradeReset");
	guiManager->AddGUIObject(barFillFuelLoss, "barFillFuelLoss");
	guiManager->AddGUIObject(lowerFuelLossSprite, "lowerFuelLossSprite");
	guiManager->AddGUIObject(higherFuelLossSprite, "higherFuelLossSprite");
	guiManager->AddGUIObject(fuelLossDisplay, "fuelLossDisplay");

	guiManager->AddGUIObject(barFillFoodLoss, "barFillFoodLoss");
	guiManager->AddGUIObject(lowerFoodLossSprite, "lowerFoodLossSprite");
	guiManager->AddGUIObject(higherFoodLossSprite, "higherFoodLossSprite");
	guiManager->AddGUIObject(foodLossDisplay, "foodLossDisplay");

	guiManager->AddGUIObject(barFillHpInFog, "barFillHpInFog");
	guiManager->AddGUIObject(lowerHpInFogSprite, "lowerHpInFogSprite");
	guiManager->AddGUIObject(higherHpInFogSprite, "higherHpInFogSprite");
	guiManager->AddGUIObject(hpInFogDisplay, "hpInFogDisplay");

	guiManager->AddGUIObject(barFillHpFromEnemies, "barFillHpFromEnemies");
	guiManager->AddGUIObject(lowerHpFromEnemiesSprite, "lowerHpFromEnemiesSprite");
	guiManager->AddGUIObject(higherHpFromEnemiesSprite, "higherHpFromEnemiesSprite");
	guiManager->AddGUIObject(enemyDamageRedDisplay, "enemyDamageRedDisplay");

	guiManager->AddGUIObject(barFillPlayerDamage, "barFillPlayerDamage");
	guiManager->AddGUIObject(lowerPlayerDamageSprite, "lowerPlayerDamageSprite");
	guiManager->AddGUIObject(higherPlayerDamageSprite, "higherPlayerDamageSprite");
	guiManager->AddGUIObject(playerDamageBoost, "playerDamageBoost");
	

}

void IntroScene::OnActivate()
{
	input.SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	//ShowCursor(true);
	renderer->AddRenderPass(guiManager);
	guiManager->ChangeGuiGroup(GuiGroup::Default);
	float musicVolume = Config::GetFloat("volumeMusic", 0.5f);
	AudioMaster::Instance().SetVolume(AudioTypes::Music, musicVolume);
	static_cast<GUISprite*>(guiManager->GetGUIObject("volumeBarFillMusic"))->SetScale(musicVolume, 1);
	SaveState state;
	SaveHandler::TryLoad(state);
	MetaProgress::Instance().LoadSave(state);


	float soundVolume = Config::GetFloat("volumeSound", 0.5f);
	AudioMaster::Instance().SetVolume(AudioTypes::Sound, soundVolume);
	static_cast<GUISprite*>(guiManager->GetGUIObject("volumeBarFillSoundeffects"))->SetScale(soundVolume, 1);

		
	static_cast<GUISprite*>(guiManager->GetGUIObject("barFillFuelLoss"))->SetScale(MetaProgress::Instance().GetFuelLossRed() - SCALE, 1.f);
	static_cast<GUISprite*>(guiManager->GetGUIObject("barFillFoodLoss"))->SetScale(MetaProgress::Instance().GetFoodLossRed() - SCALE, 1.f);
	static_cast<GUISprite*>(guiManager->GetGUIObject("barFillHpInFog"))->SetScale(MetaProgress::Instance().GetHpLossInFogRed() - SCALE, 1.f);
	static_cast<GUISprite*>(guiManager->GetGUIObject("barFillHpFromEnemies"))->SetScale(MetaProgress::Instance().GetDamageRed() - SCALE, 1.f);
	static_cast<GUISprite*>(guiManager->GetGUIObject("barFillPlayerDamage"))->SetScale(MetaProgress::Instance().GetDamageBoost() - SCALE, 1.f);

	configChanged = false;

	AudioMaster::Instance().PlaySoundEvent("menusound");
}

void IntroScene::OnDeactivate()
{
	AudioMaster::Instance().StopSoundEvent("menusound");
	renderer->RemoveRenderPass(guiManager);
}

void IntroScene::Update(const float& deltaTime)
{
	if (!Input::Instance().GetIsVisible())
		ShowCursor(true);

	Scene::Update(deltaTime);
	//Cleanup Later
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));
	std::string sTxt = std::to_string(this->sensitivity);
	sTxt.erase(sTxt.begin() + 4, sTxt.end());
	static_cast<GUIFont*>(guiManager->GetGUIObject("sensitivityDisplay"))->SetString(sTxt);
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("startSprite"))->IsClicked())
	{
		guiManager->ChangeGuiGroup(GuiGroup::Load);
		AudioMaster::Instance().StopSoundEvent("menusound");
		Engine::Instance->SwitchScene(SceneIndex::GAME);
		return;
	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("credits"))->IsClicked())
	{
		//guiManager->ChangeGuiGroup(GuiGroup::Load);
		AudioMaster::Instance().PlaySoundEvent("menusound");
		Engine::Instance->SwitchScene(SceneIndex::CREDITS);
		return;
		//AudioMaster::Instance().StopSoundEvent("menusound");
		//Engine::Instance->SwitchScene(SceneIndex::INTRO);
		//return;

	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("quitSprite"))->IsClicked())
	{
		AudioMaster::Instance().StopSoundEvent("menusound");
		OnDeactivate();
		Engine::Instance->Exit();
		return;
	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("loreSprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::Lore);

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("optionSprite"))->IsClicked())
	{
		this->sensitivity = Config::GetFloat("sensitivity", 0.5f);
		guiManager->ChangeGuiGroup(GuiGroup::Options);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("upgradesSprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::Upgrades);

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("howToPlaySprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::HowToPlay);

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("backSprite"))->IsClicked())
	{
		if (configChanged)
		{
			Config::Save();
			configChanged = false;
		}

		guiManager->ChangeGuiGroup(GuiGroup::Default);
	}

	/* Volume stuff for music */
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerMusicSprite"))->IsClicked())
	{
		float currentVol = AudioMaster::Instance().GetVolume(AudioTypes::Music);
		if (currentVol > 0.0f)
			currentVol -= 0.1f;
		else
			currentVol = 0.0f;

		Config::SetFloat("volumeMusic", currentVol);
		configChanged = true;

		AudioMaster::Instance().SetVolume(AudioTypes::Music, currentVol);
		static_cast<GUISprite*>(guiManager->GetGUIObject("volumeBarFillMusic"))->SetScale(currentVol, 1);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("higherMusicSprite"))->IsClicked())
	{
		float currentVol = AudioMaster::Instance().GetVolume(AudioTypes::Music);
		if (currentVol < 1.0f)
			currentVol += 0.1f;
		else
			currentVol = 1.0f;

		Config::SetFloat("volumeMusic", currentVol);
		configChanged = true;

		AudioMaster::Instance().SetVolume(AudioTypes::Music, currentVol);
		static_cast<GUISprite*>(guiManager->GetGUIObject("volumeBarFillMusic"))->SetScale(currentVol, 1);
	}

	/* Volume stuff for soundeffects */
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerSoundeffectSprite"))->IsClicked())
	{
		float currentVol = AudioMaster::Instance().GetVolume(AudioTypes::Sound);
		if (currentVol > 0.0f)
			currentVol -= 0.1f;
		else
			currentVol = 0.0f;

		Config::SetFloat("volumeSound", currentVol);
		configChanged = true;

		AudioMaster::Instance().SetVolume(AudioTypes::Sound, currentVol);
		static_cast<GUISprite*>(guiManager->GetGUIObject("volumeBarFillSoundeffects"))->SetScale(currentVol, 1);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("higherSoundeffectSprite"))->IsClicked())
	{
		float currentVol = AudioMaster::Instance().GetVolume(AudioTypes::Sound);
		if (currentVol < 1.0f)
			currentVol += 0.1f;
		else
			currentVol = 1.0f;

		Config::SetFloat("volumeSound", currentVol);
		configChanged = true;

		AudioMaster::Instance().SetVolume(AudioTypes::Sound, currentVol);
		static_cast<GUISprite*>(guiManager->GetGUIObject("volumeBarFillSoundeffects"))->SetScale(currentVol, 1);
	}

	/* Volume stuff for sensitivity */
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerSensitivitySprite"))->IsClicked())
	{
		//lower sense
		if (this->sensitivity > 0.05f)
			this->sensitivity -= 0.05f;
		else
			this->sensitivity = 0.05f;

		Config::SetFloat("sensitivity", sensitivity);
		configChanged = true;
		//static_cast<GUISprite*>(guiManager->GetGUIObject("sensitivityBarFill"))->SetScale(this->sensitivity, 1.f);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("higherSensitivitySprite"))->IsClicked())
	{
		//higher sense 
		if (this->sensitivity < 1.0f)
			this->sensitivity += 0.05f;
		else
			this->sensitivity = 1.0f;

		Config::SetFloat("sensitivity", sensitivity);
		configChanged = true;
		//static_cast<GUISprite*>(guiManager->GetGUIObject("sensitivityBarFill"))->SetScale(sensitivity, 1.f);
	}


	/* Upgrade stuff*/
	std::string uText = "U-Currency: ";
	uText += std::to_string(MetaProgress::Instance().GetCurrencyTotal());
	static_cast<GUIFont*>(guiManager->GetGUIObject("currencyAmount"))->SetString(uText);
	float max = 1 + SCALE;
	float mult = 1 / INCREASE;

	unsigned int currency = MetaProgress::Instance().GetCurrencyTotal();

	bool saving = false;
	//fuel loss reduction
	std::string fuellossText = "Fuel-Loss Reduction lv." + std::to_string(int((MetaProgress::Instance().GetFuelLossRed()-1.f) * mult)) + " -> Cost: " + std::to_string(int(COST * MetaProgress::Instance().GetFuelLossRed()));
	static_cast<GUIFont*>(guiManager->GetGUIObject("fuelLossDisplay"))->SetString(fuellossText);
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerFuelLossSprite"))->IsClicked())
	{
		if (MetaProgress::Instance().GetFuelLossRed()> 1.f)
		{
			MetaProgress::Instance().UseCurrency(COST * MetaProgress::Instance().GetFuelLossRed());
			MetaProgress::Instance().ChangeFuelLossRed(-INCREASE);
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillFuelLoss"))->SetScale(MetaProgress::Instance().GetFuelLossRed() - SCALE, 1.f);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("higherFuelLossSprite"))->IsClicked())
	{
		if (currency >= COST * MetaProgress::Instance().GetFuelLossRed() && MetaProgress::Instance().GetFuelLossRed() < max )
		{
			MetaProgress::Instance().ChangeFuelLossRed(INCREASE);
			MetaProgress::Instance().UseCurrency(-COST * MetaProgress::Instance().GetFuelLossRed());
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillFuelLoss"))->SetScale(MetaProgress::Instance().GetFuelLossRed() - SCALE, 1.f);
	}
	//food loss reduction
	//std::string foodtext;
	std::string foodlossText = "Food-Loss Reduction lv. " + std::to_string(int((MetaProgress::Instance().GetFoodLossRed() - 1.f) * mult)) + " -> Cost: " + std::to_string(int(COST * MetaProgress::Instance().GetFoodLossRed()));
	static_cast<GUIFont*>(guiManager->GetGUIObject("foodLossDisplay"))->SetString(foodlossText);
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerFoodLossSprite"))->IsClicked())
	{
		if (MetaProgress::Instance().GetFoodLossRed() > 1.f)
		{
			MetaProgress::Instance().UseCurrency(COST * MetaProgress::Instance().GetFoodLossRed());
			MetaProgress::Instance().ChangeFoodLossRed(-INCREASE);
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillFoodLoss"))->SetScale(MetaProgress::Instance().GetFoodLossRed() - SCALE, 1.f);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("higherFoodLossSprite"))->IsClicked())
	{
		if (currency >= + COST * MetaProgress::Instance().GetFoodLossRed() && MetaProgress::Instance().GetFoodLossRed() < max )
		{
			MetaProgress::Instance().ChangeFoodLossRed(INCREASE);
			MetaProgress::Instance().UseCurrency(-COST * MetaProgress::Instance().GetFoodLossRed());
			
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillFoodLoss"))->SetScale(MetaProgress::Instance().GetFoodLossRed() - SCALE, 1.f);
	}
	//hploss in fog reduction
	std::string fogResText = "Fog-Resistance lv. " + std::to_string(int((MetaProgress::Instance().GetHpLossInFogRed() - 1.f) * mult)) + " -> Cost: " + std::to_string(int(COST * MetaProgress::Instance().GetHpLossInFogRed()));
	static_cast<GUIFont*>(guiManager->GetGUIObject("hpInFogDisplay"))->SetString(fogResText);
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerHpInFogSprite"))->IsClicked())
	{
		if (MetaProgress::Instance().GetHpLossInFogRed() > 1.f)
		{
			MetaProgress::Instance().UseCurrency(COST * MetaProgress::Instance().GetHpLossInFogRed());
			MetaProgress::Instance().ChangeHpLossFogRed(-INCREASE);
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillHpInFog"))->SetScale(MetaProgress::Instance().GetHpLossInFogRed() - SCALE, 1.f);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("higherHpInFogSprite"))->IsClicked())
	{
		if (currency >= COST * MetaProgress::Instance().GetHpLossInFogRed() && MetaProgress::Instance().GetHpLossInFogRed()<max)
		{
			MetaProgress::Instance().ChangeHpLossFogRed(INCREASE);
			MetaProgress::Instance().UseCurrency(-COST * MetaProgress::Instance().GetHpLossInFogRed());
			
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillHpInFog"))->SetScale(MetaProgress::Instance().GetHpLossInFogRed() - SCALE, 1.f);
	}
	//hp from enemies reduction
	std::string enemyDamageText = "Enemy Damage Reduction lv." + std::to_string(int((MetaProgress::Instance().GetDamageRed() - 1.f) * mult)) + " -> Cost: " + std::to_string(int(COST * MetaProgress::Instance().GetDamageRed()));
	static_cast<GUIFont*>(guiManager->GetGUIObject("enemyDamageRedDisplay"))->SetString(enemyDamageText);
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerHpFromEnemiesSprite"))->IsClicked())
	{
		if (MetaProgress::Instance().GetDamageRed() > 1.f)
		{
			MetaProgress::Instance().UseCurrency(COST* MetaProgress::Instance().GetDamageRed());
			MetaProgress::Instance().ChangeEnemyDamageRed(-INCREASE);
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillHpFromEnemies"))->SetScale(MetaProgress::Instance().GetDamageRed() - SCALE, 1.f);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("higherHpFromEnemiesSprite"))->IsClicked())
	{
		if (currency >= COST * MetaProgress::Instance().GetDamageRed() && MetaProgress::Instance().GetDamageRed()<max)
		{
			MetaProgress::Instance().ChangeEnemyDamageRed(INCREASE);
			MetaProgress::Instance().UseCurrency(-COST * MetaProgress::Instance().GetDamageRed());
			
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillHpFromEnemies"))->SetScale(MetaProgress::Instance().GetDamageRed() - SCALE, 1.f);
	}
	//damage boost
	std::string playerDamageText = "Player Damage Boost lv." + std::to_string(int((MetaProgress::Instance().GetDamageBoost() - 1.f) * mult)) + " -> Cost: " + std::to_string(int(COST * MetaProgress::Instance().GetDamageBoost()));
	static_cast<GUIFont*>(guiManager->GetGUIObject("playerDamageBoost"))->SetString(playerDamageText);
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerPlayerDamageSprite"))->IsClicked())
	{
		if (MetaProgress::Instance().GetDamageBoost() > 1.f)
		{
			MetaProgress::Instance().UseCurrency(COST * MetaProgress::Instance().GetDamageBoost());
			MetaProgress::Instance().ChangePlayerDamageBoost(-INCREASE);
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillPlayerDamage"))->SetScale(MetaProgress::Instance().GetDamageBoost() - SCALE, 1.f);
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("higherPlayerDamageSprite"))->IsClicked())
	{
		if (currency >= COST * MetaProgress::Instance().GetDamageBoost() && MetaProgress::Instance().GetDamageBoost()<max)
		{
			MetaProgress::Instance().ChangePlayerDamageBoost(INCREASE);
			MetaProgress::Instance().UseCurrency(-COST * MetaProgress::Instance().GetDamageBoost());
			
			saving = true;
		}
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillPlayerDamage"))->SetScale(MetaProgress::Instance().GetDamageBoost() - SCALE, 1.f);
	}

	

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("reset"))->IsClicked())
	{
		// reset is scuffed atm ..
		SaveState state = SaveHandler::CreateNew();
		MetaProgress::Instance().LoadSave(state);
		MetaProgress::Instance().SaveProgress(state);

		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillFuelLoss"))->SetScale(MetaProgress::Instance().GetFuelLossRed() - SCALE, 1.f);
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillFoodLoss"))->SetScale(MetaProgress::Instance().GetFoodLossRed() - SCALE, 1.f);
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillHpInFog"))->SetScale(MetaProgress::Instance().GetHpLossInFogRed() - SCALE, 1.f);
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillHpFromEnemies"))->SetScale(MetaProgress::Instance().GetDamageRed() - SCALE, 1.f);
		static_cast<GUISprite*>(guiManager->GetGUIObject("barFillPlayerDamage"))->SetScale(MetaProgress::Instance().GetDamageBoost() - SCALE, 1.f);
		static_cast<GUIFont*>(guiManager->GetGUIObject("playerDamageBoost"))->SetString(playerDamageText);
		static_cast<GUIFont*>(guiManager->GetGUIObject("enemyDamageRedDisplay"))->SetString(enemyDamageText);
		static_cast<GUIFont*>(guiManager->GetGUIObject("hpInFogDisplay"))->SetString(fogResText);
		static_cast<GUIFont*>(guiManager->GetGUIObject("foodLossDisplay"))->SetString(foodlossText);
		static_cast<GUIFont*>(guiManager->GetGUIObject("fuelLossDisplay"))->SetString(fuellossText);
		static_cast<GUIFont*>(guiManager->GetGUIObject("currencyAmount"))->SetString(uText);
	}


	if (saving)
	{
		SaveState state;
		SaveHandler::TryLoad(state);
		MetaProgress::Instance().SaveProgress(state);
		SaveHandler::Save(state);
	}

	guiManager->UpdateAll();
}

void IntroScene::FixedUpdate(const float& fixedDeltaTime)
{
	AnimateIcon();
	Scene::FixedUpdate(fixedDeltaTime);
}

void IntroScene::ChangeBarColor()
{
	std::string blue = "Textures/blueBarFill.png";
	std::string green = "Textures/volumeBarFill.png";
	//static_cast<GUISprite*>(guiManager->GetGUIObject("barFillPlayerDamage"))->SetDDSSprite(renderer->GetDevice(),"")
}
