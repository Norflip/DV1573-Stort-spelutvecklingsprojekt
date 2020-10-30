#include "stdafx.h"
#include "IntroScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"

IntroScene::IntroScene(ResourceManager* manager) : Scene(manager)
{
	
}

IntroScene::~IntroScene()
{
	AudioMaster::Instance().StopSoundEvent(menuTest);
}

void IntroScene::Initialize(Renderer* renderer)
{
	
	this->renderer = renderer;

	/*Physics& physics = Physics::Instance();
	physics.Initialize({ 0, -10.0f, 0 });*/

	// Should change values on resize event
	this->window = renderer->GetOutputWindow();

	//Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
	input.SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void IntroScene::InitializeObjects()
{
	//audioComponent.LoadFile(L"Sounds/PopCulture.mp3", menuTest, AudioTypes::Music);
	AudioMaster::Instance().LoadFile(L"Sounds/jakestuff.mp3", menuTest, AudioTypes::Music);
	AudioMaster::Instance().LoadFile(L"Sounds/yay.wav", test2, AudioTypes::Sound);
	AudioMaster::Instance().SetVolume(AudioTypes::Music, 0.7f);
	AudioMaster::Instance().PlaySoundEvent(menuTest);

	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(window->GetWidth(), window->GetHeight());
	//cameraObject2->AddComponent<ControllerComponent>();
	this->player = cameraObject;
	AddObject(cameraObject);

	ShowCursor(true); 

	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
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
	GUISprite* quitSprite = new GUISprite(*renderer, "Textures/Exit.png", 100, 700, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* backSprite = new GUISprite(*renderer, "Textures/BackButton.png", 100, 100, 0, DrawDirection::Default, ClickFunction::Clickable,GuiGroup::HowToPlay);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	fpsDisplay->AddGroup(GuiGroup::Font);
	fpsDisplay->AddGroup(GuiGroup::Default);
	// TEXT FOR HOWTOPLAY	

	std::string howToPlayString = "";
	howToPlayString += "Wasd: move around\n";
	howToPlayString += "Mouse to aim\n";
	howToPlayString += "V = First Person / Flying Camera\n";
	howToPlayString += "O = Show / Hide Cursor\n";
	howToPlayString += "Zero = Reset player pos\n\n";
	howToPlayString += " Your goal is to survive and bring the house to the end of the road\n";
	howToPlayString += " You can gather food, fuel and healthpacks that are scattered \n    throughout the woods\n";
	GUIFont* howToPlayText = new GUIFont(*renderer,howToPlayString, 100, 250);
	howToPlayText->SetFontSize({ 0.7f,0.7f });
	howToPlayText->SetVisible(false);
	howToPlayText->RemoveGroup(GuiGroup::Default);
	howToPlayText->AddGroup(GuiGroup::HowToPlay);
	howToPlayText->AddGroup(GuiGroup::Font);
	
	//
	// TEXT FOR HOWTOPLAY	
	std::string loreString = "";
	loreString += "The name Katrineholm comes from the farm Cathrineholm by Lake Nasnaren. \nThe farm was formerly called Fulbonas\nbut the then owner Jacob von der Linde changed the name in the 1660s to honor his daughter Catharina. \nMany finds show that the area was already inhabited over 6,000 years ago.";
	
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
	renderer->AddRenderPass(guiManager);
}

void IntroScene::OnActivate()
{
	root = new Object("sceneRoot", ObjectFlag::DEFAULT);
	nextScene = INTRO;
	input.SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	InitializeGUI();
	InitializeObjects();
}

void IntroScene::OnDeactivate()
{
	renderer->RemoveRenderPass(guiManager);
	delete root;
	root = nullptr;
}

void IntroScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);

	//Cleanup Later
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("startSprite"))->IsClicked())
	{
		AudioMaster::Instance().StopSoundEvent(menuTest);
		nextScene = GAME;
	}
	else if (static_cast<GUISprite*>(guiManager->GetGUIObject("quitSprite"))->IsClicked())
	{
		AudioMaster::Instance().StopSoundEvent(menuTest);
		quit = true;
	}	

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("quitSprite"))->IsClicked())
		quit = true;

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("loreSprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::Lore);

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("optionSprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::Options);

	 if (static_cast<GUISprite*>(guiManager->GetGUIObject("howToPlaySprite"))->IsClicked())
		guiManager->ChangeGuiGroup(GuiGroup::HowToPlay);

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("backSprite"))->IsClicked())	
		guiManager->ChangeGuiGroup(GuiGroup::Default);


	guiManager->UpdateAll();
}

void IntroScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);
}

void IntroScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	//worldGenerator.DrawShapes();

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

