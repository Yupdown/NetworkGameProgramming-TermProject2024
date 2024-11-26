#include "pch.h"
#include "EnderDragonRenderer.h"
#include "AssimpMgr.h"

#include <Transform.h>

EnderDragonRenderer::EnderDragonRenderer() : GameObj()
{
}

void EnderDragonRenderer::Start()
{
	m_rendererObject = Mgr(AssimpMgr)->LoadAllPartsAsGameObj("DefaultWarpShader.glsl", "ender_dragon.fbx");
	m_rendererObject->GetTransform()->SetLocalScale(1.0f / 16.0f);
	AddChild(m_rendererObject);
}