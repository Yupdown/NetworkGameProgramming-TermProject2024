#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "imgui.h"
#include "imconfig.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ImGuiPannel;

class ImGuiMgr
	:public Singleton<ImGuiMgr>
{
	friend class Singleton;
	ImGuiMgr();
	~ImGuiMgr();
private:
	ImGuiContext* m_imguiContext = nullptr;
	vector<unique_ptr<ImGuiPannel>> m_vecImGui;
public:
	void Init();
	void Update();
	void Render();

	void AddImGui(ImGuiPannel* const _pImGui) { m_vecImGui.emplace_back(_pImGui); }
};

class ImGuiPannel
{
private:
	string m_strName;
	function<void(void)> m_function;
	GLint m_trigger;
	bool m_bOnAttach = false;
public:
	ImGuiPannel(string_view _strName,const GLint _iTrigger);
	~ImGuiPannel();
	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void SetUpdateFunction(Func&& fp, Args&&... args) { m_function = std::bind(fp, args...);}
	void SetActivate(const bool b_)noexcept { m_bOnAttach = b_; }
	void Update();
	void Render();
};
