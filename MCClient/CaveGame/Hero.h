#pragma once
#include "pch.h"
#include "Player.h"

class MCTilemap;

class Hero :
    public Player
{
private:
    void InputMove()noexcept;
    void UpdateTileManipulation()noexcept;
    void SendMyMoveData()noexcept;
    void MoveByView(const glm::vec3& vDelta);
public:
    Hero(std::shared_ptr<MCTilemap> pTilemap)noexcept;
    void Update()override;
    void OnDamaged();
private:
    float m_fAccTime = 0.f;
};
