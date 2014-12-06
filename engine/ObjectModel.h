/*
	This is supposed to be for the OctTree as a general object for storage of the game geometry
*/

#pragma once

#include <vector>

using namespace std;

#include "Transformable.h"
#include "IGeometry.h"
#include "LogUser.h"

enum class ObjectType{EnemyShip, GalleonShip, MineShip, Other};

// Logging message prefix
#define OBJECTMODEL_START_MSG_PREFIX L"ObjectModel "

class ObjectModel : public LogUser {
	public:
		ObjectModel(IGeometry* geometry, ObjectType type);
		virtual ~ObjectModel(void);

		virtual XMFLOAT3 getBoundingOrigin();
		virtual float getBoundingRadius();

		virtual HRESULT updateContainedTransforms(const DWORD currentTime, const DWORD updateTimeInterval);
		virtual HRESULT addTransformable(Transformable*);
		virtual HRESULT draw(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera);

		virtual XMFLOAT3 getGoalPos();
		virtual HRESULT updateGoalPos(XMFLOAT3 newGoal);
		virtual HRESULT updateMoveToPos(XMFLOAT3 newPos);
		virtual bool hasGoal();
		virtual float getMoveDist();

		virtual int getAgentNum();
		virtual void setAgentNum(int aNum);

		const ObjectType type;
	protected:
		IGeometry* model;
		vector<Transformable*>* tForms;

		bool seeking;
		XMFLOAT3 goalPoint;
		XMFLOAT3 moveToPoint;

		int agentNum;
};