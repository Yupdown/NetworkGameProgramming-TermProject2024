#include "pch.h"
#include "Core.h"
#include "PathMgr.h"
#include "ResMgr.h"
#include "SceneMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Texture.h"
#include "Scene.h"
#include "ThreadMgr.h"
#include "EventMgr.h"
#include "ImGuiMgr.h"
#include "UIMgr.h"
#include "AssimpMgr.h"
#include "ParticleMgr.h"
#include "CollisionMgr.h"
#include "RayCaster.h"
#include "SoundMgr.h"
#include "InstancingMgr.h"

extern std::future<void> g_resetMemPool;

void LowFragmentationHeap() noexcept
{
	ULONG HeapFragValue = 2;
	HANDLE hHeaps[100];
	const DWORD dwHeapCount = GetProcessHeaps(100, hHeaps);
	for (DWORD i = 0; i < dwHeapCount; ++i)
	{
		HeapSetInformation(
			hHeaps[i],
			HeapCompatibilityInformation,
			&HeapFragValue,
			sizeof(HeapFragValue)
		);
	}
}

Core::Core()
{
	//m_vecDrawCall.reserve(1000);
}

Core::~Core()
{
	glDeleteBuffers(1, &m_ubo);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hglrc);
	ReleaseDC(m_hWnd, m_hDC);

	glfwDestroyWindow(m_pWinInfo);
	glfwTerminate();
}

void Core::SetUBO()
{
	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOData), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

std::tuple<int, int, int, int> Core::AdjustWinSize(GLuint width_, GLuint height_, HWND hwnd)const noexcept
{
	const HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
	GetMonitorInfo(hMonitor, &monitorInfo);

	const int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	const int monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

	const UINT dpi = GetDpiForWindow(hwnd);
	m_fScaleFactor = dpi / 96.0f;

	const int adjustedWidth = static_cast<int>(width_ * m_fScaleFactor);
	const int adjustedHeight = static_cast<int>(height_ * m_fScaleFactor);

	const int posX = (monitorWidth - adjustedWidth) / 2;
	const int posY = (monitorHeight - adjustedHeight) / 2;

	return std::make_tuple(adjustedWidth, adjustedHeight, posX, posY);
}

std::tuple<int, int, int, int> Core::AdjustWinSize(HWND hwnd) const noexcept
{
	const HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
	GetMonitorInfo(hMonitor, &monitorInfo);

	const int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	const int monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

	const UINT dpi = GetDpiForWindow(hwnd);
	m_fScaleFactor = dpi / 96.0f;

	int adjustedWidth = static_cast<int>(monitorWidth * 0.9f);
	int adjustedHeight = adjustedWidth / 2;


	//if (adjustedHeight > monitorHeight * 0.8f)
	{
		adjustedHeight = static_cast<int>(monitorHeight * 0.9f);
		//adjustedWidth = adjustedHeight * 2;
	}

	const int posX = (monitorWidth - adjustedWidth) / 2;
	const int posY = (monitorHeight - adjustedHeight) / 2;

	return std::make_tuple(adjustedWidth, adjustedHeight, posX, posY);
}

void Core::PrepareStart() const noexcept
{
	Mgr(SceneMgr)->Update();
	Mgr(SceneMgr)->PreFinalUpdate();
	Mgr(SceneMgr)->FinalUpdate();
	Mgr(SceneMgr)->Render();
}

