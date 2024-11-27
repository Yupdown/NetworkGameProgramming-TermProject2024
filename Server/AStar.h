#pragma once
#include "pch.h"

class AStar
{
public:
    static constexpr glm::ivec3 g_directions[] =
    {
        // Y가 음의 방향인 녀석들 우선
   glm::ivec3{ 1,-1, 1 },   // +X -Y +Z
   glm::ivec3{ 1,-1, 0 },   // +X -Y
   glm::ivec3{ 1,-1,-1 },   // +X -Y -Z
   glm::ivec3{ 0,-1, 1 },   // -Y +Z
   glm::ivec3{ 0,-1, 0 },   // -Y
   glm::ivec3{ 0,-1,-1 },   // -Y -Z
   glm::ivec3{-1,-1, 1 },   // -X -Y +Z
   glm::ivec3{-1,-1, 0 },   // -X -Y
   glm::ivec3{-1,-1,-1 },   // -X -Y -Z

   // Y가 0인 녀석들
   glm::ivec3{ 1, 0, 1 },   // +X +Z
   glm::ivec3{ 1, 0, 0 },   // +X
   glm::ivec3{ 1, 0,-1 },   // +X -Z
   glm::ivec3{ 0, 0, 1 },   // +Z
   
   glm::ivec3{ 0, 0,-1 },   // -Z
   glm::ivec3{-1, 0, 1 },   // -X +Z
   glm::ivec3{-1, 0, 0 },   // -X
   glm::ivec3{-1, 0,-1 },   // -X -Z

   // Y가 양의 방향인 녀석들 뒤에 배치 (순수한 Y 방향은 제외)
   glm::ivec3{ 1, 1, 1 },   // +X +Y +Z
   glm::ivec3{ 1, 1, 0 },   // +X +Y
   glm::ivec3{ 1, 1,-1 },   // +X +Y -Z
   glm::ivec3{ 0, 1, 1 },   // +Y +Z
   glm::ivec3{ 0, 1, 0 },   // +Y
   glm::ivec3{ 0, 1,-1 },   // +Y -Z
   glm::ivec3{-1, 1, 1 },   // -X +Y +Z
   glm::ivec3{-1, 1, 0 },   // -X +Y
   glm::ivec3{-1, 1,-1 }    // -X +Y -Z
    };
    struct Node {
        glm::ivec3 xyz;
        int g=99999;
        int h=99999;
        int f() const { return g + h; }
        Node()noexcept = default;
        Node operator+(const glm::ivec3& v)const noexcept { 
            return Node{ xyz + v,g,h };
        }
        Node(const glm::ivec3& v)noexcept :xyz{ v }, g{}, h{} {}
        Node(const glm::ivec3& v, const int g_, const int h_)noexcept :xyz{ v }, g{ g_ }, h{ h_ } {}
        bool operator<(const Node& other) const { return f() > other.f(); }
        bool operator==(const Node& other) const { return xyz == other.xyz; }
    };
    static int mdist(const glm::ivec3& a, const glm::ivec3& b)noexcept {
        return glm::abs(a.x - b.x) + glm::abs(a.y - b.y) + glm::abs(a.z - b.z);
    }
    static int udist(const glm::ivec3& a, const glm::ivec3& b)noexcept {
        const int dx = a.x - b.x;
        const int dy = a.y - b.y;
        const int dz = a.z - b.z;
        return dx * dx + dy * dy + dz * dz;
    }
private:
    static std::vector<glm::ivec3> reconstruct_path(const std::unordered_map<glm::ivec3, glm::ivec3>& came_from, const glm::ivec3& current);
public:
    static std::vector<glm::ivec3> GetAStarPath(const glm::ivec3& start, const glm::ivec3& goal);
private:

};

namespace std {
    template <>
    struct hash<glm::ivec3> {
        std::size_t operator()(const glm::ivec3& node) const {
            return std::hash<int>()(node.x)
                ^ (std::hash<int>()(node.y) << 1)
                ^ (std::hash<int>()(node.z) << 2);
        }
    };
    template <>
    struct hash<AStar::Node> {
        std::size_t operator()(const AStar::Node& node) const {
            return std::hash<int>()(node.xyz.x)
                ^ (std::hash<int>()(node.xyz.y) << 1)
                ^ (std::hash<int>()(node.xyz.z) << 2);
        }
    };
}
