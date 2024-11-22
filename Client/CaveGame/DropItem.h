#pragma once

#include "ServerObject.h"

class MCTilemap;
class MeshRenderer;
class Material;
class MCItem;

class DropItem : public ServerObject
{
public:
	DropItem(shared_ptr<MCTilemap> tilemap, MCItem* item, int stack_size);

	MCItem* GetItem() const;
	int GetStackSize() const;

	void Start() override;
	void Update() override;

private:
	shared_ptr<MCTilemap> m_refTilemap;

	MCItem* m_item;
	int m_stackSize;

	shared_ptr<MeshRenderer> m_renderer;
	shared_ptr<Material> m_material;

	shared_ptr<GameObj> m_rendererObject;

	float m_timer = 0.0f;
};