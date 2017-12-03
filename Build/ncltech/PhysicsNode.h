/******************************************************************************
Class: PhysicsNode
Implements:
Author: 
	Author: Pieran Marris      <p.marris@newcastle.ac.uk> and YOU!
Description: 

	A class to store all of the properties that define a physical object in the world.

	This class shouldn't need to be changed for the course of the physics tutorials, though
	will be used extensively. 

	As we go through the physics tutorials, we will be adding more and more of these parameters into
	our physics engine and processing them as required. 

	The first ones (and all that is needed to give an object movement) we will use are:
		<---------LINEAR-------------->
		Vector3		position;						
		Vector3		linVelocity;
		Vector3		force;
		float		invMass;

		<----------ANGULAR-------------->
		Quaternion  orientation;
		Vector3		angVelocity;
		Vector3		torque;
		Matrix3     invInertia;


*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <nclgl\Quaternion.h>
#include <nclgl\Matrix3.h>
#include "CollisionShape.h"
#include <functional>

class PhysicsNode;

//Callback function called whenever a collision is detected between two objects
//Params:
//	PhysicsNode* this_obj			- The current object class that contains the callback
//	PhysicsNode* colliding_obj	- The object that is colliding with the given object
//Return:
//  True	- The physics engine should process the collision as normal
//	False	- The physics engine should drop the collision pair and not do any further collision resolution/manifold generation
//			  > This can be useful for AI to see if a player/agent is inside an area/collision volume
typedef std::function<bool(PhysicsNode* this_obj, PhysicsNode* colliding_obj)> PhysicsCollisionCallback;


//Callback function called whenever this physicsnode's world transform is updated
//Params:
//	const Matrix4& transform - New World transform of the physics node
typedef std::function<void(const Matrix4& transform)> PhysicsUpdateCallback;


class GameObject;
class PhysicsNode
{
public:
	enum IntegrationType { NONE, EXPLICIT_EULER, SYMPLECTIC_EULER, MIDPOINT, RK4};
public:
	PhysicsNode()
		: position(0.0f, 0.0f, 0.0f)
		, linVelocity(0.0f, 0.0f, 0.0f)
		, force(0.0f, 0.0f, 0.0f)
		, acceleration(0.0f, 0.0f, 0.0f)
		, invMass(0.0f)
		, orientation(0.0f, 0.0f, 0.0f, 1.0f)
		, angVelocity(0.0f, 0.0f, 0.0f)
		, angAcceleration(0.0f, 0.0f, 0.0f)
		, torque(0.0f, 0.0f, 0.0f)
		, invInertia(Matrix3::ZeroMatrix)
		, narrowCollisionShape(NULL)
		, broadCollisionShape(NULL)
		, friction(0.5f)
		, elasticity(0.9f)
	{
		lastPosition = position;
		lastOrientation = orientation;
		lastLinVelocity = linVelocity;
		lastAngVelocity = angVelocity;
		lastAcceleration = acceleration;
		
	}

	virtual ~PhysicsNode()
	{
		SAFE_DELETE(narrowCollisionShape);
		//SAFE_DELETE(broadCollisionShape);
	}


	//<-------- Integration --------->
	// Called automatically by PhysicsEngine on all physics nodes each frame
	void IntegratePreSolver(float dt);
	//<-- Between calling these two functions the physics engine will solve velocity to get 'true' final velocity -->
	void IntegratePostSolver(float dt);
	


	//<--------- GETTERS ------------->
	inline GameObject*			GetParent()					const { return parent; }

	inline float				GetElasticity()				const { return elasticity; }
	inline float				GetFriction()				const { return friction; }

	inline const Vector3&		GetPosition()				const { return position; }
	inline Vector3*				GetPositionPtr()				  { return &position; }
	inline const Vector3&		GetLinearVelocity()			const { return linVelocity; }
	inline const Vector3&		GetForce()					const { return force; }
	inline float				GetInverseMass()			const { return invMass; }

	inline const Quaternion&	GetOrientation()			const { return orientation; }
	inline const Vector3&		GetAngularVelocity()		const { return angVelocity; }
	inline const Vector3&		GetTorque()					const { return torque; }
	inline const Matrix3&		GetInverseInertia()			const { return invInertia; }

	Vector3						GetGravity();

	inline CollisionShape*		GetNarrowCollisionShape()	const { return narrowCollisionShape; }
	inline CollisionShape*		GetBroadCollisionShape()	const { return broadCollisionShape; }

	const Matrix4&				GetWorldSpaceTransform()    const { return worldTransform; }




	//<--------- SETTERS ------------->
	inline void SetParent(GameObject* obj)							{ parent = obj; }

	inline void SetElasticity(float elasticityCoeff)				{ elasticity = elasticityCoeff; }
	inline void SetFriction(float frictionCoeff)					{ friction = frictionCoeff; }

	inline void SetPosition(const Vector3& v)						{ position = v; FireOnUpdateCallback(); }
	inline void SetLinearVelocity(const Vector3& v)					{ linVelocity = v; }
	inline void SetForce(const Vector3& v)							{ force = v; }
	inline void SetInverseMass(const float& v)						{ invMass = v; }

	inline void SetOrientation(const Quaternion& v)					{ orientation = v; FireOnUpdateCallback(); }
	inline void SetAngularVelocity(const Vector3& v)				{ angVelocity = v; }
	inline void SetTorque(const Vector3& v)							{ torque = v; }
	inline void SetInverseInertia(const Matrix3& v)					{ invInertia = v; }

	inline void SetIntegrationMethod(IntegrationType t)				{ integrationType = t; }
	inline void SetGravitationalAttractor(Vector3* pos)				{ gravAttractorPos = pos; }

	inline void SetDamping(bool damping)							{ this->damping = damping; }

	inline void SetNarrowPhaseCollisionShape(CollisionShape* colShape)
	{ 
		if (narrowCollisionShape) narrowCollisionShape->SetParent(NULL);
		narrowCollisionShape = colShape;
		if (narrowCollisionShape) narrowCollisionShape->SetParent(this);
	}
	
	inline void SetBroadPhaseCollisionShape(CollisionShape* colShape)
	{
		if (broadCollisionShape) broadCollisionShape->SetParent(NULL);
		broadCollisionShape = colShape;
		if (broadCollisionShape) broadCollisionShape->SetParent(this);
	}


	//<---------- CALLBACKS ------------>
	inline void SetOnCollisionCallback(PhysicsCollisionCallback callback) { onCollisionCallback = callback; }
	inline bool FireOnCollisionEvent(PhysicsNode* obj_a, PhysicsNode* obj_b)
	{
		return (onCollisionCallback) ? onCollisionCallback(obj_a, obj_b) : true;
	}

	inline void SetOnUpdateCallback(PhysicsUpdateCallback callback) { onUpdateCallback = callback; }
	inline void FireOnUpdateCallback()
	{
		//Build world transform
		worldTransform = orientation.ToMatrix4();
		worldTransform.SetPositionVector(position);
			
		//Fire the OnUpdateCallback, notifying GameObject's and other potential
		// listeners that this PhysicsNode has a new world transform.
		if (onUpdateCallback) onUpdateCallback(worldTransform);
	}
	

protected:
	//Useful parameters
	GameObject*				parent;
	Matrix4					worldTransform;
	PhysicsUpdateCallback	onUpdateCallback;

	// Position of sun/planet centre which is the major gravitational attractor
	// If null gravity will be in the -y direction
	Vector3* gravAttractorPos = nullptr;
	Vector3 PredictAcceleration(Vector3 pos);

	// Motion should be damped
	bool damping = true;

//Added in Tutorial 2
	//<---------LINEAR-------------->
	Vector3		position;
	Vector3		linVelocity;
	Vector3		force;
	Vector3		acceleration;
	float		invMass;
	void IntegrateLinearVelocity(float dt);
	void IntegrateLinearPosition(float dt);
	void IntegrateForVelocity(float dt);
	void IntegrateForPosition(float dt);
	// Properties at the last time step
	Vector3		lastPosition;
	Vector3		lastLinVelocity;
	Vector3		lastAcceleration;

	// Runge-Kutta Coefficients
	Vector3 RKLinCoeffs[4]; //k1, k2, k3, k4
	void CalculateRKLinCoefficients(float dt);

	//<----------ANGULAR-------------->
	Quaternion  orientation;
	Vector3		angVelocity;
	Vector3		torque;
	Vector3		angAcceleration;
	Matrix3     invInertia;
	void IntegrateAngularVelocity(float dt);
	void IntegrateAngularPosition(float dt);

	// Properties at the last time step
	Quaternion lastOrientation;
	Vector3	   lastAngVelocity;
	Vector3	   lastAngAcceleration;

	// Runge-Kutta Coeffs
	Vector3 RKAngCoeffs[4]; //k1, k2, k3, k4
	void CalculateRKAngCoefficients(float dt);

	// Integration type
	IntegrationType integrationType = SYMPLECTIC_EULER;

//Added in Tutorial 4/5
	//<----------COLLISION------------>
	// Broad phase collision shape
	CollisionShape*				broadCollisionShape;
	// Narrow phase collision shape
	CollisionShape*				narrowCollisionShape;
	PhysicsCollisionCallback	onCollisionCallback;


//Added in Tutorial 5
	//<--------MATERIAL-------------->
	float				elasticity;		///Value from 0-1 definiing how much the object bounces off other objects
	float				friction;		///Value from 0-1 defining how much the object can slide off other objects

};