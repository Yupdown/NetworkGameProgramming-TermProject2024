#include "pch.h"
#include "PlayerCam.h"
#include "Camera.h"
#include "Transform.h"

PlayerCam::PlayerCam()
{
	auto pCam = make_shared<Camera>();
	pCam->SetMainCam();
	
	m_arrComp[etoi(COMPONENT_TYPE::CAMERA)] = pCam;
	GetTransform()->SetLocalPosition(glm::vec3{ 0,30.f,-100.f });
}

PlayerCam::~PlayerCam()
{

}

void PlayerCam::ReverseCam() noexcept
{
	const auto pTrans = GetTransform();
	auto curLocal = pTrans->GetLocalPosition();
	curLocal.z = -curLocal.z;
	pTrans->SetLocalPosition(curLocal);
	pTrans->AddLocalRotation(0, 180.f, 0);
}
