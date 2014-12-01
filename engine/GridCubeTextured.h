/*
GridCubeTextured.h
--------------------

Created for: COMP3501A Assignment 5
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 28, 2014

Primary basis: GridQuadTextured.h

Description
-A cube made of GridQuadTexture objects.
*/

#pragma once

#include <DirectXMath.h>
#include <vector>
#include "Transformable.h"
#include "GridQuadTextured.h"
#include "LogUser.h"
#include "../../oct_tree.h"
#include "../../ObjectModel.h"
#include "IGeometry.h"
#include <string>

// Default log message prefix used before more information is available
#define GRIDCUBETEXTURED_START_MSG_PREFIX L"GridCubeTextured "

/* The following definitions are:
-Key parameters used to retrieve configuration data
-Default values used in the absence of configuration data
or constructor/function arguments (where necessary)
*/

#define GRIDCUBETEXTURED_SCOPE L"GridCubeTextured"

/* LogUser and ConfigUser configuration parameters
Refer to LogUser.h and ConfigUser.h
*/
#define GRIDCUBETEXTURED_LOGUSER_SCOPE		L"GridCubeTextured_LogUser"
#define GRIDCUBETEXTURED_CONFIGUSER_SCOPE	L"GridCubeTextured_ConfigUser"

class GridCubeTextured : public IGeometry, public LogUser
{
	// Initialization and destruction
public:

	GridCubeTextured(XMFLOAT3 scale, XMFLOAT3 position, XMFLOAT4 orientation, XMFLOAT4 * pColors);
	virtual ~GridCubeTextured(void);

	HRESULT spawn(Octtree*);
	HRESULT initialize(ID3D11Device* d3dDevice);

	virtual HRESULT drawUsingAppropriateRenderer(
		ID3D11DeviceContext* const context,
		GeometryRendererManager& manager,
		const Camera* const camera
		) override;

	virtual HRESULT setTransformables(const std::vector<Transformable*>* const transforms) override;

	Transformable* m_rootTransform;
	void setParentTransformable(Transformable* theParent);

	virtual float getRadius() override;
	virtual XMFLOAT3 getPosition() override;
	std::vector<Transformable*>* getTransformables();

private:
	/*
	0 - top face
	1 - bottom face
	2 - left face
	3 - right face
	4 - front face
	5 - back face
	*/
	GridQuadTextured* m_gridQuadList[6];

	std::vector<Transformable*>* m_quadBones[6];
	std::vector<Transformable*>* m_quadBones_shared[6];
	std::vector<Transformable*>* m_allQuadBones;

	GridCubeTextured(const GridCubeTextured& other);
	GridCubeTextured& operator=(const GridCubeTextured& other);
};