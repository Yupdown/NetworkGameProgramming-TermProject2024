#pragma once
#include "pch.h"
#include "Player.h"

class MCTilemap;

class Hero :
    public Player
{
    static constexpr const float G_SEND_INTERVAL = 0.01f;
private:
    void InputMove()noexcept;
    void UpdateTileManipulation()noexcept;
    void MoveByView(const glm::vec3& vDelta);
public:
    Hero(std::shared_ptr<MCTilemap> pTilemap)noexcept;
    void Update()override;
    void OnDamaged();
    void SendMyMoveData()const noexcept;
private:
    float m_fAccTime = 0.f;
};
