#include "pch.h"
#include "EnderDragon.h"
#include "EnderDragonRenderer.h"
#include <UIMgr.h>

EnderDragon::EnderDragon()
{

}

void EnderDragon::Start()
{
	AddChild(std::make_shared<EnderDragonRenderer>());

	Mgr(UIMgr)->SetBossHealthActive(true);

	ServerObject::Start();
}

void EnderDragon::OnObjectDamaged(int value)
{
	Mgr(UIMgr)->SetBossHealth(value);

	if (value <= 0)
	{
		Mgr(UIMgr)->SetBossHealthActive(false);
	}
}
