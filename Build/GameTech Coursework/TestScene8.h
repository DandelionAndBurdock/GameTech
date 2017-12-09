#pragma once

#include <ncltech\Scene.h>
class OBJMesh;

class TestScene8 : public Scene
{
public:
	TestScene8(const std::string& friendly_name);
	virtual ~TestScene8();

	virtual void OnInitializeScene()	 override;
	virtual void OnCleanupScene()		 override;
	virtual void OnUpdateScene(float dt) override;

protected:
	float m_AccumTime;
	OBJMesh* m_MeshPlayer;
};

