#pragma once
#include "Component.h"
#include "OBBBox.hpp"

class CollisionHandler;

class Collider
	:public Component
	,public enable_shared_from_this<Collider>
{
	COMP_CLONE(Collider)
	static GLuint g_iNextID;
private:
	GLuint m_iColliderID;
	GLint m_iCollisionCount = 0;
	OBBBox	m_OBBBox;
	shared_ptr<Transform> m_pTransform;
	shared_ptr<CollisionHandler> m_pCollisionHandler;
	bool m_bIsPickingObj = false;
private:
	void UpdateOBB()noexcept { m_OBBBox.UpdateOBB(); }
public:
	Collider();
	~Collider();
	Collider(const Collider& other);
	void Awake()override;
	shared_ptr<RigidBody> GetRigidBody()const noexcept;
	void FinalUpdate() override;
	void SetCollisionHandler(shared_ptr<CollisionHandler> pColHandle_) { m_pCollisionHandler = std::move(pColHandle_); }
	void OnCollisionEnter(shared_ptr<Collider> _pOther);
	void OnCollisionStay(shared_ptr<Collider> _pOther);
	void OnCollisionExit(shared_ptr<Collider> _pOther);
public:
	void SetIsPicking(const bool b_)noexcept { m_bIsPickingObj = b_; }
	const bool IsPickingObj()const noexcept { return m_bIsPickingObj; }
public:
	const GLint GetColCount()const noexcept { return m_iCollisionCount; }
	const OBBBox& GetOBB()const { return m_OBBBox; }
	void SetColBoxScale(const glm::vec3& vScale_) { m_OBBBox.baseExtents = vScale_; }
	void SetColBoxOffset(const glm::vec3& vOffset_) { m_OBBBox.centerOffset = vOffset_; }
	const shared_ptr<Transform>& GetColliderTransform()const { return m_pTransform; }
	const GLuint GetID()const { return m_iColliderID; }
	const shared_ptr<CollisionHandler>& GetCollisionHandler()const { return m_pCollisionHandler; }
	const bool IsDead()const;
	void UpdateCollider()noexcept {
		m_OBBBox.transform = m_pTransform->GetLocalToWorldMatrix();
		UpdateOBB();
	}
	virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath) override
	{
		writer.String("Collider");

		writer.Key("baseExtents");
		writer.StartArray();
		writer.Double(m_OBBBox.baseExtents.x);
		writer.Double(m_OBBBox.baseExtents.y);
		writer.Double(m_OBBBox.baseExtents.z);
		writer.EndArray();

		writer.Key("centerOffset");
		writer.StartArray();
		writer.Double(m_OBBBox.centerOffset.x);
		writer.Double(m_OBBBox.centerOffset.y);
		writer.Double(m_OBBBox.centerOffset.z);
		writer.EndArray();
	}

	virtual void Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath) override
	{
		const rapidjson::Value& extentsValue = doc["baseExtents"];
		//if (positionValue.IsArray() && positionValue.Size() == 3)
		{
			const float extentsX = (float)extentsValue[0].GetDouble();
			const float extentsY = (float)extentsValue[1].GetDouble();
			const float extentsZ = (float)extentsValue[2].GetDouble();
			const glm::vec3 extents(extentsX, extentsY, extentsZ);
			m_OBBBox.baseExtents = extents;
		}

		const rapidjson::Value& centerValue = doc["centerOffset"];
		//if (ambientValue.IsArray() && ambientValue.Size() == 3)
		{
			const float centerX = (float)centerValue[0].GetDouble();
			const float centerY = (float)centerValue[1].GetDouble();
			const float centerZ = (float)centerValue[2].GetDouble();
			const glm::vec3 center(centerX, centerY, centerZ);
			m_OBBBox.centerOffset = center;
		}
	}
};

