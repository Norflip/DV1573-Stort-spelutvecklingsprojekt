#include "stdafx.h"
#include "IntroScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"

IntroScene::IntroScene() : Scene("IntroScene")
{
	
}

IntroScene::~IntroScene()
{

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

	ShowCursor(true); 

	AudioMaster::Instance().PlaySoundEvent("menusound");
}

void IntroScene::InitializeGUI()
{
	float windowWidth = FCAST(window->GetWidth());

	//spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* titleSprite = new GUISprite(*renderer, "Textures/OnceUponATime.png", windowWidth / 2 - 100, 100, 0, DrawDirection::Default, ClickFunction::NotClickable);
	GUISprite* startSprite = new GUISprite(*renderer, "Textures/Start.png", 100, 100, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* howToPlaySprite = new GUISprite(*renderer, "Textures/howToPlay.png", 20, 250, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* optionSprite = new GUISprite(*renderer, "Textures/Options.png", 100, 400, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* loreSprite = new GUISprite(*renderer, "Textures/Lore.png", 100, 550, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quitSprite = new GUISprite(*renderer, "Textures/Exit.png", 100, 850, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* backSprite = new GUISprite(*renderer, "Textures/BackButton.png", 100, 100, 0, DrawDirection::Default, ClickFunction::Clickable,GuiGroup::HowToPlay);
	GUISprite* loadSprite = new GUISprite(*renderer, "Textures/Loading.png", 0, 0, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Load);
	GUISprite* musicSprite = new GUISprite(*renderer, "Textures/Music.png", 110, 250, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);
	musicSprite->SetVisible(false);
	loadSprite->SetVisible(false);
	GUISprite* soundEffectsSprite = new GUISprite(*renderer, "Textures/SoundeffectsButton.png", 160, 400, 0, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Options);	
	soundEffectsSprite->SetVisible(false);


	//make Textures/Credits.png
	GUISprite* credits = new GUISprite(*renderer, "Textures/credits.png",100, 700, 0, DrawDirection::Default, ClickFunction::Clickable);
	credits->AddGroup(GuiGroup::Default);


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

	GUIFont* howToPlayText = new GUIFont(*renderer,howToPlayString, 100, 250);
	howToPlayText->SetFontSize({ 0.7f,0.7f });
	howToPlayText->SetVisible(false);
	howToPlayText->RemoveGroup(GuiGroup::Default);
	howToPlayText->AddGroup(GuiGroup::HowToPlay);
	howToPlayText->AddGroup(GuiGroup::Font);
	
	//
	// TEXT FOR HOWTOPLAY	
	std::string loreString = "";
	loreString += "The name Katrineholm comes from the farm Cathrineholm which is located at Lake Nasnaren. \n";
	loreString += "The farm was formerly called Fulbonas,\n";
	loreString += "but the former owner Jacob von der Linde changed the name in the 1660s to honor his daughter Catharina. \n";
	loreString += "Many findings show that the area was already inhabited over 6, 000 years ago.";
	
	GUIFont* loreText = new GUIFont(*renderer, loreString, 100, 250);
	loreText->SetFontSize({ 0.5f,0.5f });
	loreText->SetVisible(false);
	loreText->RemoveGroup(GuiGroup::Default);
	loreText->AddGroup(GuiGroup::Lore);
	loreText->AddGroup(GuiGroup::Font);

	//

	backSprite->SetVisible(false);
	backSprite->AddGroup(GuiGroup::Lore);
	backSprite->AddGroup(GuiGroup::HowToPlay);
	backSprite->AddGroup(GuiGroup::Options);
	guiManager = new GUIManager(renderer, 100);
	guiManager->AddGUIObject(loadSprite, "loading");
	guiManager->AddGUIObject(titleSprite, "title");
	guiManager->AddGUIObject(startSprite, "startSprite");
	guiManager->AddGUIObject(backSprite, "backSprite");
	guiManager->AddGUIObject(howToPlaySprite, "howToPlaySprite");
	guiManager->AddGUIObject(optionSprite, "optionSprite");
	guiManager->AddGUIObject(quitSprite, "quitSprite");
	guiManager->AddGUIObject(loreSprite, "loreSprite");
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(howToPlayText, "howToPlayText");
	guiManager->AddGUIObject(loreText, "loreText");

	guiManager->AddGUIObject(credits, "credits");

	//

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

}

void IntroScene::OnActivate()
{
	input.SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	renderer->AddRenderPass(guiManager);
}

void IntroScene::OnDeactivate()
{
	AudioMaster::Instance().StopSoundEvent("menusound");
	renderer->RemoveRenderPass(guiManager);
}

void IntroScene::Update(const float& deltaTime)
{

	Scene::Update(deltaTime);
	//Cleanup Later
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("startSprite"))->IsClicked())
	{
		guiManager->ChangeGuiGroup(GuiGroup::Load);
		AudioMaster::Instance().StopSoundEvent("menusound");
		Engine::Instance->SwitchScene(SceneIndex::GAME);
		return;
	}

	//
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
	//

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("quitSprite"))->IsClicked())
	{
		AudioMaster::Instance().StopSoundEvent("menusound");
		Engine::Instance->Exit();
		return;
	}	

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("loreSprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::Lore);

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("optionSprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::Options);

	 if (static_cast<GUISprite*>(guiManager->GetGUIObject("howToPlaySprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::HowToPlay);

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("backSprite"))->IsClicked())	
		guiManager->ChangeGuiGroup(GuiGroup::Default);


	/* Volume stuff for music */
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("lowerMusicSprite"))->IsClicked())
	{
		float currentVol = AudioMaster::Instance().GetVolume(AudioTypes::Music);
		if (currentVol > 0.0f)
			currentVol -= 0.1f;
		else
			currentVol = 0.0f;

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

		AudioMaster::Instance().SetVolume(AudioTypes::Sound, currentVol);
		static_cast<GUISprite*>(guiManager->GetGUIObject("volumeBarFillSoundeffects"))->SetScale(currentVol, 1);
	}


	guiManager->UpdateAll();
}

void IntroScene::FixedUpdate(const float& fixedDeltaTime)
{
	AnimateIcon();
	Scene::FixedUpdate(fixedDeltaTime);
}
