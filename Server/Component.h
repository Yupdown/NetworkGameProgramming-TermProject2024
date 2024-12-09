#pragma once

class Object;
class SendBuffer;

class Component
{
public:
	virtual ~Component()noexcept = default;
public:
	virtual void Update(const float DT) = 0;
public:
	void SetOwner(const auto p)noexcept { m_pOwner = p; }
	const auto GetOwner()const noexcept { return m_pOwner; }
private:
	Object* m_pOwner = nullptr;
};