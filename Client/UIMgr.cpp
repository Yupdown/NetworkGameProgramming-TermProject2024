#include "pch.h"
#include "UIMgr.h"
#include "UI.h"
#include "PannelUI.h"
#include "KeyMgr.h"
#include "Transform.h"
#include "TimeMgr.h"
#include "ThreadMgr.h"
#include "Core.h"
#include "Camera.h"
#include "ResMgr.h"
#include "Shader.h"
#include "PannelUI.h"
#include "EventMgr.h"
#include "SoundMgr.h"

const std::function<bool(const UI*, const UI*)> UIMgr::cmpZDepth = [](const UI* a, const UI* b){ return *a > *b; };

extern bool g_bCanResume;
extern std::function<void(void)> g_fpNewGameFunc;	// 뉴게임 눌렀을 때 할 일
UIMgr::UIMgr()
{
}

UIMgr::~UIMgr()
{
}

void UIMgr::Init()
{
	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	const float fScaleFactor = Mgr(Core)->GetScaleFactor();
	const float w = width / fScaleFactor;
	const float h = height / fScaleFactor;

	{
		const int gridCount = 16; 
		const float gridSizeW = w / gridCount; 

		for (int y = 0; y < gridCount; ++y)
		{
			for (int x = 0; x < gridCount; ++x)
			{
				const glm::vec2 pos(gridSizeW * (x + 0.5f), gridSizeW * (y + 0.5f));
				auto background = make_shared<PannelUI>(pos, "introBackGround.png", 6.0f);
				m_vecUI[etoi(SCENE_TYPE::INTRO)].emplace_back(background);
			}
		}

		auto back_logo = make_shared<PannelUI>(glm::vec2{ w/2.f,(h/2.f) - 160.f}, "title.png", 1.f);
		m_vecUI[etoi(SCENE_TYPE::INTRO)].emplace_back(back_logo);

		auto gui1 = make_shared<PannelUI>(glm::vec2{ w / 2.f,(h / 2.f) + 50.f }, "introGUI_NewGame.png", 3.f, glm::vec2{ 0,1 / 3.f }, glm::vec2{ 1,2 / 3.f });
		m_vecUI[etoi(SCENE_TYPE::INTRO)].emplace_back(gui1);

		gui1->AddClickedEvent([]() {
			if (!g_bCanResume)
			{
				Mgr(SoundMgr)->PlayBGM("calm3.ogg");
				ChangeScene(SCENE_TYPE::STAGE, true);
			}
			else 
			{
				ChangeScene(SCENE_TYPE::STAGE, false);
				if (g_fpNewGameFunc)
				{
					g_fpNewGameFunc();
				}
			}
			Mgr(SoundMgr)->PlayEffect("click.ogg", 0.25f);
			
			});
		
		auto gui2 = make_shared<PannelUI>(glm::vec2{ w / 2.f,(h / 2.f) + 50.f }, "introGUI_NewGame.png", 3.f, glm::vec2{ 0,0 }, glm::vec2{ 1,1 / 3.f });
		
		gui1->AddUIStateTex(UI_STATE::ON_MOUSE, gui2);
		
		{
			auto gui3 = make_shared<PannelUI>(glm::vec2{ w / 2.f,(h / 2.f) + 150.f }, "introGUI_Resume.png", 3.f, glm::vec2{ 0,1 / 3.f }, glm::vec2{ 1,2 / 3.f });
			m_vecUI[etoi(SCENE_TYPE::INTRO)].emplace_back(gui3);

			gui3->AddClickedEvent([]() {
				if (g_bCanResume)
				{
					glfwSetInputMode(Mgr(Core)->GetWinInfo(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					Mgr(SoundMgr)->PlayEffect("click.ogg", 0.25f);
					ChangeScene(SCENE_TYPE::STAGE, false);
				}
				});

			auto gui4 = make_shared<PannelUI>(glm::vec2{ w / 2.f,(h / 2.f) + 150.f }, "introGUI_Resume.png", 3.f, glm::vec2{ 0,0 }, glm::vec2{ 1,1 / 3.f });

			gui3->AddUIStateTex(UI_STATE::ON_MOUSE, gui4);
		}

		auto gui3 = make_shared<PannelUI>(glm::vec2{ w / 2.f,(h / 2.f) + 250.f }, "introGUI_Quit.png", 3.f, glm::vec2{ 0,1 / 3.f }, glm::vec2{ 1,2 / 3.f });
		m_vecUI[etoi(SCENE_TYPE::INTRO)].emplace_back(gui3);

		gui3->AddClickedEvent([]() {
			Mgr(SoundMgr)->PlayEffect("click.ogg", 0.25f);
			UnLoadScene();
			});

		auto gui4 = make_shared<PannelUI>(glm::vec2{ w / 2.f,(h / 2.f) + 250.f }, "introGUI_Quit.png", 3.f, glm::vec2{ 0,0 }, glm::vec2{ 1,1 / 3.f });

		gui3->AddUIStateTex(UI_STATE::ON_MOUSE, gui4);
	}

	{
		auto quick_bar = make_shared<PannelUI>(glm::vec2{ w / 2.f,h - 35.f }, "gui.png", 3.f);
		m_vecUI[etoi(SCENE_TYPE::STAGE)].emplace_back(quick_bar);

		for (int i = 0; i < 9; ++i)
		{
			auto item_icon = make_shared<PannelUI>(glm::vec2(w * 0.5f + (i - 4) * 60.0f, h - 35.0f), std::format("tile_preview_{:02d}.png", i + 1), 0.15f);
			m_vecUI[etoi(SCENE_TYPE::STAGE)].emplace_back(item_icon);
		}

		m_pTargetUI = make_shared<PannelUI>(glm::vec2(), "gui_target.png", 3.0f);
		m_vecUI[etoi(SCENE_TYPE::STAGE)].emplace_back(m_pTargetUI);

		auto cross_line = make_shared<PannelUI>(glm::vec2{ w,h } / 2.f, "cross.png", 2.f);
		m_vecUI[etoi(SCENE_TYPE::STAGE)].emplace_back(cross_line);

		auto health_bar = make_shared<PannelUI>(glm::vec2(w * 0.5f - 148.0f, h - 85.0f), "gui_health.png", 3.0f, glm::zero<glm::vec2>(), glm::vec2(1.0f, 0.5f));
		m_vecUI[etoi(SCENE_TYPE::STAGE)].emplace_back(health_bar);
	}

	SetSelectIndex(0);

	//auto temp = make_shared<PannelUI>(glm::vec2{ 10,10 }, "cross.png", 1.f);
	//quick_bar->AddChild(temp);
}

void UIMgr::Update()
{
	InputUpdate();

	for (auto& ui : m_vecUI[etoi(m_eCurUIScene)])
	{
		std::ranges::for_each(*ui, std::mem_fn(&UI::Update));

		const UI_STATE eCurState = ui->GetCurUIState();

		if (UI_STATE::ON_CLICK == eCurState ||
			UI_STATE::CLICKED == eCurState)
		{
			m_setUI.emplace(ui.get());
		}
	}

	if (!m_setUI.empty())
	{
		const UI_STATE eCurState = (*m_setUI.begin())->GetCurUIState();

		if ((UI_STATE::ON_CLICK == eCurState && KEY_TAP(GLFW_MOUSE_BUTTON_LEFT)) ||
			UI_STATE::CLICKED == eCurState)
		{
			//(*m_setUI.begin())->SetZDepth();
		}

		if (UI_STATE::ON_CLICK == eCurState)
		{
			//(*m_setUI.begin())->DragMoveUI();
			(*m_setUI.begin())->ExecuteOnClickEvent();
		}
		else if (UI_STATE::CLICKED == eCurState)
		{
			(*m_setUI.begin())->ExecuteClickEvent();
		}
	}

	m_setUI.clear();

	for (auto& ui : m_vecUI[etoi(m_eCurUIScene)])
	{
		if (!ui->IsActivate())continue;
		ui->FinalUpdate();
		m_setUI.emplace(ui.get());
	}
	Mgr(ThreadMgr)->WaitAllJob();
}

void UIMgr::Start()
{
	for (auto& ui : m_vecUI | std::views::join)
	{
		try {
			if (!ui->GetParentGameObj().expired())
				throw std::runtime_error("Dont Add Child in UIMgr");
		}
		catch (const std::runtime_error& e) {
			std::cerr << e.what();
			exit(0);
		}
		ui->Awake();
		ui->Start();
		ui->InitGameObj();
	}
}

void UIMgr::Render()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static UBOData& sceneData = Mgr(Core)->GetUBOData();
	sceneData.viewMat = glm::mat4{ 1.f };

	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	sceneData.projMat = glm::orthoLH(0.0f, width, height, 0.0f, -1.0f, 1.0f);
	sceneData.viewMat = glm::mat4{ 1.f };
	
	Mgr(Core)->BindUBOData();
	Mgr(ResMgr)->GetRes<Shader>("UIShader.glsl")->Use();

	for (auto ui = m_setUI.rbegin(); ui != m_setUI.rend(); ++ui)
	{
		//ui->ClearComponentWaitFlag();
		(*ui)->Render();
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//insertionSort(m_vecUI, [](const shared_ptr<PannelUI>& a, const shared_ptr<PannelUI>& b) {return *a < *b; });
	
	/*for (auto& ui : m_vecUI)
	{
		ui->Render();
	}*/

	m_setUI.clear();
}

void UIMgr::SaveForPractice(string_view _strPracticeName)
{
	const auto practicePath = Mgr(PathMgr)->GetSavePath() / _strPracticeName;
	fs::create_directory(practicePath);

	int cnt = 0;

	for (const auto& ui : m_vecUI)
	{
		//ui->GameObj::Save(std::to_string(cnt++), practicePath / "UI");
	}
}

void UIMgr::LoadForPractice(string_view _strPracticeName)
{
	const auto practicePath = Mgr(PathMgr)->GetSavePath() / _strPracticeName / "UI";

	fs::directory_iterator practiceIter{ practicePath };

	int cnt = 0;

	for (const auto& pracIter : practiceIter)
	{
		auto pObj = make_shared<PannelUI>();
		pObj->GameObj::Load(std::to_string(cnt++), practicePath);
		//m_vecUI.emplace_back(std::move(pObj));
	}
}

void UIMgr::InputUpdate()
{
	if (KEY_TAP(GLFW_KEY_1))
		SetSelectIndex(0);
	if (KEY_TAP(GLFW_KEY_2))
		SetSelectIndex(1);
	if (KEY_TAP(GLFW_KEY_3))
		SetSelectIndex(2);
	if (KEY_TAP(GLFW_KEY_4))
		SetSelectIndex(3);
	if (KEY_TAP(GLFW_KEY_5))
		SetSelectIndex(4);
	if (KEY_TAP(GLFW_KEY_6))
		SetSelectIndex(5);
	if (KEY_TAP(GLFW_KEY_7))
		SetSelectIndex(6);
	if (KEY_TAP(GLFW_KEY_8))
		SetSelectIndex(7);
	if (KEY_TAP(GLFW_KEY_9))
		SetSelectIndex(8);
}

void UIMgr::SetSelectIndex(int index)
{
	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	const float fScaleFactor = Mgr(Core)->GetScaleFactor();
	const float w = width / fScaleFactor;
	const float h = height / fScaleFactor;

	m_iSelectedIndex = index;
	m_pTargetUI->SetUIPosition(glm::vec2(w * 0.5f + (index - 4) * 60.0f, h - 35.0f));
}
