#include "pch.h"
#include "AStar.h"
#include "MCWorld.h"
#include "MCTilemap.h"

std::vector<glm::ivec3> AStar::reconstruct_path(const std::unordered_map<glm::ivec3, glm::ivec3>& came_from, const glm::ivec3& current) {
    std::vector<glm::ivec3> turning_points;
    turning_points.reserve(came_from.size());

    glm::ivec3 node = current;
    glm::ivec3 last_direction(0);  
    glm::ivec3 previous_node = node; 
    const auto sentinel = came_from.cend();

    turning_points.emplace_back(node); 

    for(;;)
    {
        const auto iter = came_from.find(node);
        if (iter == sentinel) break;

        glm::ivec3 next_node = iter->second;
        glm::ivec3 current_direction = next_node - node; 

       
        if (current_direction != last_direction) 
        {
            turning_points.emplace_back(previous_node);  
            turning_points.emplace_back(next_node);      
            last_direction = current_direction;         
        }

        previous_node = node;  
        node = next_node;     
    }

    return turning_points;
}


std::vector<glm::ivec3> AStar::GetAStarPath(const glm::ivec3& start, const glm::ivec3& goal)
{
    thread_local std::priority_queue<Node> open_set;
    thread_local std::unordered_map<glm::ivec3, glm::ivec3> came_from;
    thread_local std::unordered_map<Node, int> g_score;

    while (!open_set.empty()) { open_set.pop(); }
    came_from.clear();
    g_score.clear();

    open_set.emplace(start);
    g_score[start] = 0;

    Node closest_node = start;
    int closest_distance = mdist(start, goal);

    const auto& mc_tile_map = GetTileMap();
    int cnt = 0;
    while (!open_set.empty())
    {
        if (cnt == 2000) break;  // 최대 탐색 제한
        ++cnt;
        const Node current = open_set.top();
        open_set.pop();

        if (current.xyz == goal)
        {
           // std::cout << "도착\n";
            return reconstruct_path(came_from, current.xyz);
        }

        int current_distance = mdist(current.xyz, goal);
        if (current_distance < closest_distance) {
            closest_node = current;
            closest_distance = current_distance;
        }

        for (const auto& dir : g_directions)
        {
            const Node next_node = current + dir;

            // 배열 경계 검사
            if (next_node.xyz.x < 0 || next_node.xyz.y < 0 || next_node.xyz.z < 0) continue;
            if (next_node.xyz.x >= MCTilemap::MAP_WIDTH || next_node.xyz.y >= MCTilemap::MAP_HEIGHT || next_node.xyz.z >= MCTilemap::MAP_WIDTH) continue;
            if (mc_tile_map->GetTileFlag(next_node.xyz.x, next_node.xyz.y, next_node.xyz.z) != 0) continue;
           // if (next_node.xyz.y > 0 && mc_tile_map->GetTile(next_node.xyz - glm::ivec3{ 0,1,0 }) == 0)continue;
           
            const int g = g_score[current] + (glm::abs(dir.x) + glm::abs(dir.y) + glm::abs(dir.z));

            if (!g_score.contains(next_node) || g < g_score[next_node])
            {
                came_from[next_node.xyz] = current.xyz;
                g_score[next_node] = g;
                const int h = mdist(next_node.xyz, goal);
                open_set.emplace(next_node.xyz, g, h);
            }
        }
    }

    // 목표에 도달하지 못했을 때 가장 가까운 경로 반환
    //std::cout << "못찾음" << std::endl;
    return reconstruct_path(came_from, closest_node.xyz);
}
