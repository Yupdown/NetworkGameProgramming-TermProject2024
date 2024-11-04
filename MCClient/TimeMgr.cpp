#include "pch.h"
#include "TimeMgr.h"
#include "Core.h"
#include "KeyMgr.h"
#include "UI.h"

TimeMgr::TimeMgr()
{
}

TimeMgr::~TimeMgr()
{
}

void TimeMgr::Init()
{
	m_DeltaTime = std::chrono::duration<float>(0.f);
	m_InitTime = m_PrevTime = std::chrono::steady_clock::now();
}

void TimeMgr::Update()noexcept
{
	auto CurTime = std::chrono::steady_clock::now();
	m_DeltaTime = CurTime - m_PrevTime;
	m_PrevTime = std::move(CurTime);
	for (const auto& [str, fp] : m_mapTimer)
	{
		fp();
	}
	m_accFPS += m_DeltaTime.count();
	if (0.5f <= m_accFPS)
	{
		const auto vMousePos = Mgr(KeyMgr)->GetMousePos();
		const auto glCoord = ScreenToOpenGL2D(vMousePos);
		m_accFPS = 0.;
		glfwSetWindowTitle(Mgr(Core)->GetWinInfo(), std::format("FPS: {} , MOUSE: ({},{})", 1.f / m_DeltaTime.count(), glCoord.x, glCoord.y).data());
	}
}
