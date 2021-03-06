/*
Phase1TestState.cpp
-------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 22, 2014

Primary basis: None

Description
-Implementation of the Phase1TestState class
*/

#include "Phase1TestState.h"
#include "fileUtil.h"
#include "engineGlobals.h"

using namespace DirectX;

Phase1TestState::Phase1TestState(void) :
LogUser(true, PHASE1TESTSTATE_START_MSG_PREFIX),
m_camera(0), /*m_cubeModel(0),*/ m_sphereModel(0), /*m_cubeModelTransformable(0),*/ m_sphereModelTransformable(0)
{
	// Set up a custom logging output stream
	std::wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, L"Phase1TestState.txt");
	if (FAILED(setLogger(true, logFilename, false, false))) {
		logMessage(L"Failed to redirect logging output to: " + logFilename);
	}
}

Phase1TestState::~Phase1TestState(void) {
	if (m_camera != 0) {
		delete m_camera;
		m_camera = 0;
	}

	if (m_sphereModel != 0) {
		delete m_sphereModel;
		m_sphereModel = 0;
	}

	if( m_sphereModelTransformable != 0 ) {
		delete m_sphereModelTransformable;
		m_sphereModelTransformable = 0;
	}
}

HRESULT Phase1TestState::initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) {

	// Initialize the camera
	m_camera = new Camera(screenWidth, screenHeight);

	m_sphereModelTransformable = new Transformable(XMFLOAT3(1.0f, 2.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_sphereModel = new SphereModel(
		m_sphereModelTransformable,
		2.0f,
		0
		);

	if (FAILED(m_sphereModel->initialize(device))) {
		logMessage(L"Failed to initialize SphereModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
	/*
	m_cubeModelTransformable = new Transformable();

	m_cubeModel = new CubeModel(
	m_cubeModelTransformable, // Now owned by m_cubeModelTransformable
	1.0f, // X length
	2.0f, // Y length
	3.0f, // Z length
	0 // Use default colours
	);

	if( FAILED(m_cubeModel->initialize(device)) ) {
	logMessage(L"Failed to initialize CubeModel object.");
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
	*/
}

HRESULT Phase1TestState::next(State*& nextState) {
	nextState = this;
	return ERROR_SUCCESS;
}

HRESULT Phase1TestState::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	if (FAILED(m_sphereModel->drawUsingAppropriateRenderer(context, manager, m_camera))) {
		logMessage(L"Failed to render SphereModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		return ERROR_SUCCESS;
	}

	//m_sphereModel->drawUsingAppropriateRenderer(context, manager, m_camera);
	/*
	if( FAILED(m_cubeModel->drawUsingAppropriateRenderer(context, manager, m_camera)) ) {
	logMessage(L"Failed to render CubeModel object.");
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
	return ERROR_SUCCESS;
	}
	*/
}

HRESULT Phase1TestState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	m_sphereModelTransformable->update(currentTime, updateTimeInterval);
	if (FAILED(m_sphereModelTransformable->update(currentTime, updateTimeInterval))) {
		logMessage(L"Call to Transformable update() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {

		// Cycle the transparency of the cube model
		float endTime = static_cast<float>(currentTime + updateTimeInterval);
		float nPeriods = endTime / PHASE1TESTSTATE_PERIOD;
		float nRadians = nPeriods * XM_2PI;
		float sine = XMScalarSin(nRadians);
		//float transparencyMultiplier = (sine + 1.0f) * 0.5f;

		//m_sphereModel->setTransparencyBlendFactor(transparencyMultiplier);
	}
	return ERROR_SUCCESS;
}

HRESULT Phase1TestState::poll(Keyboard& input, Mouse& mouse) {
	if (FAILED(m_camera->poll(input, mouse))) {
		logMessage(L"Call to CineCameraClass poll() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		return ERROR_SUCCESS;
	}
}