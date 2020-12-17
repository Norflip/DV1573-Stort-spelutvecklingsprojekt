#include "stdafx.h"
#include "CreditsScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"

std::string CreditsScene::GetCreditsString()
{
	//the current font does not allow the use of "�","�","�" and tab - "	"
	//should text be read from file?
	//or defined as constants?
	//should every category have an own GUIFont entry?? (with title as yellow color and content as white)
	std::string tab = "    ";
	std::string gameName = "Once upon a Time in Katrineholm";
	std::string credits = "Staff Credits";
	std::string course = std::string("Kurs/Course\n") + "DV1573/UD1448 H20 lp12 "+"\n"+"Stort spelutvecklingsprojekt i grupp med agil metodik\n" + tab +"(The Great Gameproject course)";
	std::string school = "BTH - Blekinge Tekniska Hogskola";

	std::string gameIdea = "Game Idea by"; 
	std::string group = "Grupp 1 / Group 1";

	std::string roleLead = "Project Leader";
	std::string roleSystem = "System & Engine Programming";
	std::string roleGraphicsProgamming = "Graphics Programming";
	std::string roleProgramming = "Game Logic Programming"; //Enemyprogramming & Playerprogramming, Houseprogramming;
	std::string roleGui = "GUI Programming";
	std::string roleBugfix = "Misc. and bug fix programming";
	std::string role3DArt = "3D Models/Art";
	std::string roleMusicSound = "Music & Sound";
	std::string roleScrumMaster = "Scrum Master";

	std::string thirdParty = std::string("Third Party: \n")
		+ tab + "React physics 3D" + "\n"
		+ tab + "ZWEB-Loader by Emil" + "\n"
		+ tab + "DirectX 11" + "\n"
		+ tab + "xAudio2" + "\n";
	std::string specialThanks = "Special Thanks";
	std::string testing;
	std::string thanks = std::string("The End!") + "\n" + "Thanks for Playing!";
	

	std::string p_SP[2] = { "SP", "Spel-Programmering" };
	std::string p_TA[2] = { "TA","Techical-Artist" };
	std::string m_Emil = "Emil Johansson";
	std::string m_Filip = "Filip Norgren";
	std::string m_Fredrik = "Fredrik Galfi";
	std::string m_Jakob = "Jakob Lidborn";
	std::string m_John = "John Andersson";
	std::string m_Karlos = "Karlos Napa Hager";
	std::string m_Patrik = "Patrik Svensson";
	std::string m_Tristan = "Tristan Wikstrom";
	std::string m_Viktor = "Viktor Wallin";
	
	std::string theCredits;

	theCredits += gameName + "\n" + credits + "\n\n";
	theCredits += course + "\n" + school + "\n\n";
	theCredits += gameIdea + "\n" + group + "\n"
		+ tab + m_Emil + "\n" + tab + m_Filip + "\n" + tab + m_Fredrik + "\n"
		+ tab + m_Jakob + "\n" + tab + m_John + "\n" + tab + m_Karlos + "\n"
		+ tab + m_Patrik + "\n" + tab + m_Tristan + "\n" + tab + m_Viktor + "\n\n";
	theCredits += roleLead + "\n" +tab+ m_Jakob + "\n\n";
	theCredits += roleSystem + "\n" +tab+ m_Filip + "\n\n";
	theCredits += roleGraphicsProgamming + "\n"+tab + "All" + "\n\n";
	theCredits += roleProgramming + "\n" +tab+ "All" + "\n\n";
	theCredits += roleGui + "\n" +tab+ m_Jakob + "\n\n";
	theCredits += roleBugfix + "\n" +tab+ "All" + "\n\n";
	theCredits += role3DArt + "\n" +tab+ m_Viktor + "\n\n";
	theCredits += roleMusicSound + "\n"+tab + m_Patrik + "\n\n";
	theCredits += roleScrumMaster + "\n"+tab + m_Patrik + "\n"+tab + m_Emil + "\n\n";
	theCredits += thirdParty + "\n\n";
	theCredits += specialThanks + "\n" +tab+ "Everyone who tested our game during the internal\nand external testing phase" + "\n\n";
	theCredits += tab + "Teachers in the course" + "\n"
		+ tab + "Hans Tap" + "\n" + tab + "Stefan Petersson" + "\n" + tab + "Erik Wadstein"+ "\n"
		+ tab + "Marcus Holmberg" + "\n\n";

	theCredits += "\n\n" + thanks;
	
	
	//theCredits += thanks + tab + "o";


	//Title (yellow)
	//	text (white)
	//	text
	//float windowWidth = static_cast<float>(window->GetWidth());
	//creditsPos = { windowWidth / 2, 100 };
	//std::string text;
	//GUIFont* credits = new GUIFont(*renderer, text, creditsPos.x, creditsPos.y);

	////guiManager= new GUIManager(renderer, 100);
	//guiManager->AddGUIObject(credits, "credits");
	//guiManager->AddGUIObject(credits, "as");
	return theCredits;
	//return "\tyoo";
}

