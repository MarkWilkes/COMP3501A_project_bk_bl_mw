#include "GridCubeTextured.h"
#include "fileUtil.h"

using namespace DirectX;
using std::wstring;

#define GRIDCUBETEXTURED_CONFIGIO_CLASS FlatAtomicConfigIO

// Logging output and configuration input filename
#define GRIDCUBETEXTURED_FILE_NAME L"AlbedoTextureTestState.txt"

#define GRIDCUBETEXTURED_NQUADBONES 4

GridCubeTextured::GridCubeTextured(float lengthX, float lengthY, float lengthZ, XMFLOAT4 * pColors)
 : IGeometry(), LogUser(true, GRIDCUBETEXTURED_START_MSG_PREFIX),
 m_rootTransform(0)
{
	m_rootTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
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
	
	if (m_quadBones != 0) {
		delete m_quadBones;
		m_quadBones = 0;
	}

	if (m_quadBones_shared != 0) {
		delete m_quadBones_shared;
		m_quadBones_shared = 0;
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

	XMFLOAT3 cornerPositions[GRIDCUBETEXTURED_NQUADBONES];
	cornerPositions[0] = XMFLOAT3(1.0f, 1.0f, 0.0f);
	cornerPositions[1] = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	cornerPositions[2] = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	cornerPositions[3] = XMFLOAT3(1.0f, -1.0f, 0.0f);

	XMFLOAT3 cornerScales[GRIDCUBETEXTURED_NQUADBONES];
	cornerScales[0] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cornerScales[1] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cornerScales[2] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cornerScales[3] = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMFLOAT4 cornerOrientations[GRIDCUBETEXTURED_NQUADBONES];
	cornerOrientations[0] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	cornerOrientations[1] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	cornerOrientations[2] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	cornerOrientations[3] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_quadBones = new std::vector<Transformable*>();
	for (size_t i = 0; i < GRIDCUBETEXTURED_NQUADBONES; ++i) {
		Transformable* newTransform = new Transformable(cornerScales[i], cornerPositions[i], cornerOrientations[i]);
		newTransform->setParent(m_rootTransform);
		m_quadBones->push_back(newTransform);
	}

	m_quadBones_shared = new std::vector<Transformable*>();
	for (size_t i = 0; i < GRIDCUBETEXTURED_NQUADBONES; ++i) {
		m_quadBones_shared->push_back((*m_quadBones)[i]);
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

		result = m_gridQuadList[i]->initialize(d3dDevice, m_quadBones_shared, 0);
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
	return m_quadBones;
}