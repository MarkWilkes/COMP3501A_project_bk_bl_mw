/*
GeometryRendererManager.h
-------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 18, 2014

Primary basis: None
Other references: None

Description
  -Stores a set of GeometryRendererManagerRenderer objects, that GeometryRendererManager objects
   can use to render themselves by querying the set for their desired rendering setup.
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <map>
#include <string>
#include "ConfigUser.h"
#include "IGeometryRenderer.h"

// Default log message prefix used before more information is available
#define GEOMETRYRENDERERMANAGER_START_MSG_PREFIX L"GeometryRendererManager "

// Used to obtain configuration data
#define GEOMETRYRENDERERMANAGER_SCOPE	L"GeometryRendererManager"

class IGeometryRenderer;
class IGeometry;
class CineCameraClass;

class GeometryRendererManager : public ConfigUser {

public:
	/* A listing of the classes inheriting from IGeometryRenderer,
	   to be used in place of run-time type information.
	 */
	enum class GeometryRendererType : unsigned int {
		SimpleColorRenderer
	};
	/* When adding new data types to this enumeration, also do the following:
	   -Update the 's_geometryRendererTypeNames' and
	      's_geometryRendererTypesInOrder' static members
	   -Create the new type of renderer in this class's initialize() function
	 */

	/* Outputs the GeometryRendererType constant name that
	   has the same form as the input string (case-sensitive).
	   Returns a failure code if there is no corresponding
	   GeometryRendererType constant.
	*/
	static HRESULT wstringToGeometryRendererType(GeometryRendererType& out, const std::wstring& in);

	/* The inverse of wstringToGeometryRendererType()
	   Outputs the name corresponding to the enum constant

	   The output parameter is modified only if a GeometryRendererType
	   enum constant is found corresponding to the 'in' parameter
	   and there are no internal errors.
	*/
	static HRESULT geometryRendererTypeToWString(std::wstring& out, const GeometryRendererType& in);

private:
	/* Number of constants in the GeometryRendererType enumeration,
	   which also corresponds to the length of the following
	   two arrays
	*/
	static const size_t s_nGeometryRendererTypes;

	// Names of GeometryRendererType constants as string literals, in order of declaration
	static const std::wstring s_geometryRendererTypeNames[];

	// GeometryRendererType constants in order of declaration
	static const GeometryRendererType s_geometryRendererTypesInOrder[];

	// Data members
private:
	std::map<GeometryRendererType, IGeometryRenderer*> m_map;

public:
	/* Constructor parameters are documented in the ConfigUser class
	   header file.
	 */
	template<typename ConfigIOClass> GeometryRendererManager(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	virtual ~GeometryRendererManager(void);

	/* Uses its private configuration data
	   to determine which renderers to create.
	   Attempts to create and initialize all desired renderers,
	   and returns a failure result if any initialization 
	   cannot be attempted or fails.

	   Note: To determine which renderers to create,
	     the object will iterate over the constants of the
		 GeometryRendererType enumeration and check for
		 Boolean values of 'true' under the keys:

		 scope = "GeometryRendererManager"
		 field = [GeometryRendererType constant in string form,
		          produced using geometryRendererTypeToWString()]
	 */
	HRESULT initialize(ID3D11Device* const device);

	/* Render the specified geometry, using
	   the renderer stored under the given GeometryRendererType
	   enumeration constant.

	   Returns a failure result with an error code of
	   ERROR_DATA_NOT_FOUND if there is no renderer stored
	   under the given GeometryRendererType enumeration constant.
	   In this case, this object will log the error.
	*/
	HRESULT render(
		ID3D11DeviceContext* const context,
		const IGeometry& geometry,
		const CineCameraClass* const camera,
		GeometryRendererType rendererType
		);

	// Currently not implemented - will cause linker errors if called
private:
	GeometryRendererManager(const GeometryRendererManager& other);
	GeometryRendererManager& operator=(const GeometryRendererManager& other);
};

template<typename ConfigIOClass> GeometryRendererManager::GeometryRendererManager(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	ConfigUser(true, GEOMETRYRENDERERMANAGER_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_map()
{}