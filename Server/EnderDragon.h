#pragma once
#include "Component.h"

class EnderDragon
	:public Component
{
private:
    static glm::vec3 bezier(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const float t) noexcept
    {
        return (1 - t) * (1 - t) * p0 +
            2 * (1 - t) * t * p1 +
            t * t * p2;
    }
    void GenerateArcLengthTable(
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const int samples = 50) noexcept
    {
        auto& arcLengthTable = m_arcLengthTable;
        arcLengthTable.clear();

        float totalLength = 0.0f;

        glm::vec3 previousPoint = p0;
        arcLengthTable.emplace_back(0.0f, 0.0f);

        for (int i = 1; i <= samples; ++i)
        {
            const float t = static_cast<float>(i) / samples;
            const glm::vec3 currentPoint = bezier(p0, p1, p2, t);
            totalLength += glm::length(currentPoint - previousPoint);
            arcLengthTable.emplace_back(t, totalLength);
            previousPoint = currentPoint;
        }
    }
    float MapDistanceToT(const float distance) noexcept
    {
        const auto arcLengthTable = m_arcLengthTable.data();
        const auto num = (int)m_arcLengthTable.size();

        for (int i = 1; i < num; ++i)
        {
            const float prevLength = arcLengthTable[i - 1].second;
            const float currLength = arcLengthTable[i].second;

            if (distance <= currLength)
            {
                const float t1 = arcLengthTable[i - 1].first;
                const float t2 = arcLengthTable[i].first;

                return t1 + (distance - prevLength) / (currLength - prevLength) * (t2 - t1);
            }
        }
        return 1.0f; 
    }
    void SetBezierPoints()noexcept;
public:
	void Init();
	void Update(const float DT)override;
private:
	std::vector<glm::vec3> m_randomPoints;
    glm::vec3 m_oldPos = {};

	float m_bezierT = 0.f;
	float m_curveLength = 0.f;
	float m_speed = 10.f;

    glm::vec3 m_points[3];

    std::vector<std::pair<float, float>> m_arcLengthTable;
    float m_traveledDistance = 0.0f;
};

