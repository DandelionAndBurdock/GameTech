#pragma once

#include <ncltech\Scene.h>

class TestScene3 : public Scene
{
public:
	TestScene3(const std::string& friendly_name);
	virtual ~TestScene3();

	virtual void OnInitializeScene()	 override;
	virtual void OnCleanupScene()		 override;
	virtual void OnUpdateScene(float dt) override;

protected:
	float					m_AccumTime;
	GameObject*				m_pPlayer;
	int					    m_score = 0;
	Mesh*					m_TargetMesh;

};