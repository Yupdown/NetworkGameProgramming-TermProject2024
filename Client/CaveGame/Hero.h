#pragma once
#include "pch.h"
#include "Player.h"
#include "MCItemStack.h"

class MCTilemap;

class Hero :
    public Player
{
    static constexpr const float G_SEND_INTERVAL = 0.01f;
private:
    void InputMove()noexcept;
    void UpdateTileManipulation()noexcept;
    void MoveByView(const glm::vec3& vDelta);

    shared_ptr<GameObj> CreateCursorBlockObj() const;
    shared_ptr<GameObj> CreateParticlePrefab() const;

public:
    Hero(std::shared_ptr<MCTilemap> pTilemap)noexcept;
    void Start()override;
    void Update()override;
    void OnObjectDamaged(int value) override;
    void OnObjectDead() override;
    void SendMyMoveData()const noexcept;
    void InitCamDirection()noexcept;
    void UpdatePlayerCamFpsMode()noexcept;
    void UpdateCameraTransform(const shared_ptr<Transform>& pCameraTransfrom) noexcept;
    glm::vec3 GetCameraDirection() const;
    void SetPlayerControl(bool bControl)noexcept;
    void UpdatePlayerInventoryUI() noexcept;
    void Fire();

private:
    float m_fAccTime = 0.f;
    bool m_bIsPlayerControl = true;

    shared_ptr<GameObj> m_cameraAnchor;
    shared_ptr<GameObj> m_cameraObj;
    shared_ptr<GameObj> m_cursorBlockObj;

    std::function<void(void)> m_fpChangeCamMode[3];

    float m_fCamSensivity = 0.1f;

    glm::vec3 m_cameraAngleAxis = glm::zero<glm::vec3>();
    glm::vec3 m_cameraAngleAxisSmooth = glm::zero<glm::vec3>();

    shared_ptr<Camera> m_pCamera;
    shared_ptr<Transform> m_pCacheMyTransformCamera;
    ushort m_curCamMode = 0;

	std::array<MCItemStack, 9> m_inventory;
};
