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
		ObjectModel(IGeometry* geometry, bool npcMover);
		virtual ~ObjectModel(void);

		virtual XMFLOAT3 getBoundingOrigin();
		virtual float getBoundingRadius();

		virtual HRESULT updateContainedTransforms(const DWORD currentTime, const DWORD updateTimeInterval);
		virtual HRESULT addTransformable(Transformable*);
		virtual HRESULT draw(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera);

		virtual XMFLOAT3 getGoalPos();
		virtual XMFLOAT3 getMovePos();
		virtual HRESULT updateGoalPos(XMFLOAT3 newGoal);
		virtual HRESULT updateMoveToPos(XMFLOAT3 newPos);
		virtual bool hasGoal();
		virtual float getMoveDist();
		
		const bool isMover;

	protected:
		IGeometry* model;
		vector<Transformable*>* tForms;

		bool seeking;
		XMFLOAT3 goalPoint;
		XMFLOAT3 moveToPoint;
};