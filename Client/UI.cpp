#include "pch.h"
#include "Core.h"
#include "UI.h"
#include "Camera.h"
#include "KeyMgr.h"
#include "Transform.h"
#include "PathMgr.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "ResMgr.h"

float UI::g_curMaxZDepth = 0.f;

bool UI::ptInRect(const glm::vec2& point, const glm::vec2& leftTop, const glm::vec2& rightBottom) {
	return point.x >= leftTop.x && point.x <= rightBottom.x &&
		point.y >= leftTop.y && point.y <= rightBottom.y;
}

glm::vec3 UI::wc2GL(const glm::vec2& point) {
	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	return glm::vec3{
		(point.x / width) * 2.0f - 1.0f, 
		1.0f - (point.y / height) * 2.0f,
		0.f
	};
}

glm::vec2 UI::gl2WC(const glm::vec3& point) {
	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	return glm::vec2{ point.x + width / 2.f, height / 2.f - point.y };
}

//vector<SimpleVertex> UI::makeRect(const glm::vec2& LT, const glm::vec2& RB, glm::vec3& glCenter) {
//	vector<SimpleVertex> temp(4);
//
//	temp[0] = wc2GL(LT);           
//	temp[1] = wc2GL(glm::vec2(RB.x, LT.y)); 
//	temp[2] = wc2GL(RB);        
//	temp[3] = wc2GL(glm::vec2(LT.x, RB.y));
//	for (const auto& v : temp) {
//		glCenter += v.pos;
//	}
//	glCenter /= static_cast<float>(temp.size());
//
//	for (auto& v : temp) 
//	{
//		v.pos -= glCenter;
//	}
//
//	return temp;
//}

UI_STATE UI::UpdateCurUIState()
{
	const auto MousePos = Mgr(KeyMgr)->GetMousePos();
	const bool bIsOnMouse = ptInRect(MousePos, m_arrLTRB[LT], m_arrLTRB[RB]);
	const KEY_STATE MouseStateL = Mgr(KeyMgr)->GetKeyState(GLFW_MOUSE_BUTTON_LEFT);
	const KEY_STATE MouseStateM = Mgr(KeyMgr)->GetKeyState(GLFW_MOUSE_BUTTON_MIDDLE);
	// const KEY_STATE MouseStateR = Mgr(KeyMgr)->GetKeyState(GLFW_MOUSE_BUTTON_RIGHT);

	if (bIsOnMouse)
	{
		if (MouseStateL == KEY_STATE::TAP 
			|| MouseStateM == KEY_STATE::TAP )
			//|| MouseStateR == KEY_STATE::TAP)
		{
			return UI_STATE::ON_CLICK;
		}
		else if (MouseStateL == KEY_STATE::AWAY
			|| MouseStateM == KEY_STATE::AWAY)
			//|| MouseStateR == KEY_STATE::AWAY)
		{
			return UI_STATE::CLICKED;
		}
		else
		{
			return UI_STATE::ON_MOUSE;
		}
	}
	else
	{
		return UI_STATE::NONE;
	}
}

UI::UI()
{
}

void UI::Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath)
{
	std::ofstream outFile((_savePath / _resName / _resName).string() + "_UI.json");
	nlohmann::json uiJson;
	uiJson["LTRB"] = nlohmann::json::array();

	for (auto& p : m_arrLTRB)
	{
		uiJson["LTRB"].push_back({ p.x,p.y });
	}

	outFile << uiJson.dump(4);
	
	MyPolygon::Save(_resName, writer,_savePath);
}

void UI::Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath)
{
	std::ifstream inFile((_loadPath / _dirName / _dirName).string() + "_UI.json");

	nlohmann::json uiJson;

	inFile >> uiJson;

	int cnt = 0;

	for (const auto& p : uiJson["LTRB"])
	{
		glm::vec2 v;
		v.x = p[0];
		v.y = p[1];
		m_arrLTRB[cnt++] = v;
	}
	glm::vec3 glCenter{};

	//makeRect(m_arrLTRB[LT], m_arrLTRB[RB], glCenter);
	GetTransform()->SetLocalPosition(glCenter);
	
	UI::g_curMaxZDepth += 0.01f;
	m_fCurZDepth = UI::g_curMaxZDepth;

	MyPolygon::Load(_dirName, doc,_loadPath);
}

void UI::SetUIScale(const float _fRatio)
{
	const glm::vec2 center = (m_arrLTRB[LT] + m_arrLTRB[RB]) / 2.f;
	const float w = ((m_arrLTRB[RB].x - m_arrLTRB[LT].x) / 2.f) * _fRatio;
	const float h = ((m_arrLTRB[RB].y - m_arrLTRB[LT].y) / 2.f) * _fRatio;
	m_arrLTRB[LT].x = center.x - w;
	m_arrLTRB[LT].y = center.y - h;
	m_arrLTRB[RB].x = center.x + w;
	m_arrLTRB[RB].y = center.y + h;
}

