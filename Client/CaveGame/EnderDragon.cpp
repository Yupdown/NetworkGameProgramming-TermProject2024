#include "pch.h"
#include "EnderDragon.h"
#include "EnderDragonRenderer.h"

EnderDragon::EnderDragon()
{

}

void EnderDragon::Start()
{
	AddChild(std::make_shared<EnderDragonRenderer>());
	ServerObject::Start();
}
