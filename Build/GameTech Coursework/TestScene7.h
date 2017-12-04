#pragma once

#include <ncltech\Scene.h>

class Jelly;
class TestScene7 : public Scene
{
public:
	TestScene7(const std::string& friendly_name);
	~TestScene7();

	void OnInitializeScene()	 override;
	void OnUpdateScene(float dt) override;

	void AddProjectile(GameObject* obj) override;
protected:
	Jelly* jellyMesh;
};