CreditsScene::CreditsScene() : Scene("CreditsScene")
{
	creditsPos = { 0.f,0.f };
}

CreditsScene::~CreditsScene()
{
	delete guiManager;
}

void CreditsScene::Initialize()
{
	InitializeGUI();
	InitializeObjects();
}

void CreditsScene::InitializeObjects()
{
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(window->GetWidth(), window->GetHeight(), 60.0f);
	this->player = cameraObject;
	AddObjectToRoot(cameraObject);
	//ShowCursor(true);
}

void CreditsScene::InitializeGUI()
{
	float windowWidth = static_cast<float>(window->GetWidth());

	GUISprite* returnToMenu = new GUISprite(*renderer, "Textures/BackButton.png", 100, 200, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quit = new GUISprite(*renderer, "Textures/Exit.png", 100, 400, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", 50, 50);

	returnToMenu->SetActivated();
	quit->SetActivated();

	guiManager = new GUIManager(renderer, 100);
	creditsPos = { windowWidth / 2 - 500, 1080 };
	std::string text = GetCreditsString();

	GUIFont* title_credits = new GUIFont(*renderer, text, creditsPos.x, creditsPos.y);//gameName, credits,course, school, //change text
	//GUIFont* title_gameIdea = new GUIFont(*renderer, text, creditsPos.x, creditsPos.y+100);//gameIdea, group//change text
	//	//content all members
	//GUIFont* title_leadProg = new GUIFont(*renderer, text, creditsPos.x, creditsPos.y + 200);//change text
	//	//content lead
	//GUIFont* title_system = new GUIFont(*renderer, text, creditsPos.x, creditsPos.y + 300);//change text
	//	//content system
	//GUIFont* title_GraphicsProgamming =
	//	title
	//	//content etc.

	title_credits->SetFontSize(dx::XMFLOAT2(0.7f, 0.7f));
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(returnToMenu, "returnToMenu");
	guiManager->AddGUIObject(quit, "quit");
	guiManager->AddGUIObject(title_credits, "credits");
}

void CreditsScene::OnActivate()
{
	input.SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	renderer->AddRenderPass(guiManager);
}

void CreditsScene::OnDeactivate()
{
	AudioMaster::Instance().StopSoundEvent("menusound");
	renderer->RemoveRenderPass(guiManager);
}

void CreditsScene::Update(const float& deltaTime)
{
	if (!Input::Instance().GetIsVisible())
		ShowCursor(true);

	Scene::Update(deltaTime);
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString("fps " + std::to_string((int)GameClock::Instance().GetFramesPerSecond()));

	static_cast<GUIFont*>(guiManager->GetGUIObject("credits"))->Translate(0, -deltaTime * SCROLLING_SPEED);

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("quit"))->IsClicked())
	{
		AudioMaster::Instance().StopSoundEvent("menusound");
		Engine::Instance->Exit();
		return;
	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("returnToMenu"))->IsClicked())
	{
		static_cast<GUIFont*>(guiManager->GetGUIObject("credits"))->SetPosition(creditsPos.x, creditsPos.y);
		AudioMaster::Instance().PlaySoundEvent("menusound");
		Engine::Instance->SwitchScene(SceneIndex::INTRO);
		return;
	}

	guiManager->UpdateAll();
}

void CreditsScene::Render()
{
	root->Draw(renderer, camera);
	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

