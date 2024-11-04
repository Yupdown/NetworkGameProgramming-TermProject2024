#pragma once
#include "pch.h"

struct Vertex
{
	glm::vec3 position = glm::vec3{ 0.f,0.f,0.f };
	glm::vec3 normal = glm::vec3{ 1.f,1.f,1.f };
	glm::vec3 tangent = glm::vec3{ 1.f,1.f,1.f };
	glm::vec2 uv = glm::vec2{ 0.f,0.f };
	glm::vec4 color = glm::vec4{ 0.f,0.f,0.f,0.f };

	const bool operator==(const Vertex& other) const noexcept {
		return position == other.position && normal == other.normal && uv == other.uv;
	}

	//glm::ivec4 boneIds = {};
	//glm::vec4 boneWeights = {};

	//Vertex() = default;
	//  Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uvs, glm::vec4 col = glm::vec4(0.f))
    //    : position(pos), normal(norm), tangent(glm::vec3(0.f)), uv(uvs), color(col) {}
	//Vertex(const glm::vec3& _v):position{_v}{}
};