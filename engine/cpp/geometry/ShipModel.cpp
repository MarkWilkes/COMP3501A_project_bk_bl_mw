/*
ShipModel.cpp
--------------------

Created for: COMP3501A Assignment 5
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 31, 2014
Adapted for COMP3501A Project on October 31, 2014

Primary basis: GridQuadTextured.cpp
Secondary basis: A2Cylinder.cpp (COMP3501A Assignment 2 - Bernard Llanos)

Description
-Implementation of the ShipModel class
*/

#include "ShipModel.h"

using namespace DirectX;

ShipModel::ShipModel()
: IGeometry(),
LogUser(true, SHIPMODEL_START_MSG_PREFIX),
m_bones(0), body(0), leftWing(0), rightWing(0), capsule(0), leftPropellerA(0), leftPropellerB(0), rightPropellerA(0), rightPropellerB(0)
{}

ShipModel::~ShipModel(void)
{
	if (body != 0) {
		delete body;
		body = 0;
	}

	if (leftWing != 0) {
		delete leftWing;
		leftWing = 0;
	}

	if (rightWing != 0) {
		delete rightWing;
		rightWing = 0;
	}

	if (capsule != 0) {
		delete capsule;
		capsule = 0;
	}

	if (leftPropellerA != 0) {
		delete leftPropellerA;
		leftPropellerA = 0;
	}

	if (leftPropellerB != 0) {
		delete leftPropellerB;
		leftPropellerB = 0;
	}

	if (rightPropellerA != 0) {
		delete rightPropellerA;
		rightPropellerA = 0;
	}

	if (rightPropellerB != 0) {
		delete rightPropellerB;
		rightPropellerB = 0;
	}
}

HRESULT ShipModel::initialize(ID3D11Device* d3dDevice, vector<Transformable*>* bones)
{
	if (bones == 0){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	if (bones->size() != static_cast<std::vector<Transformable*>::size_type>(8)){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	m_bones = bones;

	//rootTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	body = new CubeModel(m_bones->at(0), 1.0f, 0.75f, 4.0f, 0);
	//Transformable* wingTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f));
	leftWing = new CubeModel(m_bones->at(1), 1.0f, 0.75f, 3.0f, 0);

	//wingTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f));
	rightWing = new CubeModel(m_bones->at(2), 1.0f, 0.75f, 3.0f, 0);

	capsule = new SphereModel(m_bones->at(3), 0.5f, new XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));

	leftPropellerA = new CubeModel(static_cast<PropellerTransformable*>(m_bones->at(4)), 0.15f, 1.25f, 0.25f, 0);
	leftPropellerB = new CubeModel(static_cast<PropellerTransformable*>(m_bones->at(5)), 0.15f, 1.25f, 0.25f, 0);
	rightPropellerA = new CubeModel(static_cast<PropellerTransformable*>(m_bones->at(6)), 0.15f, 1.25f, 0.25f, 0);
	rightPropellerB = new CubeModel(static_cast<PropellerTransformable*>(m_bones->at(7)), 0.15f, 1.25f, 0.25f, 0);

	if (FAILED(body->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftWing->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightWing->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(capsule->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize SphereModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftPropellerA->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftPropellerB->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightPropellerA->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightPropellerB->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT ShipModel::drawUsingAppropriateRenderer(
	ID3D11DeviceContext* const context,
	GeometryRendererManager& manager,
	const Camera* const camera
	)
{
	if (FAILED(body->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftWing->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightWing->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(capsule->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw SphereModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if (FAILED(leftPropellerA->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftPropellerB->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightPropellerA->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightPropellerB->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}



	return ERROR_SUCCESS;
}

HRESULT ShipModel::setTransformables(const std::vector<Transformable*>* const transforms)
{
	if (transforms == 0){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	if (transforms->size() != static_cast<std::vector<Transformable*>::size_type>(8)){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	vector<Transformable*>* tform = new vector<Transformable*>();
	tform->push_back(transforms->at(0));

	if (FAILED(body->setTransformables(tform))) {
		logMessage(L"Failed to set CubeModel body object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(1));

	if (FAILED(leftWing->setTransformables(tform))) {
		logMessage(L"Failed to set CubeModel left wing object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(2));

	if (FAILED(rightWing->setTransformables(tform))) {
		logMessage(L"Failed to set CubeModel right wing object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(3));

	if (FAILED(capsule->setTransformables(tform))) {
		logMessage(L"Failed to set SphereModel right wing object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(4));

	if (FAILED(leftPropellerA->setTransformables(tform))) {
		logMessage(L"Failed to set SphereModel right wing object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(5));

	if (FAILED(leftPropellerB->setTransformables(tform))) {
		logMessage(L"Failed to set SphereModel right wing object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(6));

	if (FAILED(rightPropellerA->setTransformables(tform))) {
		logMessage(L"Failed to set SphereModel right wing object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(7));

	if (FAILED(rightPropellerB->setTransformables(tform))) {
		logMessage(L"Failed to set SphereModel right wing object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	return ERROR_SUCCESS;
}

XMFLOAT3 ShipModel::getPosition()
{
	return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

float ShipModel::getRadius()
{
	return 2.0f;
}