#pragma once
#include "Component.h"

class Light
	:public Component
	,public enable_shared_from_this<Light>
{
	friend class Scene;
	COMP_CLONE(Light)
private:
	shared_ptr<Light> m_curLight;
protected:
	glm::vec3 m_position = glm::vec3{};
	glm::vec3 m_ambient = glm::vec3{ .3f,.3f,.3f };
	glm::vec3 m_diffuse = glm::vec3{ .5f,.5f,.5f };
	glm::vec3 m_specular = glm::vec3{.5f,.5f,.5f};
	glm::vec3 m_direction = glm::vec3{ 1.f };
	float m_fConstant = 1.f;
	float m_fLinear =.1f;
	float m_fQuadratic =.1f;
protected:
	void PushLightData(const GLint _curShaderID,const int _idx)const;
	virtual void PushLightData()const noexcept;
	void LightUpdate()noexcept;
public:
	Light();
	~Light();
	void FinalUpdate() override;
public:
	void SetLightPos(const glm::vec3& _position);
	void SetAmbient(const glm::vec3& _ambient) noexcept {
		m_ambient = _ambient; 
		if (m_curLight)
		{
			m_curLight->m_ambient = _ambient;
		}
	}
	void SetDiffuse(const glm::vec3& _diffuse)  noexcept {
		m_diffuse = _diffuse;
		if (m_curLight)
		{
			m_curLight->m_diffuse = _diffuse;
		}
	}
	void SetSpecular(const glm::vec3& _specular)  noexcept {
		m_specular = _specular; 
		if (m_curLight)
		{
			m_curLight->m_specular = _specular;
		}
	}
	shared_ptr<Light> GetCurLight() noexcept {
		return m_curLight ? m_curLight : shared_from_this();
	}

	shared_ptr<Light> SetCurLightType(const LIGHT_TYPE _eType);
	shared_ptr<Light> SetCurLightType(shared_ptr<Light> pLight) {
		m_curLight = pLight;
		return pLight;
	}

	void SetConstantLinearQuad(const glm::vec3 floats)noexcept
	{
		m_fConstant = floats.x;
		m_fLinear = floats.y;
		m_fQuadratic = floats.z;
	}

	const glm::vec3& GetLightPos()const { return m_position; }
	const glm::vec3& GetLightAmbient()const { return m_ambient; }
	const glm::vec3& GetLightDiffuse()const { return m_diffuse; }
	const glm::vec3& GetLightSpecular()const { return m_specular; }

	virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath) override
	{
		writer.String("Light");

		writer.Key("Position");
		writer.StartArray();
		writer.Double(m_position.x);
		writer.Double(m_position.y);
		writer.Double(m_position.z);
		writer.EndArray();

		writer.Key("Ambient");
		writer.StartArray();
		writer.Double(m_ambient.r);
		writer.Double(m_ambient.g);
		writer.Double(m_ambient.b);
		writer.EndArray();

		writer.Key("Diffuse");
		writer.StartArray();
		writer.Double(m_diffuse.r);
		writer.Double(m_diffuse.g);
		writer.Double(m_diffuse.b);
		writer.EndArray();

		writer.Key("Specular");
		writer.StartArray();
		writer.Double(m_specular.r);
		writer.Double(m_specular.g);
		writer.Double(m_specular.b);
		writer.EndArray();
	}

	virtual void Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath) override
	{
		const rapidjson::Value& positionValue = doc["Position"];
		//if (positionValue.IsArray() && positionValue.Size() == 3)
		{
			const float positionX = (float)positionValue[0].GetDouble();
			const float positionY = (float)positionValue[1].GetDouble();
			const float positionZ = (float)positionValue[2].GetDouble();
			const glm::vec3 position(positionX, positionY, positionZ);
			m_position = position;
		}

		const rapidjson::Value& ambientValue = doc["Ambient"];
		//if (ambientValue.IsArray() && ambientValue.Size() == 3)
		{
			const float ambientR = (float)ambientValue[0].GetDouble();
			const float ambientG = (float)ambientValue[1].GetDouble();
			const float ambientB = (float)ambientValue[2].GetDouble();
			const glm::vec3 ambient(ambientR, ambientG, ambientB);
			m_ambient = ambient;
		}

		const rapidjson::Value& diffuseValue = doc["Diffuse"];
		//if (diffuseValue.IsArray() && diffuseValue.Size() == 3)
		{
			const float diffuseR = (float)diffuseValue[0].GetDouble();
			const float diffuseG = (float)diffuseValue[1].GetDouble();
			const float diffuseB = (float)diffuseValue[2].GetDouble();
			const glm::vec3 diffuse(diffuseR, diffuseG, diffuseB);
			m_diffuse = diffuse;
		}

		const rapidjson::Value& specularValue = doc["Specular"];
		//if (specularValue.IsArray() && specularValue.Size() == 3)
		{
			const float specularR = (float)specularValue[0].GetDouble();
			const float specularG = (float)specularValue[1].GetDouble();
			const float specularB = (float)specularValue[2].GetDouble();
			const glm::vec3 specular(specularR, specularG, specularB);
			m_specular = specular;
		}
	}
};

