#pragma once

struct tKeyInfo
{
	int VK = -1;
	KEY_STATE		eState = KEY_STATE::NONE;	
	bool			bPrevPush = false;
	tKeyInfo(const int _VKval):VK{_VKval},eState{KEY_STATE::NONE},bPrevPush{false}{}
	operator bool()const { return -1 != VK; }
};

class KeyMgr
	:public Singleton<KeyMgr>
{
	friend class Singleton;
	KeyMgr();
	~KeyMgr();
private:
	unordered_map<int, tKeyInfo> m_mapKey;
	GLFWwindow* m_pWinInfo = nullptr;
	glm::vec2 m_vMousePos=ZERO_VEC3;
	glm::vec2 m_vPrevMousePos=ZERO_VEC3;
public:
	void Init();
	void Update()noexcept;
public:
	glm::vec2 GetMousePos()const noexcept{ return m_vMousePos; }
	glm::vec2 GetMouseDelta()const noexcept{ return m_vMousePos - m_vPrevMousePos; }
	const tKeyInfo& GetKeyInfo(const int _VKval) noexcept{ return m_mapKey.try_emplace(_VKval, tKeyInfo{ _VKval }).first->second; }
	const KEY_STATE GetKeyState(const int _VKval) noexcept{ return GetKeyInfo(_VKval).eState; }
	void SetMouseMode(const int _mode)noexcept;
};

