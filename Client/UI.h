#pragma once
#include "MyPolygon.h"
#include "UIMgr.h"

enum class UI_STATE
{
	NONE,
	ON_MOUSE,
	ON_CLICK,
	CLICKED,

	END
};


class MyPolygon;
class Mesh;
class Texture2D;

class UI
	:public MyPolygon
{
public:
	static bool ptInRect(const glm::vec2& point, const glm::vec2& leftTop, const glm::vec2& rightBottom);
	static glm::vec3 wc2GL(const glm::vec2& point);
	static glm::vec2 gl2WC(const glm::vec3& point);
	//static vector<SimpleVertex> makeRect(const glm::vec2& LT, const glm::vec2& RB, glm::vec3& glCenter);
protected:
	static float g_curMaxZDepth;
	enum UI_RECT{LT,RB,END};

private:
	UI_STATE m_eCurUIState = UI_STATE::NONE;
	Delegate m_onClickEvent;
	Delegate m_ClickedEvent;
protected:
	array<glm::vec2, UI_RECT::END> m_arrLTRB;

	array<shared_ptr<Texture2D>,etoi(UI_STATE::END)> m_uiTex;
	array<shared_ptr<Mesh>, etoi(UI_STATE::END)> m_uiMesh;

	float m_fCurZDepth = 0.f;
	bool m_bIsActivate = true;
	glm::vec2 m_originMid = {};
	float m_fOriginScale = 1.f;
	glm::vec2 m_uiSize = {};
	glm::vec2 m_uiMid = {};
	glm::vec2 m_originLTRB[2]{};
	UI_STATE UpdateCurUIState();
	Delegate m_uiCallback;
protected:
	void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath)override;
	
	void Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath) override;
	
public:
	void AddUIStateTex(UI_STATE eState, shared_ptr<UI> pUI)
	{
		m_uiTex[etoi(eState)] = pUI->m_uiTex[0];
		m_uiMesh[etoi(eState)] = pUI->m_uiMesh[0];
	}

	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void AddUICallBack(Func&& fp, Args&&... args)noexcept
	{
		m_uiCallback += std::bind_front(std::forward<Func>(fp), std::forward<Args>(args)...);
	}

	void ExecuteOnClickEvent()
	{
		if (m_onClickEvent)
		{
			m_onClickEvent();
		}
	}

	void ExecuteClickedEvent() {
		if (m_ClickedEvent)
		{
			m_ClickedEvent();
		}
	}
	UI();
	void SetUIScale(const float _fRatio);
	void SetZDepthUI();
	virtual ~UI();
	UI(const glm::vec2 midPos, string_view strTexName,const float scaleFactor = 1.f,glm::vec2 startUV = {0,0},glm::vec2 endUV = {1,1});
	UI(const UI&) = delete;
	UI& operator = (const UI&) = delete;

	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void AddOnClickEvent(Func&& fp, Args&&... args) { m_onClickEvent += std::bind_front(std::forward<Func>(fp),std::forward<Args>(args)...); }
	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void AddClickedEvent(Func&& fp, Args&&... args) { m_ClickedEvent += std::bind_front(std::forward<Func>(fp), std::forward<Args>(args)...); }

	void ResetUIState() { m_eCurUIState = UI_STATE::NONE; }
	UI_STATE GetCurUIState()const { return m_eCurUIState; }

	void Update();
	void Render() override;

	auto operator <=> (const UI& _other) const {return m_fCurZDepth <=> _other.m_fCurZDepth;}

	bool IsActivate()const { return m_bIsActivate; }
	void SetActivate(bool b_ = false) { m_bIsActivate = b_; }
	auto GetUILTRB()const { return std::make_pair(m_arrLTRB[LT], m_arrLTRB[RB]); }

	template<typename T>
		requires std::derived_from<T,UI>
	static shared_ptr<T> CreateUI(const glm::vec2& _LT, const glm::vec2& _RB);
};

template<typename T>
	requires std::derived_from<T, UI>
inline shared_ptr<T> UI::CreateUI(const glm::vec2& _LT, const glm::vec2& _RB)
{
	struct make_shared_enabler :public T {
		void Render()override { T::Render(); }
		make_shared_enabler(const glm::vec2& _LT, const glm::vec2& _RB) :T( _LT,_RB ){}
	};
	auto pUI = make_shared<make_shared_enabler>(_LT, _RB);
	pUI->InitGameObj();
	return pUI;
}
