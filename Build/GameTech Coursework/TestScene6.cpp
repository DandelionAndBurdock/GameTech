#include "TestScene6.h"

#include <ncltech\Cloth.h>
#include <ncltech\GameObject.h>
#include <ncltech\SoftBodyConstraint.h>
#include <ncltech\CommonUtils.h>
TestScene6::TestScene6(const std::string& friendly_name) :
	Scene(friendly_name)
{
}


TestScene6::~TestScene6()
{
}

void TestScene6::OnInitializeScene() {
	clothMesh = new Cloth();
	GameObject* cloth = new GameObject("Cloth");
	cloth->SetRender(new RenderNode(clothMesh));
	cloth->Render()->SetBoundingRadius(1.0f);
	cloth->Render()->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	this->AddGameObject(cloth);

	// Pin edge of cloth
	for (int i = 0; i < DEFAULT_GRID_SIZE; ++i) {
		clothMesh->Pin(i);
	}

	// Sphere
	GameObject* sphere = CommonUtils::BuildSphereObject(
				"",					// Optional: Name
				Vector3(0.0f),				// Position
				0.5f,			// Half-Dimensions
				true,				// Physics Enabled?
				20.0f,				// Physical Mass (must have physics enabled)
				true,				// Physically Collidable (has collision shape)
				true,				// Dragable by user?
				Vector4(1.0, 0.0, 0.0, 1.0));// Render color
			this->AddGameObject(sphere);
			sphere->Physics()->SetIntegrationMethod(PhysicsNode::IntegrationType::NONE);

	clothMesh->AddSphere(dynamic_cast<SphereCollisionShape*>(sphere->Physics()->GetBroadCollisionShape()));
}

void TestScene6::OnUpdateScene(float dt) {
	if (!PhysicsEngine::Instance()->IsPaused()) {
		clothMesh->Update(dt);
	}
	
}