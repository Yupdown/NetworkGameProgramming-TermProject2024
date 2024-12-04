#include "pch.h"
#include "PannelProgressUI.h"
#include "Core.h"
#include "UI.h"
#include "Camera.h"
#include "KeyMgr.h"
#include "Transform.h"
#include "PathMgr.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "ResMgr.h"

PannelProgressUI::PannelProgressUI(const glm::vec2 midPos, string_view strTexName, const float scaleFactor, glm::vec2 startUV, glm::vec2 endUV)
	:PannelUI{ midPos,strTexName,scaleFactor,startUV,endUV }
{
}

PannelProgressUI::~PannelProgressUI()
{
}

void PannelProgressUI::Render()
{
	for (const auto& childUI : m_vecChildObj) {
		childUI->Render();
	}

	Mgr(ResMgr)->GetRes<Shader>("UIProgressShader.glsl")->Use();
	Mgr(ResMgr)->GetRes<Shader>("UIProgressShader.glsl")->SetUniformMat4(GetObjectWorldTransform(), "uModel");
	Mgr(ResMgr)->GetRes<Shader>("UIProgressShader.glsl")->SetFloat(m_fProgress, "uProgress");

	m_uiTex[etoi(UI_STATE::NONE)]->BindTexture();
	m_uiMesh[etoi(UI_STATE::NONE)]->Render();
	m_uiTex[etoi(UI_STATE::NONE)]->UnBindTexture();
}
