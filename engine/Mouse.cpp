/*
COMP2501A Winter 2014
Tutorial 7 (A student-made tutorial)
Bernard Llanos
March 5, 2014
*/

/*
Implementation of the Mouse class
*/

#include "Mouse.h"
#include "engineGlobals.h"
#include <string>
#include <sstream>   // for wostringstream
using std::wostringstream;

const int Mouse::nButtons = 5;

const DWORD Mouse::maxMotionSamplingInterval = static_cast<DWORD>(100); // measured milliseconds

Mouse::Mouse(void) :
m_Tracking(false), m_Position(0.0f, 0.0f), m_PastPosition(0.0f, 0.0f), m_ButtonStates(0),
m_Moving(false), m_t(static_cast<DWORD>(0)), m_tPast(static_cast<DWORD>(0)),
m_ScreenDimensions(0.0f, 0.0f)
{
	m_ButtonStates = new bool[nButtons];
	for (int i = 0; i < nButtons; ++i)
	{
			m_ButtonStates[i] = false;
	}
}

int Mouse::Initialize(HWND hwnd)
{
	RECT rcClient;                 // client area rectangle 
	// Retrieve the screen coordinates of the client area, 
	// and convert them into client coordinates.
	// Add one to the right and bottom sides, because the 
	// coordinates retrieved by GetClientRect do not 
	// include the far left and lowermost pixels.
	GetClientRect(hwnd, &rcClient);
	m_ScreenDimensions.x = static_cast<float>(rcClient.right + 1);
	m_ScreenDimensions.y = static_cast<float>(rcClient.bottom + 1);
	return C_OK;
}

Mouse::~Mouse(void)
{
	if (m_ButtonStates)
	{
		delete[] m_ButtonStates;
		m_ButtonStates = 0;
	}
}

