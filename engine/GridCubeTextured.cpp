#include "GridCubeTextured.h"
#include "fileUtil.h"

using namespace DirectX;
using std::wstring;

#define GRIDCUBETEXTURED_CONFIGIO_CLASS FlatAtomicConfigIO

// Logging output and configuration input filename
#define GRIDCUBETEXTURED_FILE_NAME L"AlbedoTextureTestState.txt"

#define GRIDCUBETEXTURED_NQUADBONES 4

GridCubeTextured::GridCubeTextured(Transformable* transform)
 : IGeometry(), LogUser(true, GRIDCUBETEXTURED_START_MSG_PREFIX),
 m_rootTransform(transform)
{
}


GridCubeTextured::~GridCubeTextured()
{
	if (m_gridQuadList)
	{
		for (int i = 0; i < 6; ++i) {
			delete m_gridQuadList[i];
			m_gridQuadList[i] = 0;
		}
	}
	
	for (int i = 0; i < 6; ++i) {
		if (m_quadBones[i] != 0) {
			delete m_quadBones[i];
			m_quadBones[i] = 0;
		}

		if (m_quadBones_shared[i] != 0) {
			delete m_quadBones_shared[i];
			m_quadBones_shared[i] = 0;
		}
	}
}

HRESULT GridCubeTextured::initialize(ID3D11Device* d3dDevice)
{
	/*
	0 - top face
	1 - bottom face
	2 - left face
	3 - right face
	4 - front face
	5 - back face
	*/

	XMFLOAT3 cornerPositions[6][GRIDCUBETEXTURED_NQUADBONES];
	XMFLOAT3 cornerScales[6][GRIDCUBETEXTURED_NQUADBONES];
	XMFLOAT4 cornerOrientations[6][GRIDCUBETEXTURED_NQUADBONES];
	Transformable* angledTransforms[6];

	float xs = m_rootTransform->getScale().x;
	float ys = m_rootTransform->getScale().y;
	float zs = m_rootTransform->getScale().z;

	// top face
	XMVECTOR oriVec = XMQuaternionRotationRollPitchYaw(XM_PI / 2.0f, 0.0f, 0.0f);
	XMFLOAT4 newOri = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4(&newOri, oriVec);
	angledTransforms[0] = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), 
											m_rootTransform->getPosition(), 
											newOri);
	angledTransforms[0]->setParent(m_rootTransform);

	cornerPositions[0][0] = XMFLOAT3(1.0f * xs, 1.0f * zs, -1.0f * ys);
	cornerPositions[0][1] = XMFLOAT3(-1.0f * xs, 1.0f * zs, -1.0f * ys);
	cornerPositions[0][2] = XMFLOAT3(-1.0f * xs, -1.0f * zs, -1.0f * ys);
	cornerPositions[0][3] = XMFLOAT3(1.0f * xs, -1.0f * zs, -1.0f * ys);

	// bottom face
	oriVec = XMQuaternionRotationRollPitchYaw(-XM_PI / 2.0f, 0.0f, 0.0f);
	XMStoreFloat4(&newOri, oriVec);
	angledTransforms[1] = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f),
		m_rootTransform->getPosition(),
		newOri);
	angledTransforms[1]->setParent(m_rootTransform);

	cornerPositions[1][0] = XMFLOAT3(1.0f * xs, 1.0f * zs, -1.0f * ys);
	cornerPositions[1][1] = XMFLOAT3(-1.0f * xs, 1.0f * zs, -1.0f * ys);
	cornerPositions[1][2] = XMFLOAT3(-1.0f * xs, -1.0f * zs, -1.0f * ys);
	cornerPositions[1][3] = XMFLOAT3(1.0f * xs, -1.0f * zs, -1.0f * ys);

	// left face
	oriVec = XMQuaternionRotationRollPitchYaw(0.0f, -XM_PI / 2.0f, 0.0f);
	XMStoreFloat4(&newOri, oriVec);
	angledTransforms[2] = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f),
		m_rootTransform->getPosition(),
		newOri);
	angledTransforms[2]->setParent(m_rootTransform);

	cornerPositions[2][0] = XMFLOAT3(1.0f * zs, 1.0f * ys, -1.0f * xs);
	cornerPositions[2][1] = XMFLOAT3(-1.0f * zs, 1.0f * ys, -1.0f * xs);
	cornerPositions[2][2] = XMFLOAT3(-1.0f * zs, -1.0f * ys, -1.0f * xs);
	cornerPositions[2][3] = XMFLOAT3(1.0f * zs, -1.0f * ys, -1.0f * xs);

	// right face
	oriVec = XMQuaternionRotationRollPitchYaw(0.0f, XM_PI / 2.0f, 0.0f);
	XMStoreFloat4(&newOri, oriVec);
	angledTransforms[3] = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f),
		m_rootTransform->getPosition(),
		newOri);
	angledTransforms[3]->setParent(m_rootTransform);

	cornerPositions[3][0] = XMFLOAT3(1.0f * zs, 1.0f * ys, -1.0f * xs);
	cornerPositions[3][1] = XMFLOAT3(-1.0f * zs, 1.0f * ys, -1.0f * xs);
	cornerPositions[3][2] = XMFLOAT3(-1.0f * zs, -1.0f * ys, -1.0f * xs);
	cornerPositions[3][3] = XMFLOAT3(1.0f * zs, -1.0f * ys, -1.0f * xs);

	// front face
	oriVec = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	XMStoreFloat4(&newOri, oriVec);
	angledTransforms[4] = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f),
		m_rootTransform->getPosition(),
		newOri);
	angledTransforms[4]->setParent(m_rootTransform);

	cornerPositions[4][0] = XMFLOAT3(1.0f * xs, 1.0f * ys, -1.0f * zs);
	cornerPositions[4][1] = XMFLOAT3(-1.0f * xs, 1.0f * ys, -1.0f * zs);
	cornerPositions[4][2] = XMFLOAT3(-1.0f * xs, -1.0f * ys, -1.0f * zs);
	cornerPositions[4][3] = XMFLOAT3(1.0f * xs, -1.0f * ys, -1.0f * zs);

	// back face
	oriVec = XMQuaternionRotationRollPitchYaw(0.0f, XM_PI, 0.0f);
	XMStoreFloat4(&newOri, oriVec);
	angledTransforms[5] = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f),
		m_rootTransform->getPosition(),
		newOri);
	angledTransforms[5]->setParent(m_rootTransform);

	cornerPositions[5][0] = XMFLOAT3(1.0f * xs, 1.0f * ys, -1.0f * zs);
	cornerPositions[5][1] = XMFLOAT3(-1.0f * xs, 1.0f * ys, -1.0f * zs);
	cornerPositions[5][2] = XMFLOAT3(-1.0f * xs, -1.0f * ys, -1.0f * zs);
	cornerPositions[5][3] = XMFLOAT3(1.0f * xs, -1.0f * ys, -1.0f * zs);


	oriVec = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	XMStoreFloat4(&newOri, oriVec);

	for (int i = 0; i < 6; ++i) {

		for (int k = 0; k < GRIDCUBETEXTURED_NQUADBONES; ++k) {
			cornerScales[i][k] = XMFLOAT3(1.0f, 1.0f, 1.0f);
			cornerOrientations[i][k] = newOri;
		}
	}

	m_allQuadBones = new std::vector<Transformable*>();
	for (int i = 0; i < 6; ++i) {
		m_quadBones[i] = new std::vector<Transformable*>();
		//m_allQuadBones->push_back(angledTransforms[i]);
		for (size_t k = 0; k < GRIDCUBETEXTURED_NQUADBONES; ++k) {
			Transformable* newTransform = new Transformable(cornerScales[i][k], cornerPositions[i][k], cornerOrientations[i][k]);
			newTransform->setParent(angledTransforms[i]);
			m_quadBones[i]->push_back(newTransform);
			m_allQuadBones->push_back(newTransform);
		}

		m_quadBones_shared[i] = new std::vector<Transformable*>();
		for (size_t k = 0; k < GRIDCUBETEXTURED_NQUADBONES; ++k) {
			m_quadBones_shared[i]->push_back(m_quadBones[i]->at(k));
		}
	}

	// Vertex skinning geometry
	// ------------------------

	HRESULT result = ERROR_SUCCESS;
	wstring errorStr;

	// Set up configuration data
	Config config;

	FlatAtomicConfigIO configIO;
	wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, GRIDCUBETEXTURED_FILE_NAME);
	result = configIO.setLogger(true, logFilename, false, false);
	if (FAILED(result)) {
		logMessage(L"Failed to redirect logging output of the FlatAtomicConfigIO object.");
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	wstring configFilename;
	fileUtil::combineAsPath(configFilename, ENGINE_DEFAULT_CONFIG_PATH_TEST, GRIDCUBETEXTURED_FILE_NAME);
	result = configIO.read(configFilename, config);
	if (FAILED(result)) {
		logMessage(L"Failed to read the configuration file: " + configFilename);
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	/* The logging output of this object is assumed to be
	redirected by the configuration data.
	*/
	for (int i = 0; i < 6; ++i) {
		m_gridQuadList[i] = new GridQuadTextured(true, L"GridQuadTextured Object: ", &config);

		result = m_gridQuadList[i]->initialize(d3dDevice, m_quadBones_shared[i], 0);
		if (FAILED(result)) {
			logMessage(L"Failed to initialize GridCubeTextured object.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			return result;
		}
	}
	return result;
}

HRESULT GridCubeTextured::drawUsingAppropriateRenderer(
	ID3D11DeviceContext* const context,
	GeometryRendererManager& manager,
	const Camera* const camera
	)
{
	for (int i = 0; i < 6; ++i) {
		if (FAILED(m_gridQuadList[i]->drawUsingAppropriateRenderer(context,
			manager,
			camera))) {
			logMessage(L"Failed to draw GridCubeTextured object.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

HRESULT GridCubeTextured::setTransformables(const std::vector<Transformable*>* const transforms)
{
	for (int i = 0; i < 6; ++i) {
		if (FAILED(m_gridQuadList[i]->setTransformables(transforms))) {
			logMessage(L"Failed to set GridCubeTextured object transforms.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

void GridCubeTextured::setParentTransformable(Transformable* theParent)
{
	m_rootTransform->setParent(theParent);
}

XMFLOAT3 GridCubeTextured::getPosition()
{
	return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

float GridCubeTextured::getRadius()
{
	return 2.0f;
}

std::vector<Transformable*>* GridCubeTextured::getTransformables()
{
	return m_allQuadBones;
}