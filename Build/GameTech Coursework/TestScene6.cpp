#include "TestScene6.h"

#include <ncltech\Cloth.h>
#include <ncltech\GameObject.h>
#include <ncltech\SoftBodyConstraint.h>
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
		

}

void TestScene6::OnUpdateScene(float dt) {
	if (!PhysicsEngine::Instance()->IsPaused()) {
		clothMesh->Update(dt);
	}
	
}