/*
	This is supposed to be for the OctTree as a general object for storage of the game geometry
*/

#pragma once

#include <vector>

using namespace std;

#include "Transformable.h"
#include "IGeometry.h"
#include "LogUser.h"

// Logging message prefix
#define OBJECTMODEL_START_MSG_PREFIX L"ObjectModel "

class ObjectModel : public LogUser {
	public:
		ObjectModel(IGeometry* geometry);
		virtual ~ObjectModel(void);

		virtual XMFLOAT3 getBoundingOrigin();
		virtual float getBoundingRadius();

		virtual HRESULT updateContainedTransforms(const DWORD currentTime, const DWORD updateTimeInterval);
		virtual HRESULT addTransformable(Transformable*);
		virtual HRESULT draw(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera);

	protected:
		IGeometry* model;
		vector<Transformable *>* tForms;
};