#include "CommonUtils.h"
#include "SphereCollisionShape.h"
#include "CuboidCollisionShape.h"
#include "CommonMeshes.h"
#include "ScreenPicker.h"
#include <nclgl\RenderNode.h>
#include <functional>

//Horrible!!!
// TODO: Fix these variables!
bool dragDataSet = false;
Matrix3 dragDataInertia;
float dragDataMass;

void CommonUtils::DragableObjectCallback(GameObject* obj, float dt, const Vector3& newWsPos, const Vector3& wsMovedAmount, bool stopDragging)
{
	if (obj->HasPhysics())
	{
		if (Window::GetMouse()->ButtonDown(MOUSE_LEFT))
		{
			//Position
			obj->Physics()->SetPosition(newWsPos);
			obj->Physics()->SetLinearVelocity(wsMovedAmount / dt);
			obj->Physics()->SetAngularVelocity(Vector3(0, 0, 0));
		}
		else if (Window::GetMouse()->ButtonDown(MOUSE_RIGHT))
		{
			Matrix3 viewRot = (Matrix3(GraphicsPipeline::Instance()->GetCamera()->BuildViewMatrix()));
			Matrix3 invViewRot = Matrix3::Transpose(viewRot);

			//Rotation
			Vector3 angVel = invViewRot * Vector3::Cross(Vector3(0, 0, 1), viewRot * wsMovedAmount * 25.f);

			obj->Physics()->SetAngularVelocity(angVel);
			Quaternion quat = obj->Physics()->GetOrientation();
			quat = quat + Quaternion(angVel * dt * 0.5f, 0.0f) * quat;
			quat.Normalise();
			obj->Physics()->SetOrientation(quat);
			obj->Physics()->SetLinearVelocity(Vector3(0, 0, 0));
		}


		if (stopDragging)
		{
			dragDataSet = false;

			obj->Physics()->SetInverseInertia(dragDataInertia);
			obj->Physics()->SetInverseMass(dragDataMass);
			
		}
		else if(!dragDataSet)
		{
			dragDataSet = true;

			dragDataInertia = obj->Physics()->GetInverseInertia();
			dragDataMass = obj->Physics()->GetInverseMass();

			obj->Physics()->SetInverseInertia(Matrix3::ZeroMatrix);
			obj->Physics()->SetInverseMass(0.0f);
		}
	}
	else
	{
		Matrix4 transform = obj->Render()->GetTransform();
		transform.SetPositionVector(newWsPos);
		obj->Render()->SetTransform(transform);
	}
}

Vector4 CommonUtils::GenColor(float scalar, float alpha)
{
	Vector4 c;
	c.w = alpha;

	//Hue
	float t;
	c.x = abs(modf(scalar + 1.0f, &t) * 6.0f - 3.0f) - 1.0f;
	c.y = abs(modf(scalar + 2.0f / 3.0f, &t) * 6.0f - 3.0f) - 1.0f;
	c.z = abs(modf(scalar + 1.0f / 3.0f, &t) * 6.0f - 3.0f) - 1.0f;

	//Safety Checks
	c.x = min(max(c.x, 0.0f), 1.0f);
	c.y = min(max(c.y, 0.0f), 1.0f);
	c.z = min(max(c.z, 0.0f), 1.0f);

	return c;
}

Vector4 CommonUtils::GenHSVColor(const Vector3& hsv, float alpha)
{
	Vector4 c = GenColor(hsv.x, alpha);

	//Saturation & Value
	c.x = hsv.z * (c.x * hsv.y + 1.0f - hsv.y);
	c.y = hsv.z * (c.y * hsv.y + 1.0f - hsv.y);
	c.z = hsv.z * (c.z * hsv.y + 1.0f - hsv.y);

	return c;
}



