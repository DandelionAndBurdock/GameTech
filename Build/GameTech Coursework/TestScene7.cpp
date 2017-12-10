#include "TestScene7.h"

#include <ncltech\Jelly.h>
#include <ncltech\GameObject.h>
#include <ncltech\SoftBodyConstraint.h>
#include <ncltech\CommonUtils.h>
TestScene7::TestScene7(const std::string& friendly_name) :
	Scene(friendly_name)
{
}


TestScene7::~TestScene7()
{
}

void TestScene7::OnInitializeScene() {
	//jellyMesh = new Jelly();
	//GameObject* cloth = new GameObject("Jelly");
	//cloth->SetRender(new RenderNode(jellyMesh));
	//cloth->Render()->SetBoundingRadius(1.0f);
	//cloth->Render()->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//this->AddGameObject(cloth);
	//
	//
	//// Sphere
	//GameObject* sphere = CommonUtils::BuildSphereObject(
	//	"",					// Optional: Name
	//	Vector3(0.0f, -2.0f, 0.0f),				// Position
	//	0.5f,			// Half-Dimensions
	//	true,				// Physics Enabled?
	//	20.0f,				// Physical Mass (must have physics enabled)
	//	true,				// Physically Collidable (has collision shape)
	//	true,				// Dragable by user?
	//	Vector4(1.0, 0.0, 0.0, 1.0));// Render color
	//this->AddGameObject(sphere);
	//sphere->Physics()->SetIntegrationMethod(PhysicsNode::IntegrationType::NONE);
	Jelly j(5, 1.0f);
	j.AddToScene(this);
	//jellyMesh->AddSphere(dynamic_cast<SphereCollisionShape*>(sphere->Physics()->GetBroadCollisionShape()));
}

void TestScene7::OnUpdateScene(float dt) {
	if (!PhysicsEngine::Instance()->IsPaused()) {
//		jellyMesh->Update(dt);
	}

}

void TestScene7::AddProjectile(GameObject* obj) {
	Scene::AddProjectile(obj);
	//jellyMesh->AddSphere(dynamic_cast<SphereCollisionShape*>(obj->Physics()->GetBroadCollisionShape()));
}