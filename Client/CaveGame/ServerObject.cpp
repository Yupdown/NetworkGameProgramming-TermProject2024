#include "ServerObject.h"


ServerObject::~ServerObject() noexcept
{
}

void ServerObject::SyncMovement(const s2c_MOVE_OBJECT& pkt)
{
	// Set the position, rotation, velocity, and acceleration of the object
	// This assumes that the server manager was calling the individual object by its ID

	glm::vec3 vPosition = glm::vec3(pkt.position_x, pkt.position_y, pkt.position_z);
	glm::quat qRotation = glm::angleAxis(glm::radians(pkt.rotation_y), glm::vec3(0, 1, 0));
	glm::vec3 vVelocity = glm::vec3(pkt.velocity_x, pkt.velocity_y, pkt.velocity_z);
	glm::vec3 vAcceleration = glm::vec3(pkt.acceleration_x, pkt.acceleration_y, pkt.acceleration_z);

	SetPosition(vPosition);
	SetRotation(qRotation);
	SetVelocity(vVelocity);
	SetAcceleration(vAcceleration);
}
