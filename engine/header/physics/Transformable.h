////////////////////////////////////////////////////////////////////////////////
// Filename: Transformable.h
////////////////////////////////////////////////////////////////////////////////

/*

PLEASE READ *HOW TO USE* BELOW

Created for: COMP3501A Game
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created October 2, 2014

Adapted from A2Transformable.h (COMP3501A Assignment 2 code)
and from the COMP3501A quaternion camera demo posted on cuLearn.

Description
-Quaternion orientation
-Capacity for hierarchichal transformations

How To Use:
-Either extend the class, or create pointers within the object that needs to use this class.
-Call Move, Strafe, Crane, Spin if you wish to modify the position/orientation of the object.
-If you're going to perform transformations, override the transformations function,
 modify the argument matrix, and save the modified argument matrix. The parent transformation
 and the calculation of the world transform with and without scale is handled automatically
 in the update function.
-If you override the update function instead of transformations, you'll have to handle the parent
 transformation, object transformations, and you'll have to save the modified matrix to 
 the world space matrix (worldTransform and worldTransformNoScale).
-The position of the object in world space is represented by m_worldTransform.
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "ITransformable.h"
#include "engineGlobals.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Transformable
////////////////////////////////////////////////////////////////////////////////

const float TRANSFORM_ORI_CHANGE_FACTOR = 0.01f; // amount to change the roll, pitch, and yaw by

class Transformable : public ITransformable
{
	// Data members
protected:
	DirectX::XMFLOAT4X4 m_worldTransform;
	DirectX::XMFLOAT4X4 m_worldTransformNoScale;
	Transformable* m_parent;

	DirectX::XMFLOAT3 m_scale;

	// Copied from demo - Camera variables
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT4 m_orientation;
	DirectX::XMFLOAT3 m_forward;
	DirectX::XMFLOAT3 m_up;
	DirectX::XMFLOAT3 m_left; // player coordinate frame, rebuilt from orientation

	// Motion variables
public:
	DirectX::XMFLOAT4 m_L; // angular momentum (use as velocity)
	DirectX::XMFLOAT3 m_velDirection; // linear velocity direction (expected to be a unit vector)
	
	float m_speed; // Linear speed (units per second)
	float m_radius;

public:
	Transformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);

	virtual ~Transformable(void);

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const override;

	virtual HRESULT getWorldTransformNoScale(DirectX::XMFLOAT4X4& worldTransformNoScale);

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	// override this function in the child and use it for matrix transformations
	virtual HRESULT transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval);

	HRESULT setParent(Transformable* const parent);
	HRESULT multiplyByMatrix(DirectX::XMFLOAT4X4 theMatrix);

	void Move(float amount); // move forward and back (move forward, move backward)
	void Strafe(float amount); // move left and right
	void Crane(float amount); // move up and down
	void Spin(float roll, float pitch, float yaw); // spin the object (tilt, pan)

	// Only use if object doesn't have a parent (returns true if is parent)
	bool MoveIfParent(float amount); // move forward and back (move forward, move backward)
	bool StrafeIfParent(float amount); // move left and right
	bool CraneIfParent(float amount); // move up and down
	bool SpinIfParent(float roll, float pitch, float yaw); // spin the object (tilt, pan)

	DirectX::XMFLOAT3 getScale() const;
	DirectX::XMFLOAT3 getPosition() const;
	DirectX::XMFLOAT4 getOrientation() const;

	// computes and returns the respective direction
	DirectX::XMFLOAT3 getForwardWorldDirection();
	DirectX::XMFLOAT3 getUpWorldDirection();
	DirectX::XMFLOAT3 getLeftWorldDirection();

protected:
	virtual void computeLocalTransform(DirectX::XMFLOAT4X4& localTransformNoScale, const DWORD updateTimeInterval);
	virtual void computeTransforms(DirectX::XMFLOAT4X4 newWorldTransform, const DWORD updateTimeInterval);
	void updateTransformProperties();

	// Currently not implemented - will cause linker errors if called
private:
	Transformable(const Transformable& other);
	Transformable& operator=(const Transformable& other);
};

