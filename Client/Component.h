#pragma once
#include "Resource.h"

class WaitLock
{
private:
	std::atomic_bool m_waitFlag = true;
public:
	void Wait()const {
		while (m_waitFlag.load(std::memory_order_seq_cst)) {
		}
	}
	void Done() {
		m_waitFlag.store(false, std::memory_order_seq_cst);
	}
	void ClearFlag() {
		m_waitFlag.store(true, std::memory_order_relaxed);
	}
};

enum class UPDATE_TYPE
{
	UPDATE,
	LATE_UPDATE,
	LAST_UPDATE,
	FINAL_UPDATE,

	END
};

class GameObj;
class Transform;

class Component
	:public Resource
{
	friend class GameObj;
	void SetOwner(weak_ptr<GameObj> _pObj) { m_pOwner = _pObj; }
	virtual shared_ptr<Component> Comp_Clone()const = 0;
	/*void ClearWaitLockAndFlag() { m_waitLock.ClearFlag();
	std::ranges::for_each(m_arrUpdateFlag, [](std::binary_semaphore& _sema) {_sema.release(); });
	}*/
protected:
	Component(const Component& _other);
protected:
	COMPONENT_TYPE m_eCompType;
	weak_ptr<GameObj> m_pOwner;
	bool m_bTransformDirty = true;
	//WaitLock m_waitLock = {};
	//SpinLock m_spinLock = {};
	/*array<std::binary_semaphore, etoi(UPDATE_TYPE::END)> m_arrUpdateFlag {
		std::binary_semaphore{1}, std::binary_semaphore{1},std::binary_semaphore{1},std::binary_semaphore{1},
	};*/
protected:
	//const bool IsUpdated(const UPDATE_TYPE _eType) { return !m_arrUpdateFlag[etoi(_eType)].try_acquire(); }
public:
	virtual void Awake(){}
	virtual void Start(){}

	virtual void Update(){}
	virtual void LateUpdate(){}
	virtual void LastUpdate(){}

	const bool IsTransformDirty()const noexcept { return m_bTransformDirty; }
	void MarkTransformDirty()noexcept;

	virtual void FinalUpdate(){}

	virtual void PreRender()const{}
	virtual void Render()const{}
public:
	Component(COMPONENT_TYPE _eType);
	virtual ~Component();

	shared_ptr<GameObj> GetGameObj()const;
	const weak_ptr<GameObj>& GetGameObjWeak()const { return m_pOwner; }
	shared_ptr<Transform> GetTransform()const noexcept;

public:
	COMPONENT_TYPE GetCompType()const { return m_eCompType; }
	bool IsValid()const { return m_pOwner.expired() == false; }
	const bool IsAlive()const noexcept;
	const glm::mat4& GetOwnerWorldTransform()const noexcept;
	//const WaitLock& GetWaitLock()const { return m_waitLock; }
};

