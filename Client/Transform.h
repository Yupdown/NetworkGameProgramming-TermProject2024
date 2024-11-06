#pragma once
#include "Component.h"
#include "ThreadMgr.h"

class Camera;

class Transform :
	public Component
	, public enable_shared_from_this<Transform>
{
	
	static constexpr glm::vec3 ZERO_VEC = glm::vec3{ 0.f,0.f,0.f };
	static const glm::quat defaultQuat;
public:
	shared_ptr<Component> Comp_Clone()const override
	{
		auto newTrans = std::make_shared<Transform>(*this);
		newTrans->m_bDirty = true;
		newTrans->m_vecChild.clear();
		newTrans->m_pParent.reset();
		return newTrans;
	}
private:
	weak_ptr<Transform> m_pParent;
	vector<weak_ptr<Transform>> m_vecChild;
	bool    m_bDirty = true;
private:
	/*void MakeDirty() { Mgr(ThreadMgr)->Enqueue(&Transform::MakeDirtyByThread, this); }
	void MakeDirtyByThread()
	{
		if (m_bDirty || IsUpdated(UPDATE_TYPE::UPDATE))
		{
			return;
		}

		m_bDirty = true;

		for (const auto& child : m_vecChild)
		{
			if (auto trans = child.lock())
			{
				Mgr(ThreadMgr)->Enqueue(&Transform::MakeDirtyByThread, trans.get());
			}
		}
	}*/
	void MakeDirty()noexcept
	{
		if (m_bDirty)
		{
			return;
		}

		m_bDirty = true;

		for (const auto& child : m_vecChild)
		{
			child.lock()->MakeDirty();
		}
	}
private:
	glm::vec3 m_localPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat m_localRotation = glm::angleAxis(glm::radians(0.0f), glm::vec3{0.0f, 1.0f, 0.0f});
	glm::vec3 m_localScale = glm::vec3(1.f, 1.f, 1.f);

	glm::mat4 m_matLocal = glm::mat4(1.0f);
	glm::mat4 m_matWorld = glm::mat4(1.0f);

	glm::vec3 m_revOffset = glm::vec3{};
	glm::vec3 m_posOffset = glm::vec3{};
	glm::vec3 m_rotatePivot = glm::vec3{};

	glm::quat m_rotOffsetLocal = Transform::defaultQuat;
	glm::quat m_rotOffsetWorld = Transform::defaultQuat;
public:
	Transform();
	~Transform();
	void AddChild(shared_ptr<Transform> _pChild)
	{
		auto pNewParent = shared_from_this();
		if (pNewParent == _pChild)
		{
			return;
		}
		if (const auto pOldParent = _pChild->m_pParent.lock())
		{
			if (pOldParent == pNewParent)
			{
				return;
			}
			else
			{
				std::erase_if(pOldParent->m_vecChild, [&_pChild](const weak_ptr<Transform>& _pTrans) {
					return _pTrans.lock() == _pChild;
					});
			}
		}
		_pChild->m_pParent = std::move(pNewParent);
		m_vecChild.emplace_back(std::move(_pChild));
	}
	const std::weak_ptr<Transform>& GetParent()const { return m_pParent; }
	std::shared_ptr<Transform> GetParent() { return m_pParent.lock(); }
public:
	const bool IsDirty()const noexcept { return m_bDirty; }
	void ResetY()noexcept { m_localPosition.y = m_posOffset.y = m_revOffset.y = 0.f; MakeDirty(); }
	void AddPosition(const glm::vec3& pos_)noexcept { m_posOffset += pos_; MakeDirty(); }
	void SetMatrix(const glm::mat4& transformMat_)noexcept {
		std::tie(m_localPosition, m_localRotation, m_localScale) = ::Decompose(transformMat_);
		m_matLocal = transformMat_;
		MakeDirty();
	}
	const glm::mat4 GetWorldToLocal()const noexcept { return glm::inverse(m_matWorld); }
	vector<weak_ptr<Transform>>& GetChildTransform() { return m_vecChild; }

	const glm::vec3& GetLocalPosition()const { return m_localPosition; }
	const glm::quat& GetLocalRotation()const { return m_localRotation; }
	const glm::vec3& GetLocalScale() const { return m_localScale; }

	glm::vec3 GetLocalPositionAcc()const noexcept { return m_localPosition + m_posOffset + m_revOffset; }
	glm::quat GetLocalRotationAcc()const noexcept { return m_rotOffsetWorld * m_localRotation * m_rotOffsetLocal; }


	const glm::mat4& GetLocalToWorldMatrix()const noexcept { return m_matWorld; }
	const glm::mat4& GetLocalMatrix()const noexcept { return m_matLocal; }

	glm::vec3 GetWorldPosition() const noexcept { return glm::vec3(m_matWorld[3]); }

	inline const glm::quat GetLocalRotationRecursion()const noexcept{
		return GetWorldRotationRecursion() * glm::inverse(m_localRotation);
	}

	inline glm::quat GetWorldRotationRecursion(const glm::quat _accRotate = Transform::defaultQuat)const noexcept {
		return m_pParent.expired() ? m_localRotation * _accRotate : m_pParent.lock()->GetWorldRotationRecursion(m_localRotation * _accRotate);
	}

	inline glm::vec3 GetWorldPositionRecursion(const glm::vec3 _accPos = Transform::ZERO_VEC)const noexcept {
		return m_pParent.expired() ? m_localPosition + m_posOffset + m_revOffset + _accPos : m_pParent.lock()->GetWorldPositionRecursion(m_localPosition + m_posOffset + m_revOffset + _accPos);
	}

	inline glm::quat GetWorldRotationAccRecursion(const glm::quat _accRotate = Transform::defaultQuat)const noexcept {
		return m_pParent.expired() ? GetLocalRotationAcc() * _accRotate : m_pParent.lock()->GetWorldRotationAccRecursion(GetLocalRotationAcc() * _accRotate);
	}

	inline glm::vec3 GetWorldPositionAccRecursion(const glm::vec3 _accPos = Transform::ZERO_VEC)const noexcept {
		return m_pParent.expired() ? GetLocalPositionAcc() + _accPos : m_pParent.lock()->GetWorldPositionAccRecursion(GetLocalPositionAcc() + _accPos);
	}

	glm::vec3 GetRight()const noexcept { return glm::vec3(m_matWorld[0]); }
	glm::vec3 GetUp() const noexcept { return glm::vec3(m_matWorld[1]); }
	glm::vec3 GetLook() const noexcept { return glm::vec3(m_matWorld[2]); }

	glm::vec3 GetRightRecursion()const noexcept { return GetRightByQuat(GetWorldRotationRecursion()); }
	glm::vec3 GetUpRecursion() const noexcept { return GetUpByQuat(GetWorldRotationRecursion()); }
	glm::vec3 GetLookRecursion() const noexcept { return GetLookByQuat(GetWorldRotationRecursion()); }

	void SetPivot(const glm::vec3& _pivot)noexcept { m_rotatePivot = _pivot; }

	void SetLocalPosition(const glm::vec3& position) { m_localPosition = position; MakeDirty(); }
	void SetLocalRotation(const glm::quat& rotation) { m_localRotation = rotation; MakeDirty(); }
	void SetLocalScale(const glm::vec3& scale) { m_localScale = scale; MakeDirty(); }
	void SetLocalScale(const float _fScale) { m_localScale = glm::vec3{ _fScale }; MakeDirty(); }

	void SetLocalRotation(const float _degree) { m_localRotation = glm::angleAxis(glm::radians(_degree), GetUpRecursion()); MakeDirty(); }
	void SetLocalRotation(const float _degree, const glm::vec3& _axis) { m_localRotation = glm::angleAxis(glm::radians(_degree), glm::normalize(_axis)); MakeDirty(); }
	void SetLocalRotation(const float _xDeg, const float _yDeg, const float _zDeg) { m_localRotation = glm::quat{ glm::radians(glm::vec3{_xDeg,_yDeg,_zDeg}) }; MakeDirty(); }

	void SetLocalRevolution(const float _degree, const glm::vec3& _axis, const glm::vec3& _center)
	{
		const glm::vec3 worldPosition = GetWorldPositionRecursion();

		assert(!IsSameVector(worldPosition, _center));

		const glm::vec3 aDir = worldPosition - _center;
		const float aLen = glm::length(aDir);
		const glm::vec3 bDir = GetUpRecursion();
		const float angle = glm::acos(glm::dot(aDir / aLen, bDir));
		const float bLen = glm::cos(angle) * aLen;
		const glm::vec3 cDir = aDir - bDir * bLen;
		const glm::quat rotateQuat = glm::angleAxis(glm::radians(_degree), bDir);
		const glm::vec3 destDir = rotateQuat * cDir;
		m_localPosition = (destDir - cDir) + aDir;
		MakeDirty();
	}

	void AddLocalRotation(const float _degree)
	{
		MakeDirty();
		const glm::quat worldRotation = GetWorldRotationRecursion();
		const glm::quat newRotation = glm::angleAxis(glm::radians(_degree), GetUpByQuat(worldRotation) * glm::normalize(worldRotation * glm::inverse(m_localRotation)));
		//SPIN_LOCK;
		m_rotOffsetWorld *= newRotation;
	}

	void AddLocalRotation(const float _degree, const glm::vec3& _axis)
	{
		MakeDirty();
		const glm::quat newRotation = glm::angleAxis(glm::radians(_degree), glm::normalize(_axis));
		//SPIN_LOCK;
		m_rotOffsetLocal *= newRotation;
	}

	void AddLocalRotation(const float _xDeg, const float _yDeg, const float _zDeg)
	{
		MakeDirty();
		m_rotOffsetLocal *= glm::quat{ glm::radians(glm::vec3{_xDeg,_yDeg,_zDeg}) };
	}

	void SetLookAt(const glm::vec3& target)
	{
		const glm::vec3 direction = glm::normalize(target - GetWorldPositionRecursion());
		m_localRotation = glm::quatLookAtLH(direction, GetUpRecursion());
		MakeDirty();
	}

	void AddLocalRevolution(const float _degree, const glm::vec3& _center)
	{
		MakeDirty();
		const glm::vec3 worldPosition = GetWorldPositionRecursion();

		assert(!IsSameVector(worldPosition, _center));

		const glm::vec3 aDir = worldPosition - _center;
		const float aLen = glm::length(aDir);
		const glm::vec3 bDir = GetUpRecursion();
		const float angle = glm::acos(glm::dot(aDir / aLen, bDir));
		const float bLen = glm::cos(angle) * aLen;
		const glm::vec3 cDir = aDir - bDir * bLen;
		const glm::quat rotateQuat = glm::angleAxis(glm::radians(_degree), bDir);
		const glm::vec3 destDir = rotateQuat * cDir;

		//SPIN_LOCK;
		m_revOffset += (destDir - cDir);

	}

	void AddWorldRevolution(const float _degree, const glm::vec3& _axis, const glm::vec3& _center)
	{
		MakeDirty();
		const glm::vec3 worldPosition = GetWorldPositionRecursion();

		assert(!IsSameVector(worldPosition, _center));

		const glm::vec3 aDir = worldPosition - _center;
		const float aLen = glm::length(aDir);
		const glm::vec3 bDir = glm::normalize(_axis);
		const float angle = glm::acos(glm::dot(aDir / aLen, bDir));
		const float bLen = glm::cos(angle) * aLen;
		const glm::vec3 cDir = aDir - bDir * bLen;
		const glm::quat rotateQuat = glm::angleAxis(glm::radians(_degree), bDir);
		const glm::vec3 destDir = rotateQuat * cDir;

		//SPIN_LOCK;
		m_revOffset += (destDir - cDir);

	}

	void AddLocalPosition(const glm::vec3& position)
	{
		MakeDirty();
		//SPIN_LOCK;
		m_posOffset += position;
	}

	void AddLocalScale(const glm::vec3& scale_)noexcept { m_localScale += scale_; MakeDirty(); }
	void AddLocalScale(const float fScale_)noexcept { m_localScale += glm::vec3{ fScale_ }; MakeDirty(); }

	void AddFrontBack(const float _fSpeed)
	{
		AddLocalPosition(GetLookRecursion() * _fSpeed);
	}

	void AddLeftRight(const float _fSpeed)
	{
		AddLocalPosition(GetRightRecursion() * _fSpeed);
	}

	void AddUpDown(const float _fSpeed)
	{
		AddLocalPosition(GetUpRecursion() * _fSpeed);
	}

	void AddWorldRotation(const float _degree, const glm::vec3& _axis)
	{
		MakeDirty();
		const glm::quat newRotation = glm::angleAxis(glm::radians(_degree), glm::normalize(_axis));
		//SPIN_LOCK;
		m_rotOffsetWorld = newRotation * m_rotOffsetWorld;
	}

	void AddWorldRotation(const float _xDeg, const float _yDeg, const float _zDeg)
	{
		MakeDirty();
		m_rotOffsetWorld = glm::quat{ glm::radians(glm::vec3{_xDeg,_yDeg,_zDeg}) } *m_rotOffsetWorld;
	}
	
	inline void MakeFinalMat()noexcept
	{
		m_localPosition = m_localPosition + m_revOffset + m_posOffset;
		m_localRotation = glm::normalize(m_rotOffsetWorld * m_localRotation * m_rotOffsetLocal);

		const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_localPosition);
		const glm::mat4 rotationMatrix = glm::mat4_cast(m_localRotation);
		const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_localScale);

		const glm::mat4 pivotMatrix = glm::translate(glm::mat4(1.0f), m_rotatePivot);

		m_matLocal = glm::inverse(pivotMatrix) * translationMatrix * rotationMatrix * scaleMatrix * pivotMatrix;

		if (const auto parent = m_pParent.lock())
		{
			m_matWorld = parent->GetLocalToWorldMatrix() * m_matLocal;
		}
		else
		{
			m_matWorld = m_matLocal;
		}

		m_posOffset = m_revOffset = Transform::ZERO_VEC;
		m_rotOffsetLocal = m_rotOffsetWorld = Transform::defaultQuat;
	}

	void UpdateTransfromHierarchyForPrepare()noexcept
	{
		MakeFinalMat();
		for (const auto& child : m_vecChild)
		{
			child.lock()->MakeFinalMat();
		}
	}

	void UpdateTransfromHierarchy()noexcept
	{
		if (!m_bDirty)
		{
			for (const auto& child : m_vecChild)
			{
				Mgr(ThreadMgr)->Enqueue(&Transform::UpdateTransfromHierarchy, child.lock().get());
			}
			return;
		}
		MakeFinalMat();
		for (const auto& child : m_vecChild)
		{
			Mgr(ThreadMgr)->Enqueue(&Transform::UpdateTransfromHierarchy, child.lock().get());
		}
		m_bDirty = false;
	}

	void FinalUpdate()override
	{
		if (!m_bDirty)
		{
			return;
		}
		MakeFinalMat();
	}
	void ClearTransformFlag()noexcept { m_bDirty = false; }

	virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath) override
	{
		writer.String("Transform");

		writer.Key("Scale");
		writer.StartArray();
		writer.Double(m_localScale.x);
		writer.Double(m_localScale.y);
		writer.Double(m_localScale.z);
		writer.EndArray();

		writer.Key("Translation");
		writer.StartArray();
		writer.Double(m_localPosition.x);
		writer.Double(m_localPosition.y);
		writer.Double(m_localPosition.z);
		writer.EndArray();

		const glm::vec3 rot = glm::degrees(glm::eulerAngles(m_localRotation));
		writer.Key("Rotation");
		writer.StartArray();
		writer.Double(rot.x);
		writer.Double(rot.y);
		writer.Double(rot.z);
		writer.EndArray();
	}

	virtual void Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath) override
	{
		//if (doc.HasMember("Scale"))
		{
			const rapidjson::Value& scaleArray = doc["Scale"].GetArray();
			//if (scaleArray.Size() == 3)
			{
				const float scaleX = (float)scaleArray[0].GetDouble();
				const float scaleY = (float)scaleArray[1].GetDouble();
				const float scaleZ = (float)scaleArray[2].GetDouble();
				const glm::vec3 scale(scaleX, scaleY, scaleZ);
				m_localScale = scale;
			}
		}

		//if (doc.HasMember("Translation"))
		{
			const rapidjson::Value& translationArray = doc["Translation"].GetArray();
			//if (translationArray.Size() == 3)
			{
				const float translationX = (float)translationArray[0].GetDouble();
				const float translationY = (float)translationArray[1].GetDouble();
				const float translationZ = (float)translationArray[2].GetDouble();
				const glm::vec3 translation(translationX, translationY, translationZ);
				m_localPosition = translation;
			}
		}

		//if (doc.HasMember("Rotation"))
		{
			const rapidjson::Value& rotationArray = doc["Rotation"];
			//if (rotationArray.Size() == 3)
			{
				const float rotX = (float)rotationArray[0].GetDouble();
				const float rotY = (float)rotationArray[1].GetDouble();
				const float rotZ = (float)rotationArray[2].GetDouble();
				const glm::vec3 rotation(rotX, rotY, rotZ);
				m_localRotation = glm::quat{ glm::radians(rotation) };
			}
		}
	}
};

