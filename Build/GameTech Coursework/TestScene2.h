#pragma once

#include <ncltech\Scene.h>
#include <ncltech\CommonUtils.h>

//Fully striped back scene to use as a template for new scenes.
class TestScene2 : public Scene
{
public:
	TestScene2(const std::string& friendly_name)
		: Scene(friendly_name)
	{
	}

	virtual ~TestScene2()
	{
	}

	virtual void OnInitializeScene() override
	{
		Scene::OnInitializeScene();


		GameObject* sun = CommonUtils::BuildSphereObject(
			"Sun",					// Optional: Name
			Vector3(0),				// Position
			3.0f,			// Half-Dimensions
			true,				// Physics Enabled?
			0.0f,				// Physical Mass (must have physics enabled)
			true,				// Physically Collidable (has collision shape)
			false,				// Dragable by user?
			Vector4(1.0f, 1.0f, 0.0f, 1.0f));// Render color
		this->AddGameObject(sun);

		radius = 15.0f;
		float orbitalSpeed = sqrt(PhysicsEngine::Instance()->GetGravity() * radius);
		angularFrequency = orbitalSpeed / radius;

		planets[0] = CommonUtils::BuildSphereObject(
			"My",					// Optional: Name
			Vector3(radius, 0.0f, 0.0f),				// Position
			0.5f,			// Half -Dimensions
			false,				// Physics Enabled?
			1.0f,				// Physical Mass (must have physics enabled)
			false,				// Physically Collidable (has collision shape)
			false,				// Dragable by user?
			Vector4(1.0f, 0.5f, 1.0f, 1.0f));// Render color

		this->AddGameObject(planets[0]);

		planets[1] = CommonUtils::BuildSphereObject(
			"Very",					// Optional: Name
			Vector3(radius, 0.0f, 0.0f),				// Position
			0.5f,			// Half -Dimensions
			true,				// Physics Enabled?
			1.0f,				// Physical Mass (must have physics enabled)
			false,				// Physically Collidable (has collision shape)
			false,				// Dragable by user?
			Vector4(1.0f, 0.5f, 0.0f, 1.0f));// Render color

		this->AddGameObject(planets[1]);
		planets[1]->Physics()->SetIntegrationMethod(PhysicsNode::IntegrationType::EXPLICIT_EULER);
		planets[1]->Physics()->SetGravitationalAttractor(sun->Physics()->GetPositionPtr());
		planets[1]->Physics()->SetLinearVelocity(Vector3(0.0f, 0.0f, orbitalSpeed));
		planets[1]->Physics()->SetDamping(false);

		planets[2] = CommonUtils::BuildSphereObject(
			"Easy",					// Optional: Name
			Vector3(radius, 0.0f, 0.0f),				// Position
			0.5f,			// Half-Dimensions
			true,				// Physics Enabled?
			1.0f,				// Physical Mass (must have physics enabled)
			false,				// Physically Collidable (has collision shape)
			false,				// Dragable by user?
			Vector4(0.3f, 0.5f, 0.4f, 1.0f));// Render color

		this->AddGameObject(planets[2]);
		planets[2]->Physics()->SetIntegrationMethod(PhysicsNode::IntegrationType::SYMPLECTIC_EULER);
		planets[2]->Physics()->SetGravitationalAttractor(sun->Physics()->GetPositionPtr());
		planets[2]->Physics()->SetLinearVelocity(Vector3(0.0f, 0.0f, orbitalSpeed));
		planets[2]->Physics()->SetDamping(false);

		planets[3] = CommonUtils::BuildSphereObject(
			"Method",					// Optional: Name
			Vector3(radius, 0.0f, 0.0f),				// Position
			0.5f,			// Half-Dimensions
			true,				// Physics Enabled?
			1.0f,				// Physical Mass (must have physics enabled)
			false,				// Physically Collidable (has collision shape)
			false,				// Dragable by user?
			Vector4(1.0f, 1.0f, 1.0f, 1.0f));// Render color

		this->AddGameObject(planets[3]);
		planets[3]->Physics()->SetIntegrationMethod(PhysicsNode::IntegrationType::MIDPOINT);
		planets[3]->Physics()->SetGravitationalAttractor(sun->Physics()->GetPositionPtr());
		planets[3]->Physics()->SetLinearVelocity(Vector3(0.0f, 0.0f, orbitalSpeed));
		planets[3]->Physics()->SetDamping(false);


		float moonRadius = 1.0f;
		float moonSpeed = sqrt(PhysicsEngine::Instance()->GetGravity() * moonRadius);
		GameObject* moon = CommonUtils::BuildSphereObject(
			"Moon",					// Optional: Name
			Vector3(radius, 0.0f, moonRadius),				// Position
			0.1f,			// Half-Dimensions
			true,				// Physics Enabled?
			1.0f,				// Physical Mass (must have physics enabled)
			false,				// Physically Collidable (has collision shape)
			false,				// Dragable by user?
			Vector4(1.0f, 1.0f, 1.0f, 1.0f));// Render color
		this->AddGameObject(moon);
		moon->Physics()->SetLinearVelocity(Vector3(moonSpeed, 0.0f, 0.0f));
		moon->Physics()->SetGravitationalAttractor(planets[1]->Physics()->GetPositionPtr());
		moon->Physics()->SetDamping(false);

	}
	void OnUpdateScene(float dt) override {
		if (!PhysicsEngine::Instance()->IsPaused()) {
			time += dt;
			planets[0]->Render()->SetTransform(Matrix4::Translation(Vector3(radius * cos(angularFrequency * time), 0.0f, radius * sin(angularFrequency * time))));
		}
	}


protected:
	static const int NUM_PLANETS = 4;
	GameObject* planets[NUM_PLANETS];
	float angularFrequency;
	float radius;
	float time = 0.0f;
};