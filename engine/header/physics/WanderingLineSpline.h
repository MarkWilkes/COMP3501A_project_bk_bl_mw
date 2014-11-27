/*
WanderingLineSpline.h
----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 26, 2014

Primary basis: Spline.h

Description
  -Narrows the functionality of the Spline class to produce a spline
     between two endpoints, with WanderingLineTransformable
	 objects defining the knots in-between.
  -WanderingLineTransformable objects are constructed
     with uniformly-distributed random interpolation parameter values
	 between 0 and 1.
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <list>
#include "Spline.h"
#include "WanderingLineTransformable.h"

class WanderingLineSpline : public Spline {

public:
	/* Creates a Spline that holds 'capacity' segments.
	   The 'knotParameters' structure will be passed
	   to the WanderingLineSpline constructor
	   when knots are created, although its 't' member
	   will be modified according to the position
	   of the knot along the curve.

	   'capacity' must be greater than zero or the constructor
	   will throw an exception.

	   'start' and 'end' define the ends of the spline.
	 */
	WanderingLineSpline(const size_t capacity, Transformable* const start,
		Transformable* const end,
		const WanderingLineTransformable::Parameters& knotParameters);

	virtual ~WanderingLineSpline(void);

	/* Changes the endpoints of the spline,
	   also setting them in all knots.
	 */
	HRESULT setEndpoints(Transformable* const start,
		Transformable* const end);

	/* Updates the knots of this Spline for the specified time interval,
	   'updateTimeInterval', starting from the time indicated by the 'currentTime'
	   parameter. (e.g. 'currentTime' could be the time since program startup.)

	   Time values are in milliseconds.
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	// Data members
private:

	/* Parameters used to construct knots. */
	WanderingLineTransformable::Parameters m_knotParameters;

	/* Endpoints of the spline */
	Transformable* const m_start; // Shared - not deleted by destructor
	Transformable* const m_end; // Shared - not deleted by destructor

	/* The transformations contained within the knots of the spline.
	 */
	std::list<WanderingLineTransformable*> m_knotTransforms;

	// Currently not implemented - will cause linker errors if called
private:
	WanderingLineSpline(const WanderingLineSpline& other);
	WanderingLineSpline& operator=(const WanderingLineSpline& other);
};