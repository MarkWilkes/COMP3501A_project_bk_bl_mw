#pragma once

#include "Transformable.h"

class PropellerTransformable :
	public Transformable
{
public:
	PropellerTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);
	virtual ~PropellerTransformable();

	virtual HRESULT transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval) override;
};

