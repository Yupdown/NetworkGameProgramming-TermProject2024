#include "pch.h"
#include "PannelUI.h"
#include "Button.h"
#include "KeyMgr.h"
#include "Transform.h"
#include "TimeMgr.h"

void PannelUI::ResetUIState()
{
	std::ranges::for_each(*GetRootUI(), std::mem_fn(&UI::ResetUIState));
}

PannelUI::PannelUI(const glm::vec2 midPos, string_view strTexName, const float scaleFactor, glm::vec2 startUV, glm::vec2 endUV)
	:UI{midPos,strTexName,scaleFactor,startUV,endUV}
{
}

PannelUI::PannelUI()
{
}

PannelUI::~PannelUI()
{
}

void PannelUI::SetZDepth()
{
	std::ranges::for_each(m_vecButton,std::mem_fn(&UI::SetZDepthUI));
	std::ranges::for_each(*this, std::mem_fn(&UI::SetZDepthUI));
}

void PannelUI::DragMoveUI()
{
	if (!KEY_HOLD(GLFW_MOUSE_BUTTON_LEFT) || !m_bIsActivate)
	{
		return;
	}

	glm::vec3 dir = glm::vec3{ Mgr(KeyMgr)->GetMouseDelta(),0.f };
	const auto pTrans = GetTransform();
	const glm::vec3 curPos = pTrans->GetLocalPosition();
	
	const glm::vec2 diff = glm::vec2{ dir } *70.f * DT;

	for (auto& childs : *GetRootUI())
	{
		for (auto& b : childs->m_vecButton)
		{
			for (auto& p : b->m_arrLTRB)p += diff;
		}
		for (auto& p : childs->m_arrLTRB) p += diff;
	}
	dir.y = -dir.y;
	pTrans->SetLocalPosition(curPos + dir * 70.f * DT);
}

void PannelUI::SetUIScaleAll(const float _fRatio)
{
	for (auto& childs : *GetRootUI())
	{
		const auto pTrans = childs->GetTransform();
		const glm::vec3 curScale = pTrans->GetLocalScale();
		pTrans->SetLocalScale(curScale * _fRatio);
		for (auto& b : childs->m_vecButton)
		{
			b->SetUIScale(_fRatio);
		}
		childs->SetUIScale(_fRatio);
	}
}

void PannelUI::AddChild(shared_ptr<PannelUI> _pUI)
{
	GameObj::AddChild(_pUI);
	const glm::vec3 diff = GetTransform()->GetLocalPosition() - _pUI->GetTransform()->GetLocalPosition();
	for (auto& p : _pUI->m_arrLTRB)p -= glm::vec2{ diff };

	_pUI->GetTransform()->SetLocalPosition({ -m_uiSize.x + _pUI->m_uiMid.x ,-m_uiSize.y + _pUI->m_uiMid.y ,0.f});
	
	_pUI->m_fCurZDepth = m_fCurZDepth;
	_pUI->m_parentUI = static_pointer_cast<PannelUI>(shared_from_this());
	m_childUI.emplace_back(std::move(_pUI));
}

void PannelUI::AddButton(shared_ptr<Button> _pButton)
{
	GameObj::AddChild(_pButton);
	_pButton->m_fCurZDepth = m_fCurZDepth;
	m_vecButton.emplace_back(std::move(_pButton));
}

void PannelUI::ExecuteClickEvent()
{
	for (auto& child : m_childUI)
	{
		if (UI_STATE::CLICKED == child->GetCurUIState())
		{
			child->ExecuteClickEvent();
		}
	}
	if (UI_STATE::CLICKED == GetCurUIState())
	{
		ExecuteClickedEvent();
		for (auto& b : m_vecButton)
		{
			if (UI_STATE::CLICKED == b->GetCurUIState())
			{
				b->ExecuteClickedEvent();
				break;
			}
		}
		PannelUI::ResetUIState();
	}
}

void PannelUI::SetUIPosition(const glm::vec2& WCpos_)
{
	m_originMid = WCpos_;
	const auto pTrans = GetTransform();

	for (auto& childs : *GetRootUI())
	{
		for (auto& b : childs->m_vecButton)
		{
			const glm::vec2 diff = b->m_arrLTRB[RB] - b->m_arrLTRB[LT];
			b->m_arrLTRB[LT] = WCpos_ - diff / 2.f;
			b->m_arrLTRB[RB] = WCpos_ + diff / 2.f;
		}
		const glm::vec2 diff = childs->m_arrLTRB[RB] - childs->m_arrLTRB[LT];
		childs->m_arrLTRB[LT] = WCpos_ - diff / 2.f;
		childs->m_arrLTRB[RB] = WCpos_ + diff / 2.f;
	}
	// TODO 
	// UI와 파티클 추후 분리필요. 좌표계를 확인하라
	pTrans->SetLocalPosition(ScreenToOpenGL2D(WCpos_));
}
