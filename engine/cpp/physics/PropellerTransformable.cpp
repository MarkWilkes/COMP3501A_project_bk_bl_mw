#include "PropellerTransformable.h"

using namespace DirectX;

PropellerTransformable::PropellerTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation)
	: Transformable(scale, position, orientation)
{

}

PropellerTransformable::~PropellerTransformable()
{
}

HRESULT PropellerTransformable::transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval)
{
	XMFLOAT4 oriRot;
	XMStoreFloat4(&oriRot, XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.2f));
	XMVECTOR oriVec = XMQuaternionMultiply(XMLoadFloat4(&oriRot), XMLoadFloat4(&m_orientation));
	XMStoreFloat4(&m_orientation, oriVec);

	return ERROR_SUCCESS;
}