LRESULT CALLBACK Mouse::winProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	/* Allow for the default handler to be called by the SystemClass object
	regardless of how the mouse handles the event, because this allows
	the window to be dragged, etc., using the mouse. */
	int result = 1; // Zero would mean that the message was fully handled here

	/* Whenever a mouse button is pressed down, this class captures the mouse,
	which means that the window will receive all mouse input. The mouse
	must be captured in order for the default window procedure to handle
	mouse input as well. The mouse is released when no buttons are down. */
	bool startTracking = false;
	bool stopTracking = false;

	bool positionMustChange = false;

	//HWND hwnd = bwin.getHWND();

	switch (umsg)
	{
	case WM_LBUTTONDOWN:
		m_ButtonStates[Button::LEFT] = true;
		startTracking = true;
		// if (verbose_mouse) writeToDebugConsole(L"Mouse -- Left mouse button down.\n");
		break;

	case WM_LBUTTONUP:
		m_ButtonStates[Button::LEFT] = false;
		stopTracking = true;
		// if (verbose_mouse) writeToDebugConsole(L"Mouse -- Left mouse button up.\n");
		break;

	case WM_MBUTTONDOWN:
		m_ButtonStates[Button::MIDDLE] = true;
		startTracking = true;
		// if (verbose_mouse) writeToDebugConsole(L"Mouse -- Middle mouse button down.\n");
		break;

	case WM_MBUTTONUP:
		m_ButtonStates[Button::MIDDLE] = false;
		stopTracking = true;
		// if (verbose_mouse) writeToDebugConsole(L"Mouse -- Middle mouse button up.\n");
		break;

	case WM_RBUTTONDOWN:
		m_ButtonStates[Button::RIGHT] = true;
		startTracking = true;
		// if (verbose_mouse) writeToDebugConsole(L"Mouse -- Right mouse button down.\n");
		break;

	case WM_RBUTTONUP:
		m_ButtonStates[Button::RIGHT] = false;
		stopTracking = true;
		// if (verbose_mouse) writeToDebugConsole(L"Mouse -- Right mouse button up.\n");
		break;

	case WM_XBUTTONDOWN:
		if (wparam & MK_XBUTTON1)
		{
			m_ButtonStates[Button::X1] = true;
			startTracking = true;
			// if (verbose_mouse) writeToDebugConsole(L"Mouse -- X1 mouse button down.\n");
		}
		else if (wparam & MK_XBUTTON2)
		{
			m_ButtonStates[Button::X2] = true;
			startTracking = true;
			// if (verbose_mouse) writeToDebugConsole(L"Mouse -- X2 mouse button down.\n");
		}
		break;

	case WM_XBUTTONUP:
		if (wparam & MK_XBUTTON1)
		{
			m_ButtonStates[Button::X1] = false;
			stopTracking = true;
			// if (verbose_mouse) writeToDebugConsole(L"Mouse -- X1 mouse button up.\n");
		}
		else if (wparam & MK_XBUTTON2)
		{
			m_ButtonStates[Button::X2] = false;
			stopTracking = true;
			// if (verbose_mouse) writeToDebugConsole(L"Mouse -- X2 mouse button up.\n");
		}
		break;

	case WM_MOUSEMOVE:
		if (m_Tracking)
		{
			m_Moving = true;
			positionMustChange = true;
		}
		break;

	default:
		result = C_BAD_INPUT; // Someone else must process this message
	}

	// Check whether to start or stop tracking the mouse

	if (startTracking)
	{
		// if (verbose_mouse) writeToDebugConsole(L"Mouse -- Starting Tracking\n");
		m_Tracking = true;
		SetCapture(hwnd); // Capture mouse input

		/* Constrain the cursor to the window
		   Copied from the "Drawing Lines with the Mouse" example at
		   http://msdn.microsoft.com/en-us/library/windows/desktop/ms645602%28v=vs.85%29.aspx
		   */
		RECT rcClient;                 // client area rectangle 
		POINT ptClientUL;              // client upper left corner 
		POINT ptClientLR;              // client lower right corner
		// Retrieve the screen coordinates of the client area, 
		// and convert them into client coordinates.
		GetClientRect(hwnd, &rcClient);
		ptClientUL.x = rcClient.left;
		ptClientUL.y = rcClient.top;

		// Add one to the right and bottom sides, because the 
		// coordinates retrieved by GetClientRect do not 
		// include the far left and lowermost pixels.
		ptClientLR.x = rcClient.right + 1;
		ptClientLR.y = rcClient.bottom + 1;
		// Convert client-area coordinates to screen coordinates.
		ClientToScreen(hwnd, &ptClientUL);
		ClientToScreen(hwnd, &ptClientLR);

		// Copy the client coordinates of the client area 
		// to the rcClient structure. Confine the mouse cursor 
		// to the client area by passing the rcClient structure 
		// to the ClipCursor function.
		SetRect(&rcClient, ptClientUL.x, ptClientUL.y,
			ptClientLR.x, ptClientLR.y);
		ClipCursor(&rcClient);
	}

	else if (stopTracking)
	{
		// if (verbose_mouse) writeToDebugConsole(L"Mouse -- Stopping Tracking\n");
		m_Tracking = false;
		m_Moving = false;
		ClipCursor(NULL); // Release the cursor clipping
		ReleaseCapture(); // Uncapture the mouse

		// Destroy position and time data
		m_tPast = static_cast<DWORD>(0); // Suspend timer
		m_t = static_cast<DWORD>(0); // Suspend timer
		m_PastPosition = XMFLOAT2(0.0f, 0.0f);
		m_Position = XMFLOAT2(0.0f, 0.0f);
	}

	// Update the position of the mouse
	POINTS mousePts; // Mouse position in integer format
	if (startTracking || (m_Tracking && positionMustChange))
	{
		// Save the previous position and time
		if (m_Moving)
		{
			m_tPast = m_t;
			m_PastPosition = m_Position;
		}

		// Update the time
		m_t = GetTickCount();

		// Retrieve mouse position
		mousePts = MAKEPOINTS(lparam);
		m_Position.x = static_cast<float>(mousePts.x);
		m_Position.y = static_cast<float>(mousePts.y);

		// Output mouse position
		//if (verbose_mouse)
		//{
		//	wostringstream mousePosWOSStream;
		//	mousePosWOSStream << L"Mouse -- Position in window: ( x, y ) = ( ";
		//	mousePosWOSStream << m_Position.x;
		//	mousePosWOSStream << L", ";
		//	mousePosWOSStream << m_Position.y;
		//	mousePosWOSStream << L" )\n";
		//	// writeToDebugConsole(mousePosWOSStream.str().c_str());
		//}
	}
	return result;
}

bool Mouse::IsBeingTracked(void) const
{
	return m_Tracking;
}

bool Mouse::IsPressed(Button index) const
{
	return m_ButtonStates[static_cast<int>(index)];
}

bool Mouse::IsMoving(bool& moving) const
{
	if (!m_Tracking)
	{
		return false;
	}
	moving = m_Moving;
	return true;
}

bool Mouse::GetWindowPosition(XMFLOAT2& position) const
{
	if (!m_Tracking)
	{
		return false;
	}
	position = m_Position;
	return true;
}

bool Mouse::GetWindowVelocity(XMFLOAT2& velocity) const
{
	if (!m_Tracking || !m_Moving || m_tPast == static_cast<DWORD>(0))
	{
		return false;
	}
	velocity.x = (m_Position.x - m_PastPosition.x) /
		static_cast<float>(m_t - m_tPast);
	velocity.y = (m_Position.y - m_PastPosition.y) /
		static_cast<float>(m_t - m_tPast);
	return true;
}

