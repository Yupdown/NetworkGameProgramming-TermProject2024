#pragma once

class Object;

class HP
{
public:
	void SetHP(const int hp_)noexcept { m_HP = hp_; }
	const auto GetHP()const noexcept { return m_HP; }
	void SetOwner(Object* const obj_)noexcept { m_pOwner = obj_; }
public:
	const auto GetOwner()const noexcept { return m_pOwner; }
	void IncHP(const int inc_)noexcept { m_HP += inc_; }
	void DecHP(const int dec_)noexcept { AfterDecHPAction(m_HP -= dec_); }
private:
	virtual void AfterDecHPAction(const int hp_)noexcept = 0;
protected:
	static void SendHitPacket(const Object* const owner, const int hp_)noexcept;
	const bool IsDead()const noexcept { return 0 >= m_HP; }
private:
	Object* m_pOwner = nullptr;
	int m_HP = 0;
};

class PlayerHP
	:public HP
{
	virtual void AfterDecHPAction(const int hp_)noexcept override;
};

class EnderManHP
	:public HP
{
	virtual void AfterDecHPAction(const int hp_)noexcept override;
};

class EnderDragonHP
	:public HP
{
	virtual void AfterDecHPAction(const int hp_)noexcept override;
};