void Core::Init(const GLuint _winWidth, const GLuint _winHeight)
{
	LowFragmentationHeap();

	glfwInit();
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);
	
	m_pWinInfo = glfwCreateWindow(1, 1, "MyOpenGL", nullptr, nullptr);

	m_hWnd = glfwGetWin32Window(m_pWinInfo);
	m_hDC = GetDC(m_hWnd);
	m_hglrc = wglCreateContext(m_hDC);

	int posX, posY;

	std::tie(m_winWidth, m_winHeight, posX, posY) = 0 == _winWidth || 0 == _winHeight ? AdjustWinSize(m_hWnd) : AdjustWinSize(_winWidth, _winHeight, m_hWnd);

	glfwSetWindowPos(m_pWinInfo, posX, posY);
	glfwSetWindowSize(m_pWinInfo, m_winWidth, m_winHeight);

	wglMakeCurrent(m_hDC, m_hglrc);

	m_fScaleFactorW = m_fScaleFactorH = m_fScaleFactor;

	glewInit();

	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "OpenGL Version: " << version << std::endl;

	glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glDepthRange(0.0F, 1.0F);
	glClearDepthf(1.0F);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glfwSetWindowRefreshCallback(m_pWinInfo, [](GLFWwindow* _winInfo) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mgr(SceneMgr)->PreRender();
		Mgr(InstancingMgr)->Render();
		Mgr(SceneMgr)->Render();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		Mgr(ImGuiMgr)->Render();

		glfwSwapBuffers(_winInfo);
		});

	glfwSetFramebufferSizeCallback(m_pWinInfo, [](GLFWwindow* _window, int width, int height) {
		static const auto [prevW, prevH] = Mgr(Core)->GetWidthHeight();
		static const auto factor = Mgr(Core)->GetScaleFactor();
		width = 0 == width ? 1 : width;
		height = 0 == height ? 1 : height;
		Mgr(Core)->SetWinWidth(width);
		Mgr(Core)->SetWinHeight(height);
		glViewport(0, 0, width, height);

		Mgr(Core)->SetScaleFactorW((float)width /((float)prevW/factor));
		Mgr(Core)->SetScaleFactorH((float)height / ((float)prevH/factor));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mgr(SceneMgr)->Update();
		Mgr(SceneMgr)->PreFinalUpdate();
		Mgr(SceneMgr)->FinalUpdate();

		Mgr(SceneMgr)->PreRender();
		Mgr(InstancingMgr)->Render();
		Mgr(SceneMgr)->Render();
		

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		Mgr(ImGuiMgr)->Update();
		Mgr(ImGuiMgr)->Render();

		glfwSwapBuffers(_window);
		});

	glfwSetKeyCallback(m_pWinInfo, [](GLFWwindow* _window, int key, int scancode, int action, int mods) {
		ImGui_ImplGlfw_KeyCallback(_window, key, scancode, action, mods);

		});

	glfwSetCharCallback(m_pWinInfo, [](GLFWwindow* _window, unsigned int ch) {
		ImGui_ImplGlfw_CharCallback(_window, ch);
		});

	glfwSetCursorPosCallback(m_pWinInfo, [](GLFWwindow* _window, double x, double y) {
		ImGui_ImplGlfw_CursorPosCallback(_window, x, y);
		});

	glfwSetMouseButtonCallback(m_pWinInfo, [](GLFWwindow* _window, int button, int action, int modifier) {
		ImGui_ImplGlfw_MouseButtonCallback(_window, button, action, modifier);
		});

	glfwSetScrollCallback(m_pWinInfo, [](GLFWwindow* _window, double xoffset, double yoffset) {
		ImGui_ImplGlfw_ScrollCallback(_window, xoffset, yoffset);
		});

	glViewport(0, 0, m_winWidth, m_winHeight);
	//glfwSwapInterval(1);

	SetUBO();

	Mgr(ThreadMgr)->Init();
	Mgr(PathMgr)->Init();
	Mgr(ResMgr)->Init();
	Mgr(SceneMgr)->Init();
	Mgr(KeyMgr)->Init();
	Mgr(EventMgr)->Init();
	Mgr(ParticleMgr)->Init();
	Mgr(AssimpMgr)->Init();
	Mgr(CollisionMgr)->Init();
	Mgr(RayCaster)->Init();
	Mgr(SoundMgr)->Init();
	Mgr(InstancingMgr)->Init();

	std::atomic_thread_fence(std::memory_order_seq_cst);

	Mgr(ImGuiMgr)->Init();
	Mgr(UIMgr)->Init();


	Mgr(TimeMgr)->Init();
}

void Core::GameLoop()
{
	Mgr(SceneMgr)->GetCurScene()->EnterScene();
	
	Mgr(UIMgr)->Start();
	Mgr(ThreadMgr)->ExecuteMainThreadTask();
	
	PrepareStart();

	while (!glfwWindowShouldClose(m_pWinInfo))
	{
		glfwPollEvents();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		Mgr(KeyMgr)->Update();
		Mgr(TimeMgr)->Update();


		Mgr(SceneMgr)->Update();

		Mgr(SceneMgr)->PreFinalUpdate();

		Mgr(CollisionMgr)->CollisionUpdate();

		Mgr(SceneMgr)->FinalUpdate();

		Mgr(RayCaster)->Update();

		Mgr(InstancingMgr)->Update();

		Mgr(SceneMgr)->PreRender();

		Mgr(SceneMgr)->Render();
		Mgr(InstancingMgr)->Render();

		if (g_resetMemPool.valid())
		{
			g_resetMemPool.get();
		}

		Mgr(CollisionMgr)->RenderCollisionBox();

		//DrawCall();

		Mgr(UIMgr)->Update();
		Mgr(UIMgr)->Render();

		//DrawCall();
		Mgr(EventMgr)->Update();
		Mgr(SoundMgr)->Update();

		Mgr(ImGuiMgr)->Update();
		Mgr(ImGuiMgr)->Render();

		glfwSwapBuffers(m_pWinInfo);
	}
}

void Core::Quit() const noexcept
{
	glfwSetWindowShouldClose(m_pWinInfo, GL_TRUE);
}