bool Mouse::MapScreenToWorld(const XMFLOAT2& screenPosition, const CineCameraClass& camera, XMFLOAT3& nearClipPosition, XMFLOAT3& farClipPosition) const
{
	// Input validation
	if (screenPosition.x < 0 || screenPosition.y < 0 || screenPosition.x > m_ScreenDimensions.x || screenPosition.y > m_ScreenDimensions.y)
	{
		// The position is outside the screen
		return false;
	}

	// The following code is based on the (hopefully not outdated) article at
	// https://www.mvps.org/directx/articles/rayproj.htm
	/* I had difficulty finding anything about ray tracing in DirectX on the web.
	However, just as I finished writing the following code based on the above link,
	I discovered the XMVector3Unproject method
	(see http://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.transformation.xmvector3unproject%28v=vs.85%29.aspx ),
	which performs the same function.
	I was unable to get meaningful coordinates using this function, unfortunately.
	*/

	/* Step 1 : Normalize the screen coordinates such that the origin is at the centre
	of the screen, and the coordinates are in the range [0, 1] */
	float cameraAspectRatio = camera.GetAspectRatio();
	float xNormalized;
	float yNormalized;
	xNormalized = (((2.0f * screenPosition.x) / m_ScreenDimensions.x) - 1.0f) * cameraAspectRatio;
	yNormalized = 1.0f - ((2.0f * screenPosition.y) / m_ScreenDimensions.y);

	/* Step 2 : Determine frustum view coordinates (x,y) in the form of
	ratios of the entire frustum dimensions */
	float fieldOfView = 0.0f;
	camera.GetFieldOfView(fieldOfView);
	XMFLOAT2 tanCalcTemp(fieldOfView / 2.0f, 0.0f);
	XMStoreFloat2(&tanCalcTemp, XMVectorTan(XMLoadFloat2(&tanCalcTemp)));
	float tanHalfFOV = tanCalcTemp.x;

	XMFLOAT2 xyRatios(tanHalfFOV*xNormalized, tanHalfFOV*yNormalized);

	/* Step 3 : Determine frustum view coordinates (x,y,z) in view space */
	XMFLOAT2 clipDistances(0.0f, 0.0f);
	camera.GetClipDistances(clipDistances);
	XMFLOAT3 nearClipPosition_View(xyRatios.x * clipDistances.x, xyRatios.y * clipDistances.x, clipDistances.x);
	XMFLOAT3  farClipPosition_View(xyRatios.x * clipDistances.y, xyRatios.y * clipDistances.y, clipDistances.y);

	/* Step 4 : Get the inverse of the view matrix */
	XMFLOAT4X4 viewMatrix;
	camera.GetViewMatrix(viewMatrix);
	XMMATRIX invViewMatrix = XMMatrixInverse(NULL, XMLoadFloat4x4(&viewMatrix));

	/* Step 5 : Transform the ray start and end coordinates to world coordinate space */
	XMStoreFloat3(&nearClipPosition,
		XMVector3Transform(XMLoadFloat3(&nearClipPosition_View), invViewMatrix));
	XMStoreFloat3(&farClipPosition,
		XMVector3Transform(XMLoadFloat3(&farClipPosition_View), invViewMatrix));

	//// Output results
	//if (verbose_mouse)
	//{
	//	wostringstream mousePosWOSStream;
	//	mousePosWOSStream << L"  Non-Library Position: ( x, y, z near clip plane ) \n\t= ( ";
	//	mousePosWOSStream << nearClipPosition.x;
	//	mousePosWOSStream << L", ";
	//	mousePosWOSStream << nearClipPosition.y;
	//	mousePosWOSStream << L", ";
	//	mousePosWOSStream << nearClipPosition.z;
	//	mousePosWOSStream << L" )\n";
	//	mousePosWOSStream << L"  Non-Library Position: ( x, y, z far  clip plane ) \n\t= ( ";
	//	mousePosWOSStream << farClipPosition.x;
	//	mousePosWOSStream << L", ";
	//	mousePosWOSStream << farClipPosition.y;
	//	mousePosWOSStream << L", ";
	//	mousePosWOSStream << farClipPosition.z;
	//	mousePosWOSStream << L" )\n";
	//	
	//	XMFLOAT3 cameraPosition = camera.GetPosition();
	//	XMFLOAT3 cameraPositionLength; // Distance from the origin
	//	XMStoreFloat3(&cameraPositionLength, XMVector3Length(XMLoadFloat3(&cameraPosition)));
	//	XMFLOAT3 originIntersect; // Ray intersection with the origin
	//	float frac = (cameraPositionLength.x - clipDistances.x) / (clipDistances.y - clipDistances.x);
	//	XMStoreFloat3(&originIntersect, XMVectorLerp(XMLoadFloat3(&nearClipPosition), XMLoadFloat3(&farClipPosition), frac));
	//	mousePosWOSStream << L"  Non-Library Position: ( x, y, z origin          ) \n\t= ( ";
	//	mousePosWOSStream << originIntersect.x;
	//	mousePosWOSStream << L", ";
	//	mousePosWOSStream << originIntersect.y;
	//	mousePosWOSStream << L", ";
	//	mousePosWOSStream << originIntersect.z;
	//	mousePosWOSStream << L" )\n";

	//	// writeToDebugConsole(mousePosWOSStream.str().c_str());
	//}
	return true;
}

