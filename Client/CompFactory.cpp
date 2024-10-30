#include "pch.h"
#include "CompFactory.h"
#include "Component.h"
#include "Transform.h"
#include "MonoBehavior.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "InputHandler.h"
#include "CoRoutineHandler.h"
#include "Animator.h"
#include "Collider.h"
#include "RigidBody.h"

shared_ptr<Component> CompFactory::CreateComponent(COMPONENT_TYPE _eType)
{
	switch (_eType)
	{
	case COMPONENT_TYPE::TRANSFORM:return make_shared<Transform>();
	case COMPONENT_TYPE::MESH_RENDERER:return make_shared<MeshRenderer>();
	case COMPONENT_TYPE::CAMERA:return make_shared<Camera>();
	case COMPONENT_TYPE::LIGHT:return make_shared<Light>();
	case COMPONENT_TYPE::INPUT_HANDLER:return make_shared<InputHandler>();
	case COMPONENT_TYPE::COROUTINE_HANDLER:return make_shared<CoRoutineHandler>();
	case COMPONENT_TYPE::ANIMATOR:return make_shared<Animator>();
	case COMPONENT_TYPE::COLLIDER:return make_shared<Collider>();
	case COMPONENT_TYPE::RIGIDBODY:return make_shared<RigidBody>();
	default:
		return nullptr;
	}
}
