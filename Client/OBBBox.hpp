#pragma once
#include "pch.h"

struct OBBBox 
{
private:
    static constexpr const glm::vec3 offsets[8] = {
        glm::vec3(0.5f, 0.0f, 0.5f),
        glm::vec3(-0.5f, 0.0f, 0.5f),
        glm::vec3(0.5f, 0.0f, -0.5f),
        glm::vec3(-0.5f, 0.0f, -0.5f),
        glm::vec3(0.5f, 1.0f, 0.5f),
        glm::vec3(-0.5f, 1.0f, 0.5f),
        glm::vec3(0.5f, 1.0f, -0.5f),
        glm::vec3(-0.5f, 1.0f, -0.5f)
    };

    const glm::vec3 CalculateCenter() const noexcept {
        return glm::vec3(transform * glm::vec4(centerOffset, 1.f));
    }

    const glm::vec3 CalculateActualExtents() const noexcept {
        const glm::vec3 scales = glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));
        return baseExtents * scales;
    }

    const glm::mat4 CalculateRotationMatrix() const noexcept {
        const glm::vec3 scales = glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0] = transform[0] / scales.x;
        rotation[1] = transform[1] / scales.y;
        rotation[2] = transform[2] / scales.z;
        return rotation;
    }
    glm::vec3 cacheCenter = glm::vec3{ 0,0,0 };
    glm::vec3 cacheExtends = glm::vec3{ 0,0,0 };
    glm::mat4 cacheRotationMat = glm::mat4{ 1.f };
    std::array<glm::vec3, 8> corners;
public:
    glm::mat4 transform = glm::mat4{ 1.f };
    glm::vec3 baseExtents = glm::vec3{ 0,0,0 };
    glm::vec3 centerOffset = glm::vec3{ 0,0,0 };
    
    void UpdateOBB()noexcept
    {
        cacheCenter = CalculateCenter();
        cacheExtends = CalculateActualExtents();
        cacheRotationMat = CalculateRotationMatrix();

        glm::mat4 reversedZRotation = cacheRotationMat;
        //reversedZRotation[2] = -reversedZRotation[2];
        const auto cornersCache = corners.data();
        for (ushort i = 0; i < 8; ++i)
        {
            cornersCache[i] = cacheCenter + glm::vec3(reversedZRotation * glm::vec4(cacheExtends * offsets[i], 0));
        }
        //transform[2] = -transform[2];
    }

    const glm::vec3 getAxis(unsigned short index) const noexcept{return glm::normalize(glm::vec3(transform[index]));}
    const glm::vec3& getCenter() const noexcept { return cacheCenter; }
    const glm::vec3& getActualExtents() const noexcept { return cacheExtends; }
    const glm::mat4& getRotationMatrix() const noexcept { return cacheRotationMat; }
    const glm::mat4 getFinalMatrix() const noexcept {
        const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), getActualExtents());
        const glm::mat4 offsetMatrix = glm::translate(glm::mat4(1.0f), getCenter());
        return offsetMatrix * getRotationMatrix() * scaleMatrix;
    }

    std::span<const glm::vec3> GetCorners()const noexcept { return corners; }
};