#pragma once

#include <ncltech\Scene.h>

class Cloth;
class TestScene6 : public Scene
{
public:
	TestScene6(const std::string& friendly_name);
	~TestScene6();

	virtual void OnInitializeScene()	 override;
protected:
	Cloth* clothMesh;
};

