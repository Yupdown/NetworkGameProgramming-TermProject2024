#include "pch.h"
#include "NumTextUI.h"
#include "Core.h"
#include "UI.h"
#include "Camera.h"
#include "KeyMgr.h"
#include "Transform.h"
#include "PathMgr.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "ResMgr.h"

NumTextUI::NumTextUI(const glm::vec2 midPos, string_view strTexName, const float scaleFactor, glm::vec2 startUV, glm::vec2 endUV)
	:PannelUI{ midPos,strTexName,scaleFactor,startUV,endUV }
{
}

void NumTextUI::Render()
{
	for (const auto& childUI : m_vecChildObj) {
		childUI->Render();
	}

	int digit1 = m_num % 10;
	int digit10 = m_num / 10;

	shared_ptr<Shader> pShader = Mgr(ResMgr)->GetRes<Shader>("UIDigitShader.glsl");

	pShader->Use();
	m_uiTex[etoi(UI_STATE::NONE)]->BindTexture();

	glm::mat4 model = GetObjectWorldTransform();

	pShader->SetUniformMat4(glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f)), "uModel");
	pShader->SetFloat(static_cast<float>(digit1), "uDigit");
	m_uiMesh[etoi(UI_STATE::NONE)]->Render();

	if (digit10 != 0)
	{
		pShader->SetUniformMat4(glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f)), "uModel");
		pShader->SetFloat(static_cast<float>(digit10), "uDigit");
		m_uiMesh[etoi(UI_STATE::NONE)]->Render();
	}

	m_uiTex[etoi(UI_STATE::NONE)]->UnBindTexture();
}