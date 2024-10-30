#include "pch.h"
#include "Core.h"
#include "KeyMgr.h"
#include "Camera.h"
#include "ImGuiMgr.h"
#include "GameObj.h"
#include "CamMoveScript.h"
#include "Transform.h"
#include "RayCaster.h"
#include "Collider.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "ResMgr.h"

static char directoryBuffer[256]{};
static char objNameBuffer[256]{};

ImGuiMgr::ImGuiMgr()
{
	
}

ImGuiMgr::~ImGuiMgr()
{
	ImGui_ImplOpenGL3_DestroyFontsTexture();
	ImGui_ImplOpenGL3_DestroyDeviceObjects();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(m_imguiContext);
}

void ImGuiMgr::Init()
{
	m_imguiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_imguiContext);
	ImGui_ImplGlfw_InitForOpenGL(Mgr(Core)->GetWinInfo(), false);
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplOpenGL3_CreateFontsTexture();
	ImGui_ImplOpenGL3_CreateDeviceObjects();

	m_vecImGui.reserve(100);
	
	
	auto pCamGui = new ImGuiPannel{ "CamGUI",GLFW_KEY_SLASH };
	pCamGui->SetUpdateFunction([]() {
		ImGui::SetWindowFontScale(2.f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {100,70});
		static const auto pCam = Camera::GetCurCam();
		static const auto pTrans = pCam->GetTransform();
		//static const auto pCamMove = static_pointer_cast<CamMoveScript>(pCam->GetGameObj()->GetMonoBehavior("CamMoveScript"));
		static const auto pCamMove =pCam->GetGameObj()->GetMonoBehavior<CamMoveScript>();
		bool bNowFixed = pCamMove->GetIsFixed();

		if (ImGui::Button("CamFix")) {
			pCamMove->SetFixed(true);
			pCamMove->SetLookSlerp(ZERO_VEC3);
		}
		ImGui::SameLine();
		if (ImGui::Button("Quit")) {
			Mgr(Core)->Quit();
		}
		ImGui::Separator();
		glm::vec4 clearColor = Mgr(Core)->GetClearColor();
		if (ImGui::ColorEdit4("ClearColorEdit",glm::value_ptr(clearColor) )) {
			Mgr(Core)->SetClearColor(clearColor);
		}
	
		glm::vec3 camPos = pTrans->GetLocalPosition();
		if (ImGui::InputFloat3("CamPosition", glm::value_ptr(camPos))) {
			pTrans->SetLocalPosition(camPos);
		}
		glm::vec3 axis = glm::degrees(glm::eulerAngles(pTrans->GetLocalRotation()));
		glm::vec3 LookPoint = pCamMove->GetLookPoint();
		if (ImGui::InputFloat3("Cam Pitch, Yaw , Roll", glm::value_ptr(axis))) {
			if (bNowFixed)
			{
				axis = glm::radians(axis);
				const glm::quat newQuat = glm::normalize(glm::quat{ axis });
				pTrans->SetLocalRotation(newQuat);
				pCamMove->SetLookAtNow(pTrans->GetLocalPosition() + GetLookByQuat(newQuat));
			}
		}
		if (ImGui::InputFloat3("Set Cam Look Point", glm::value_ptr(LookPoint))) {
			if (bNowFixed)
			{
				pCamMove->SetLookSlerp(LookPoint);
			}
		}
		float fCamSpeed = pCamMove->GetMoveSpeed();
		if (ImGui::InputFloat("Set Cam Move Speed", &fCamSpeed)) {
			pCamMove->SetMoveSpeed(fCamSpeed);
		}
		ImGui::PopStyleVar();
		});
	AddImGui(pCamGui);

	auto pPickingGUI = new ImGuiPannel{ "PickingUI",GLFW_KEY_COMMA };
	pPickingGUI->SetUpdateFunction([]() {
		const auto pPickedObj = Mgr(RayCaster)->GetCurPickedObj();
		if (!pPickedObj)
		{
			return;
		}

		ImGui::SetWindowFontScale(2.f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 100,70 });
		
		const auto pTrans = pPickedObj->GetTransform();
		const auto pCollider = pPickedObj->GetComp<Collider>();
		const auto& OBB = pCollider->GetOBB();

		glm::vec3 objPos = pTrans->GetLocalPosition();
		if (ImGui::InputFloat3("ObjectPosition", glm::value_ptr(objPos))) {
			pTrans->SetLocalPosition(objPos);
		}

		glm::vec3 axis = glm::degrees(glm::eulerAngles(pTrans->GetLocalRotation()));
		if (ImGui::InputFloat3("Object Pitch, Yaw , Roll", glm::value_ptr(axis))) {
			axis = glm::radians(axis);
			const glm::quat newQuat = glm::normalize(glm::quat{ axis });
			pTrans->SetLocalRotation(newQuat);
		}

		glm::vec3 scale = pTrans->GetLocalScale();
		if (ImGui::InputFloat3("Object Scale", glm::value_ptr(scale))) {
			pTrans->SetLocalScale(scale);
		}

		glm::vec3 baseExtents = OBB.baseExtents;
		if (ImGui::InputFloat3("OBB Box Scale", glm::value_ptr(baseExtents))) {
			pCollider->SetColBoxScale(baseExtents);
			pTrans->AddLocalPosition({ 0.f,0.f,0.f });
		}

		glm::vec3 obbOffset = OBB.centerOffset;
		if (ImGui::InputFloat3("OBB Box Offset", glm::value_ptr(obbOffset))) {
			pCollider->SetColBoxOffset(obbOffset);
			pTrans->AddLocalPosition({ 0.f,0.f,0.f });
		}

		ImGui::InputText("DirectoryName", directoryBuffer, sizeof(directoryBuffer));
		ImGui::InputText(("ObjName : " + pPickedObj->GetResName()).data(), directoryBuffer, sizeof(directoryBuffer));

		if (ImGui::Button("SaveAll") && directoryBuffer[0]) {
			Mgr(SceneMgr)->GetCurScene()->SaveForPractice(directoryBuffer);
		}
		if (ImGui::Button("ChangeName") && objNameBuffer[0]) {
			pPickedObj->SetObjName(objNameBuffer);
		}
		if (ImGui::Button("Quit")) {
			Mgr(Core)->Quit();
		}
		ImGui::PopStyleVar();
		});

	AddImGui(pPickingGUI);
}

void ImGuiMgr::Update()
{
	for (const auto& guis : m_vecImGui)
	{
		guis->Update();
	}
}

void ImGuiMgr::Render()
{
	for (const auto& guis : m_vecImGui)
	{
		guis->Render();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImGuiPannel::ImGuiPannel(string_view _strName, const GLint _iTrigger)
	:m_strName{_strName}
	,m_trigger{_iTrigger}
{
}

ImGuiPannel::~ImGuiPannel()
{
}

void ImGuiPannel::Update()
{
	if (KEY_TAP(m_trigger))
	{
		m_bOnAttach = !m_bOnAttach;
	}
}

void ImGuiPannel::Render()
{
	if (!m_bOnAttach)
	{
		return;
	}
	if (ImGui::Begin(m_strName.data()))
	{
		m_function();
	}
	ImGui::End();
}
