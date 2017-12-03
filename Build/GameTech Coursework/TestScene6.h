#pragma once

#include <ncltech\Scene.h>

class Cloth;
class TestScene6 : public Scene
{
public:
	TestScene6(const std::string& friendly_name);
	~TestScene6();

	void OnInitializeScene()	 override;
	void OnUpdateScene(float dt) override;

	void AddProjectile(GameObject* obj) override;
protected:
	Cloth* clothMesh;
};

