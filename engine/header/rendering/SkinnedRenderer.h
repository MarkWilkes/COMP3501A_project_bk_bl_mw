/*
SkinnedRenderer.h
---------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 8, 2014

Primary basis: "SimpleColorRenderer.h", with ideas from
Chapter 8 (which discussed vertex skinning) of
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -A class for rendering indexed vertex geometry
   containing moveable bones, and whose vertices contain colour data,
   rather than texture coordinates.
  -Lighting can be enabled or disabled (during initialization only)
   using configuration data
*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <string>
#include "IGeometryRenderer.h"
#include "ConfigUser.h"
#include "FlatAtomicConfigIO.h"
#include "SkinnedColorGeometry.h"
#include "Shader.h"

// Type of loader to use for configuration data
#define SKINNEDRENDERER_CONFIGIO_CLASS FlatAtomicConfigIO

// Type of material data handled by this class
#define SKINNEDRENDERER_MATERIAL_STRUCT SkinnedColorGeometry::Material

// Default log message prefix used before more information is available
#define SKINNEDRENDERER_START_MSG_PREFIX L"SkinnedRenderer "

/* The following definitions are:
  -Key parameters used to retrieve configuration data
  -Default values used in the absence of configuration data
   or constructor/function arguments (where necessary)
*/
#define SKINNEDRENDERER_SCOPE L"SkinnedRenderer"
#define SKINNEDRENDERER_LOGUSER_SCOPE L"SkinnedRenderer_LogUser"
#define SKINNEDRENDERER_CONFIGUSER_SCOPE L"SkinnedRenderer_ConfigUser"

#define SKINNEDRENDERER_LIGHT_FLAG_FIELD L"enableLighting"

// Lighting parameters
#define SKINNEDRENDERER_LIGHT_POSITION_DEFAULT DirectX::XMFLOAT4(-1000.0f, 0.0f, 0.0f, 1.0f)
#define SKINNEDRENDERER_LIGHT_POSITION_FIELD L"lightPosition"
#define SKINNEDRENDERER_LIGHT_COLOR_DEFAULT DirectX::XMFLOAT4(1.0, 1.0f, 1.0f, 1.0f)
#define SKINNEDRENDERER_LIGHT_COLOR_FIELD L"lightColor"
#define SKINNEDRENDERER_LIGHT_AMBIENT_WEIGHT_DEFAULT 1.0f
#define SKINNEDRENDERER_LIGHT_AMBIENT_WEIGHT_FIELD L"lightAmbientWeight"
#define SKINNEDRENDERER_LIGHT_DIFFUSE_WEIGHT_DEFAULT 1.0f
#define SKINNEDRENDERER_LIGHT_DIFFUSE_WEIGHT_FIELD L"lightDiffuseWeight"
#define SKINNEDRENDERER_LIGHT_SPECULAR_WEIGHT_DEFAULT 1.0f
#define SKINNEDRENDERER_LIGHT_SPECULAR_WEIGHT_FIELD L"lightSpecularWeight"

class SkinnedRenderer : public IGeometryRenderer, public ConfigUser {
private:
	struct CameraBufferType {
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 cameraPosition;
	};

	struct MaterialBufferType {
		DirectX::XMFLOAT4 ambientAlbedo;
		DirectX::XMFLOAT4 diffuseAlbedo;
		DirectX::XMFLOAT4 specularAlbedo;
		float specularPower;
		float blendAmount;
		DirectX::XMFLOAT2 padding;
	};

public:
	// Point light source
	struct Light {
		DirectX::XMFLOAT4 lightPosition;
		DirectX::XMFLOAT4 lightColor;
		float lightAmbientWeight;
		float lightDiffuseWeight;
		float lightSpecularWeight;
		float padding;
	};

public:
	/* Configuration data is needed to know which
	   shader files to compile.

	   The 'filename' and 'path' parameters describe
	   the location of the configuration file.
	   (Documented in ConfigUser.h)
	 */
	SkinnedRenderer(
		const std::wstring filename,
		const std::wstring path = L""
		);

	virtual ~SkinnedRenderer(void);

	virtual HRESULT initialize(ID3D11Device* const device) override;

	virtual HRESULT render(ID3D11DeviceContext* const context, const IGeometry& geometry, const Camera* const camera) override;

	// Helper functions
protected:

	/* Retrieves configuration data
	   If 'configUserScope' is null, it defaults to SKINNEDRENDERER_CONFIGUSER_SCOPE.
	   If 'logUserScope' is null, it defaults to SKINNEDRENDERER_LOGUSER_SCOPE.
	*/
	virtual HRESULT configure(const std::wstring& scope = SKINNEDRENDERER_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0);

	/* Creates the pipeline shaders
	   Also responsible for calling configureRendering()
	   and createInputLayout()
	 */
	virtual HRESULT createShaders(ID3D11Device* const);

	/* Creates the vertex input layout */
	virtual HRESULT createInputLayout(ID3D11Device* const, ID3D10Blob* const vertexShaderBuffer);

	/* Creates lighting-independent constant buffers */
	virtual HRESULT createNoLightConstantBuffers(ID3D11Device* const);

	/* Creates lighting-dependent constant buffers */
	virtual HRESULT createLightConstantBuffers(ID3D11Device* const);

	/* Retrieves and logs shader compilation error messages.
	   Also releases the input argument.
	 */
	void outputShaderErrorMessage(ID3D10Blob* const);

	virtual HRESULT setShaderParameters(
		ID3D11DeviceContext* const,
		const DirectX::XMFLOAT4X4 viewMatrix,
		const DirectX::XMFLOAT4X4 projectionMatrix,
		const DirectX::XMFLOAT4 cameraPosition,
		const float blendFactor,
		const SKINNEDRENDERER_MATERIAL_STRUCT* material);

	/* Sets light-independent pipeline state */
	virtual HRESULT setNoLightShaderParameters(
		ID3D11DeviceContext* const,
		const DirectX::XMFLOAT4X4 viewMatrix,
		const DirectX::XMFLOAT4X4 projectionMatrix,
		const DirectX::XMFLOAT4 cameraPosition,
		const float blendFactor);

	/* Sets light-dependent pipeline state */
	virtual HRESULT setLightShaderParameters(
		ID3D11DeviceContext* const,
		const SKINNEDRENDERER_MATERIAL_STRUCT* material,
		const float blendFactor);

	void renderShader(ID3D11DeviceContext* const, const size_t);

	// Data members
private:
	Shader* m_vertexShader;
	Shader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_materialBuffer;
	ID3D11Buffer* m_lightBuffer;

	// Is the renderer configured to use lighting?
	bool m_lighting;

	/* To be initialized from configuration data
	   Currently, the light gets sent to the pipeline each
	   rendering pass, even though there is no
	   mechanism for changing the light data between frames.
	 */
	Light* m_light;

	// Currently not implemented - will cause linker errors if called
private:
	SkinnedRenderer(const SkinnedRenderer& other);
	SkinnedRenderer& operator=(const SkinnedRenderer& other);
};