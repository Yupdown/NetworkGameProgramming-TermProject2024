#pragma once
#include "Component.h"

class Projectile
	:public Component
{
public:
	static constexpr float G_PROJ_REMAIN_TIME = 3.f;
public:
	void Update(const float DT)override final;
private:
	virtual void UpdateProjectile(const float DT)noexcept = 0;
protected:
	static void CheckGroupCollision(Projectile* const proj, const MC_OBJECT_TYPE type, const float radius, const float height, const int dmg)noexcept;
protected:
	float m_accProjTime = 0.f;
	bool m_isPinned = false;
};

class ProjArrow
	:public Projectile
{
private:
	static constexpr int G_ARROW_DMG = 1;
private:
	void CheckCollisionToEnderDragon();
	virtual void UpdateProjectile(const float DT)noexcept override;
};

class ProjFireBall
	:public Projectile
{
private:
	static constexpr int G_FIRE_BALL_DMG = 3;
private:
	virtual void UpdateProjectile(const float DT)noexcept override;
};
