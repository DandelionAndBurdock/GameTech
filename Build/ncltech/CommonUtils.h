/******************************************************************************
Class: 
Namespace: CommonUtils
Implements:
Author:
	Pieran Marris <p.marris@newcastle.ac.uk>
Description:
	Common Utilities/functions to help in quickly building up a scene with as little code as possible.
	Contains compounded factory functions to automatically construct GameObject's that fit the parameters
	provided.

	Everything here, probably shouldn't be bundled with the physics engine library... and in honesty
	should have been built better. Though for the sake of simplicity (and for the love of ridiculously long
	parameter lists) it's here, and here to stay. =]

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "SceneManager.h"

namespace CommonUtils
{
	void DragableObjectCallback(GameObject* obj, float dt, const Vector3& newWsPos, const Vector3& wsMovedAmount, bool stopDragging);

	//Generates a unique color based on scalar parameter in the range of 0-1.
	// - Works off HSV color format, in this case Saturation and Value are 100% and Hue is dependant on 'scalar'
	Vector4 GenColor(float scalar, float alpha = 1.0f);

	//Similar as above, but full HSV rather than just hue
	// - All hsv values range from 0-1 (including hue!)
	Vector4 GenHSVColor(const Vector3& hsv, float alpha = 1.0f);

	//Generates a default Sphere object with the parameters specified.
	GameObject* BuildSphereObject(
		const std::string& name,
		const Vector3& pos,
		float radius,
		bool physics_enabled = false,
		float inverse_mass = 0.0f,			//requires physics_enabled = true
		bool collidable = true,				//requires physics_enabled = true
		bool dragable = true,
		const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	//Generates a default Sphere object with the parameters specified but no render node
	GameObject* BuildInvisibleSphereObject(
		const std::string& name,
		const Vector3& pos,
		float radius,
		bool physics_enabled = false,
		float inverse_mass = 0.0f,			//requires physics_enabled = true
		bool collidable = true,				//requires physics_enabled = true
		bool dragable = true,
		const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	//Generates a default Cuboid object with the parameters specified
	GameObject* BuildCuboidObject(
		const std::string& name,
		const Vector3& pos,
		const Vector3& halfdims,
		bool physics_enabled = false,
		float inverse_mass = 0.0f,			//requires physics_enabled = true
		bool collidable = true,				//requires physics_enabled = true
		bool dragable = true,
		const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	// Create a sphere with attached render node
	const float SPHERE_RADIUS = 1.0f;
	const float SPHERE_INV_MASS = 1.0f;
	const float SPHERE_SPEED = 50.0f;
	const Vector4 SPHERE_COLOUR = Vector4(0.8f, 0.2f, 0.8f, 1.0f);
	//GameObject* CreateSphere() {
	//	GameObject* sphere = BuildSphereObject(
	//		"",					// Optional: Name
	//		pos,				// Position
	//		ballsize,			// Half-Dimensions
	//		true,				// Physics Enabled?
	//		10.f,				// Physical Mass (must have physics enabled)
	//		true,				// Physically Collidable (has collision shape)
	//		false,				// Dragable by user?
	//		col);// Render color
	//	this->AddGameObject(sphere);
	//}
	//}
	// Spawn a sphere from spawnPos in direction dir
	void SpawnSphere(const Vector3& spawnPos, const Vector3& dir);

	void DrawBox(Vector3 centre, float dimension, float thickness = 0.1f);
};

