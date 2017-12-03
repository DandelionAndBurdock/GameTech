#pragma once

#pragma once

#include <ncltech\Scene.h>
class OBJMesh;

class TestScene5 : public Scene
{
public:
	TestScene5(const std::string& friendly_name);
	virtual ~TestScene5();

	virtual void OnInitializeScene()	 override;
	virtual void OnCleanupScene()		 override;
	virtual void OnUpdateScene(float dt) override;

protected:
	float m_AccumTime;
	OBJMesh* m_MeshPlayer;
};