bool ChangeColourCallback(PhysicsNode* this_obj, PhysicsNode* colliding_obj) {
	this_obj->GetParent()->Render()->SetColorRecursive(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	return true;
}


GameObject* CommonUtils::BuildSphereObject(
	const std::string& name,
	const Vector3& pos,
	float radius,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	//Due to the way SceneNode/RenderNode's were setup, we have to make a dummy node which has the mesh and scaling transform
	// and a parent node that will contain the world transform/physics transform
	RenderNode* rnode = new RenderNode();

	RenderNode* dummy = new RenderNode(CommonMeshes::Sphere(), color);
	dummy->SetTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
	rnode->AddChild(dummy);

	rnode->SetTransform(Matrix4::Translation(pos));
	rnode->SetBoundingRadius(radius);

	PhysicsNode* pnode = NULL;	
	if (physics_enabled)
	{
		pnode = new PhysicsNode();
		pnode->SetPosition(pos);
		pnode->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			pnode->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new SphereCollisionShape(radius);
			pnode->SetNarrowPhaseCollisionShape(pColshape);
			pnode->SetBroadPhaseCollisionShape(pColshape);
			pnode->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	GameObject* obj = new GameObject(name, rnode, pnode);

	if (dragable)
	{
		ScreenPicker::Instance()->RegisterNodeForMouseCallback(
			dummy, //Dummy is the rendernode that actually contains the drawable mesh, and the one we can to 'drag'
			std::bind(&DragableObjectCallback, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
		);
	}

	return obj;
}

GameObject* CommonUtils::BuildInvisibleSphereObject(const std::string& name,
	const Vector3& pos,
	float radius,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color) {
	PhysicsNode* pnode = NULL;
	if (physics_enabled)
	{
		pnode = new PhysicsNode();
		pnode->SetPosition(pos);
		pnode->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			pnode->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new SphereCollisionShape(radius);
			pnode->SetNarrowPhaseCollisionShape(pColshape);
			pnode->SetBroadPhaseCollisionShape(pColshape);
			pnode->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	GameObject* obj = new GameObject(name, nullptr, pnode);
	return obj;
}

GameObject* CommonUtils::BuildCuboidObject(
	const std::string& name,
	const Vector3& pos,
	const Vector3& halfdims,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	//Due to the way SceneNode/RenderNode's were setup, we have to make a dummy node which has the mesh and scaling transform
	// and a parent node that will contain the world transform/physics transform
	RenderNode* rnode = new RenderNode();

	RenderNode* dummy = new RenderNode(CommonMeshes::Cube(), color);
	dummy->SetTransform(Matrix4::Scale(halfdims));
	rnode->AddChild(dummy);

	rnode->SetTransform(Matrix4::Translation(pos));
	rnode->SetBoundingRadius(halfdims.Length());

	PhysicsNode* pnode = NULL;
	if (physics_enabled)
	{
		pnode = new PhysicsNode();
		pnode->SetPosition(pos);
		pnode->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			pnode->SetInverseInertia(CuboidCollisionShape(halfdims).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new CuboidCollisionShape(halfdims);
			pnode->SetNarrowPhaseCollisionShape(pColshape);
			pnode->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
			CollisionShape* pColBPshape = new SphereCollisionShape(halfdims.Length());
			pnode->SetBroadPhaseCollisionShape(pColBPshape);
	
		}
	}

	GameObject* obj = new GameObject(name, rnode, pnode);

	if (dragable)
	{
		ScreenPicker::Instance()->RegisterNodeForMouseCallback(
			dummy, //Dummy is the rendernode that actually contains the drawable mesh
			std::bind(&DragableObjectCallback, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
		);
	}
	
	return obj;
}


void CommonUtils::SpawnSphere(const Vector3& spawnPos, const Vector3& dir) {
	GameObject* sphere = BuildSphereObject(
		"Projectile",
		spawnPos,
		SPHERE_RADIUS,
		true,
		SPHERE_INV_MASS,			//requires physics_enabled = true
		true,				//requires physics_enabled = true
		false,
		SPHERE_COLOUR);

	sphere->Physics()->SetPosition(spawnPos);
	sphere->Physics()->SetLinearVelocity(Vector3(dir) * SPHERE_SPEED);

	SceneManager::Instance()->GetCurrentScene()->AddProjectile(sphere);
}

void CommonUtils::DrawBox(Vector3 centre, float dimension, float thickness) {
	float halfDimension = dimension / 2.0f;

	// Corners of the box
	Vector3 frontBottomLeft = centre - Vector3(halfDimension, halfDimension, halfDimension);
	Vector3 frontBottomRight = centre - Vector3(-halfDimension, halfDimension, halfDimension);

	Vector3 backBottomLeft = centre - Vector3(halfDimension, halfDimension, -halfDimension);
	Vector3 backBottomRight = centre - Vector3(-halfDimension, halfDimension, -halfDimension);

	Vector3 backTopRight = centre + Vector3(halfDimension, halfDimension, halfDimension);
	Vector3 backTopLeft = centre + Vector3(-halfDimension, halfDimension, halfDimension);

	Vector3 frontTopRight = centre - Vector3(-halfDimension, -halfDimension, halfDimension);
	Vector3 frontTopLeft = centre - Vector3(halfDimension, -halfDimension, halfDimension);

	NCLDebug::DrawThickLine(frontBottomLeft, frontBottomRight, thickness);
	NCLDebug::DrawThickLine(frontBottomLeft, frontTopLeft, thickness);
	NCLDebug::DrawThickLine(backBottomLeft, backBottomRight, thickness);
	NCLDebug::DrawThickLine(backBottomLeft, backTopLeft, thickness);
	NCLDebug::DrawThickLine(backBottomLeft, frontBottomLeft, thickness);
	NCLDebug::DrawThickLine(backTopLeft, frontTopLeft, thickness);
	NCLDebug::DrawThickLine(backTopRight, frontTopRight, thickness);
	NCLDebug::DrawThickLine(backBottomRight, frontBottomRight, thickness);
	NCLDebug::DrawThickLine(backBottomRight, backTopRight, thickness);
	NCLDebug::DrawThickLine(frontBottomRight, frontTopRight, thickness);
	NCLDebug::DrawThickLine(frontTopLeft, frontTopRight, thickness);
	NCLDebug::DrawThickLine(backTopLeft, backTopRight, thickness);
}