#include "../camera/CameraTransformable.h"

using namespace DirectX;

CameraTransformable::CameraTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation)
	: Transformable(scale, position, orientation), followTransform(0)
{}

CameraTransformable::~CameraTransformable()
{}

void CameraTransformable::SetFollowTransform(Transformable* otherFollowTransform)
{
	followTransform = otherFollowTransform;
}

void CameraTransformable::setFirstPersonCam()
{
	Move(-m_position.z);
	Move(CAMERA_FIRST_PERSON_DIST);
	m_orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

void CameraTransformable::setThirdPersonCam()
{
	Move(-m_position.z);
	Move(CAMERA_THIRD_PERSON_DIST);
	Crane(CAMERA_THIRD_PERSON_HEIGHT);
	
	m_orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}
