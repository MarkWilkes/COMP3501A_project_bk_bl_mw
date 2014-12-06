/*
ObjectModel.cpp

The object model is an object that contains the geometry and transformations of all objects
It is used for the storage of these attributes in the OctTree

*/

#include "ObjectModel.h"
#include "FlatAtomicConfigIO.h"
#include <stdio.h>

#define OBJECTMODEL_CONFIGIO_CLASS FlatAtomicConfigIO

// Logging output and configuration input filename
#define OBJECTMODEL_FILE_NAME L"ObjectModel.txt"

ObjectModel::ObjectModel(IGeometry* geometry, ObjectType type) :
LogUser(true, OBJECTMODEL_START_MSG_PREFIX),
model(0), tForms(0), type(type), seeking(type == ObjectType::Other ? false : true), goalPoint(0, 0, 0), moveToPoint(0,0,0)
{
	model = geometry;
	tForms = new vector<Transformable *>();
}

ObjectModel::~ObjectModel(){
	for (size_t i = 0; i < tForms->size(); i++){
		delete (*tForms)[i];
	}

	if (tForms != 0){
		delete tForms;
		tForms = 0;
	}
}


XMFLOAT3 ObjectModel::getBoundingOrigin(){
	XMFLOAT3 pos = model->getPosition();
	
	XMFLOAT3 newPos;

	for (size_t i = 0; i < tForms->size(); i++){
		if (!(tForms->at(i)->hasParent())){
			 newPos = tForms->at(i)->getPosition();
		}
	}

	pos.x += newPos.x;
	pos.y += newPos.y;
	pos.z += newPos.z;
	
	return pos;
}

float ObjectModel::getBoundingRadius(){
	float radi = model->getRadius();
	
	float greatestScale = 0.0f;
	XMFLOAT3 scale;

	for (size_t i = 0; i < tForms->size(); i++){
		if (!(tForms->at(i)->hasParent())){
			scale = tForms->at(i)->getScale();
			greatestScale = scale.x;
			greatestScale < scale.y ? greatestScale = scale.y : greatestScale = greatestScale;
			greatestScale < scale.z ? greatestScale = scale.z : greatestScale = greatestScale;
			//radi * greatestScale;
		}
	}
	return radi * greatestScale;
	
	//turn radi;
}

HRESULT ObjectModel::updateContainedTransforms(const DWORD currentTime, const DWORD updateTimeInterval){
	HRESULT result;
	for (std::vector<Transformable*>::size_type i = 0; i < tForms->size(); i++){
		if (type == ObjectType::MineShip){
			//tForms->at(i)->SpinIfParent(2.0f, 2.0f, 2.0f);
			/*
			XMFLOAT4 newDir = XMFLOAT4(moveToPoint.x - getBoundingOrigin().x, 
				moveToPoint.y - getBoundingOrigin().y,
				moveToPoint.z - getBoundingOrigin().z, 1.0f);
			XMVECTOR oriVec = XMLoadFloat4(&tForms->at(i)->getOrientation());
			XMVECTOR newVec = XMLoadFloat4(&newDir);

			XMVECTOR axis = XMVector3Cross(oriVec, newVec);

			XMVECTOR angle = XMVector3AngleBetweenVectors(oriVec, newVec);
			*/
			if (tForms->at(i)->MoveToPointIfParent(moveToPoint)){
//				float x = 1;
			}
			else{
	//			tForms->at(i)->Spin(2.0f, 2.0f, 2.0f);
			}
		}
		if (type == ObjectType::EnemyShip || type == ObjectType::GalleonShip){
			//spin towards the moveToPoint
			/*
			printf("movetoPoint(%f , %f , %f)\n", moveToPoint.x , moveToPoint.y, moveToPoint.z);

			tForms->at(i)->StrafeIfParent(moveToPoint.x + getBoundingOrigin().x);
			tForms->at(i)->CraneIfParent(moveToPoint.y - getBoundingOrigin().y);
			tForms->at(i)->MoveIfParent(moveToPoint.z + getBoundingOrigin().z);
			*/

			tForms->at(i)->MoveToPointIfParent(moveToPoint);
		}
		result = ((*tForms)[i])->update(currentTime, updateTimeInterval);
		if (FAILED(result)){
			return result;
		}
	}
	return ERROR_SUCCESS;
}

HRESULT ObjectModel::addTransformable(Transformable * newTrans){
	tForms->push_back(newTrans);
	return ERROR_SUCCESS;
}

HRESULT ObjectModel::draw(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera){
	HRESULT result = ERROR_SUCCESS;

	result = model->setTransformables(tForms);
	if (FAILED(result)){
		logMessage(L"Failed to set geometry bones.");
	}

	result = model->drawUsingAppropriateRenderer(context, manager, camera);
	if (FAILED(result)){
		logMessage(L"Failed to render model.");
	}

	return result;
}


XMFLOAT3 ObjectModel::getGoalPos(){
	return goalPoint;
}

HRESULT ObjectModel::updateGoalPos(XMFLOAT3 newGoal){
	goalPoint = newGoal;
	seeking = false;
	return ERROR_SUCCESS;
}

bool ObjectModel::hasGoal(){
	return !seeking;
}

HRESULT ObjectModel::updateMoveToPos(XMFLOAT3 newMovePos){
	moveToPoint = newMovePos;
	seeking = false;
	return ERROR_SUCCESS;
}

float ObjectModel::getMoveDist(){
	return 0.01f;
}

int ObjectModel::getAgentNum(){
	return agentNum;
}

void ObjectModel::setAgentNum(int aNum){
	agentNum = aNum;
}