bool Mouse::GetWorldPosition(const CineCameraClass& camera, const float distAlongCameraLook, XMFLOAT3& worldPosition) const
{
	if (!m_Tracking)
	{
		return false;
	}

	// Validate input
	XMFLOAT2 clipDistances(0.0f, 0.0f);
	camera.GetClipDistances(clipDistances);
	if (distAlongCameraLook < clipDistances.x || distAlongCameraLook > clipDistances.y)
	{
		return false;
	}

	// Retrieve world coordinates of the mouse
	XMFLOAT3 nearClipPosition(0.0f, 0.0f, 0.0f);
	XMFLOAT3 farClipPosition(0.0f, 0.0f, 0.0f);
	MapScreenToWorld(m_Position, camera, nearClipPosition, farClipPosition);

	// Interpolate to desired distance
	XMFLOAT3 cameraPosition = camera.GetPosition();
	float frac = distAlongCameraLook / clipDistances.y;
	XMStoreFloat3(&worldPosition, XMVectorLerp(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&farClipPosition), frac));

	return true;
}

bool Mouse::GetWorldVelocity(const CineCameraClass& camera, const float distAlongCameraLook, XMFLOAT3& worldVelocity) const
{
	if (!m_Tracking || !m_Moving || m_tPast == static_cast<DWORD>(0))
	{
		return false;
	}

	// Validate input
	XMFLOAT2 clipDistances(0.0f, 0.0f);
	camera.GetClipDistances(clipDistances);
	if (distAlongCameraLook < clipDistances.x || distAlongCameraLook > clipDistances.y)
	{
		return false;
	}

	// Retrieve present world coordinates of the mouse
	XMFLOAT3 tempPosition(0.0f, 0.0f, 0.0f);
	XMFLOAT3 farClipPosition(0.0f, 0.0f, 0.0f);
	MapScreenToWorld(m_Position,    camera, tempPosition, farClipPosition);

	// Retrieve past world coordinates of the mouse
	XMFLOAT3 farClipPastPosition(0.0f, 0.0f, 0.0f);
	MapScreenToWorld(m_PastPosition, camera, tempPosition, farClipPastPosition);

	// Determine velocity at far clip plane
	XMFLOAT3 farClipVelocity(
		(farClipPosition.x - farClipPastPosition.x) / static_cast<float>(m_t - m_tPast),
		(farClipPosition.y - farClipPastPosition.y) / static_cast<float>(m_t - m_tPast),
		(farClipPosition.z - farClipPastPosition.z) / static_cast<float>(m_t - m_tPast)
		);

	// Interpolate to desired distance
	tempPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float frac = distAlongCameraLook / clipDistances.y;
	XMStoreFloat3(&worldVelocity, XMVectorLerp(XMLoadFloat3(&tempPosition), XMLoadFloat3(&farClipVelocity), frac));

	return true;
}

bool Mouse::GetWorldDirection(const CineCameraClass& camera, XMFLOAT3& worldDirection) const
{
	if (!m_Tracking)
	{
		return false;
	}

	// Retrieve world coordinates of the mouse
	XMFLOAT3 nearClipPosition(0.0f, 0.0f, 0.0f);
	XMFLOAT3 farClipPosition(0.0f, 0.0f, 0.0f);
	MapScreenToWorld(m_Position, camera, nearClipPosition, farClipPosition);

	// Determine ray from the camera
	XMFLOAT3 cameraPosition = camera.GetPosition();
	XMFLOAT3 ray(
		farClipPosition.x - cameraPosition.x,
		farClipPosition.y - cameraPosition.y,
		farClipPosition.z - cameraPosition.z
		);

	// Normalize the ray direction vector
	XMStoreFloat3(&worldDirection, XMVector3Normalize(XMLoadFloat3(&ray)));
	return true;
}

int Mouse::Update(void)
{
	if (!m_Tracking)
	{
		return C_OK;
	}

	// Check the time
	DWORD t = GetTickCount();
	if ((t - m_t) >= maxMotionSamplingInterval )
	{
		// Set the mouse speed to zero
		m_tPast = m_t;
		m_PastPosition = m_Position;
		m_t = t;

		// Indicate that speed data is available
		m_Moving = true;
	}
	return C_OK;
}