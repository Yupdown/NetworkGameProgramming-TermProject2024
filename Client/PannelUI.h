#pragma once
#include "UI.h"

class Button;

class PannelUI
	:public UI
{
private:
	weak_ptr<PannelUI> m_parentUI;
	vector<shared_ptr<PannelUI>> m_childUI;
	vector<shared_ptr<Button>> m_vecButton;
private:
	void ResetUIState();
public:
	 PannelUI();
	~PannelUI();
public:
	shared_ptr<PannelUI> GetRootUI() {
		return m_parentUI.expired() ? this->PannelUI::shared_from_this() : m_parentUI.lock()->GetRootUI();
	}
	
	void SetZDepth();
	void DragMoveUI();
	void SetUIScaleAll(const float _fRatio);
	void AddChild(shared_ptr<PannelUI> _pUI);
	void AddButton(shared_ptr<Button> _pButton);
	void ExecuteClickEvent();
	void SetUIPosition(const glm::vec2& WCpos_);
public:
	PannelUI(const glm::vec2 midPos, string_view strTexName, const float scaleFactor = 1.f, glm::vec2 startUV = { 0,0 }, glm::vec2 endUV = { 1,1 });
	shared_ptr<const PannelUI> shared_from_this() const { return static_pointer_cast<const PannelUI>(GameObj::shared_from_this());}
	shared_ptr<PannelUI> shared_from_this() { return static_pointer_cast<PannelUI>(GameObj::shared_from_this()); }
	class iterator_UI
	{
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = shared_ptr<PannelUI>;
		using difference_type = std::ptrdiff_t;
		using pointer = shared_ptr<PannelUI>*;
		using reference = shared_ptr<PannelUI>&;
	private:
		mutable shared_ptr<PannelUI> m_curObj = nullptr;
		std::queue<shared_ptr<PannelUI>> m_bfsQ = {};
	public:
		iterator_UI() = default;
		~iterator_UI() = default;
		iterator_UI(shared_ptr<PannelUI> _root) :m_curObj{ std::move(_root) } { m_bfsQ.emplace(nullptr); }
		const bool operator != (const iterator_UI& other) const { return m_curObj != other.m_curObj; }
		const bool operator == (const iterator_UI& other) const { return m_curObj == other.m_curObj; }
		reference operator * ()const { return m_curObj; }
		pointer operator -> ()const { return &m_curObj; }
		iterator_UI& operator ++() {
			m_bfsQ.pop();
			for (auto& child : m_curObj->m_childUI)
				m_bfsQ.emplace(child);
			m_curObj = m_bfsQ.empty() ? nullptr : std::move(m_bfsQ.front());
			return *this;
		}
		iterator_UI operator ++ (int) {
			iterator_UI temp{ *this };
			++(*this);
			return temp;
		}
	};
	iterator_UI begin() { return iterator_UI{ this->PannelUI::shared_from_this() }; }
	iterator_UI end() { return iterator_UI{ nullptr }; }
	iterator_UI begin()const { return iterator_UI{ std::const_pointer_cast<PannelUI>(this->PannelUI::shared_from_this()) }; }
	iterator_UI end()const { return iterator_UI{ nullptr }; }
};