void UI::SetZDepthUI()
{
	g_curMaxZDepth += 0.01f;
	m_fCurZDepth = g_curMaxZDepth;
}

UI::UI(const glm::vec2 midPos, string_view strTexName, const float scaleFactor ,glm::vec2 startUV, glm::vec2 endUV)
	: m_fCurZDepth {UI::g_curMaxZDepth += 0.01f}
	, m_originMid{midPos}
{
	for (auto& tex : m_uiTex)
	{
		tex = Mgr(ResMgr)->GetRes<Texture2D>(strTexName);
	}
	for (auto& mesh : m_uiMesh)
	{
		mesh = make_shared<Mesh>();
	}

	const auto [w, h] = m_uiTex[0]->GetTexWH();

	const float actualWidth = (endUV.x - startUV.x) * w;
	const float actualHeight = (endUV.y - startUV.y) * h;
	const float fScaleFactor = Mgr(Core)->GetScaleFactor();

	const glm::vec2 half_size{ (actualWidth * fScaleFactor) / 2.f, (actualHeight * fScaleFactor) / 2.f };

	m_originLTRB[LT] = m_originMid - half_size * scaleFactor/fScaleFactor;
	m_originLTRB[RB] = m_originMid + half_size * scaleFactor/fScaleFactor;

	m_uiSize = half_size;
	m_uiMid = m_originMid * fScaleFactor;

	m_arrLTRB[LT] = m_uiMid - half_size * scaleFactor;
	m_arrLTRB[RB] = m_uiMid + half_size * scaleFactor;

	GetTransform()->SetLocalPosition(glm::vec3{ m_originMid * fScaleFactor, 0.f });
	m_fOriginScale = scaleFactor;
	GetTransform()->SetLocalScale(scaleFactor / fScaleFactor);

	const vector<glm::vec3> temp_vertex =
	{
		glm::vec3{-half_size.x,-half_size.y,0.f},
		glm::vec3{ half_size.x,-half_size.y,0.f},
		glm::vec3{ half_size.x, half_size.y,0.f},
		glm::vec3{-half_size.x, half_size.y,0.f},
	};

	static const vector<GLuint> temp_index = { 0, 1, 2, 0, 2, 3 };

	vector<glm::vec2> temp_uv =
	{
		glm::vec2(startUV.x, endUV.y),
		glm::vec2(endUV.x, endUV.y),
		glm::vec2(endUV.x, startUV.y),
		glm::vec2(startUV.x, startUV.y)
	};

	vector<Vertex> vert;
	vert.reserve(4);

	for (int i = 0; i < 4; ++i)
	{
		Vertex v;
		v.position = temp_vertex[i];
		v.uv = temp_uv[i];
		vert.emplace_back(v);
	}

	for (auto& mesh : m_uiMesh)
	{
		auto temp1 = vert;
		auto temp2 = temp_index;
		mesh->Init(temp1, temp2);
	}
}

UI::~UI()
{
}

void UI::SetUITex(string_view sTex)
{
	auto pTex = Mgr(ResMgr)->GetRes<Texture2D>(sTex);
	for (auto& tex : m_uiTex)
		tex = pTex;
}

void UI::Update()
{
	if (m_pParentGameObj.expired())
	{
		const auto pTrans = GetTransform();
		const auto [w, h] = Mgr(Core)->GetScaleFactorWH();
		const auto newLocal = glm::vec3{ m_originMid.x * w,m_originMid.y * h ,0.f };
		const float factor = Mgr(Core)->GetScaleFactor();
		pTrans->SetLocalPosition(newLocal);
		pTrans->SetLocalScale(m_fOriginScale * glm::vec3{ w, h, 0.f } /factor);
		m_arrLTRB[LT].x = (m_originLTRB[LT].x) * w;
		m_arrLTRB[LT].y = (m_originLTRB[LT].y) * h;
		m_arrLTRB[RB].x = (m_originLTRB[RB].x) * w;
		m_arrLTRB[RB].y = (m_originLTRB[RB].y) * h;
	}

	m_eCurUIState = UpdateCurUIState();
	m_uiCallback();

	for (const auto& childUI : m_vecChildObj)
	{
		childUI->Update();
	}

}

void UI::Render()
{
	for (const auto& childUI : m_vecChildObj)
	{
		childUI->Render();
	}

	Mgr(ResMgr)->GetRes<Shader>("UIShader.glsl")->Use();
	Mgr(ResMgr)->GetRes<Shader>("UIShader.glsl")->SetUniformMat4(GetObjectWorldTransform(), "uModel");

	m_uiTex[etoi(m_eCurUIState)]->BindTexture();
	m_uiMesh[etoi(m_eCurUIState)]->Render();
	m_uiTex[etoi(m_eCurUIState)]->UnBindTexture();
}
