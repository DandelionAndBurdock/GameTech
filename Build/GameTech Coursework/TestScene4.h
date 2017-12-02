#pragma once

#include <ncltech\Scene.h>

class TestScene4 : public Scene
{
public:
	TestScene4(const std::string& friendly_name);
	virtual ~TestScene4();

	virtual void OnInitializeScene()	 override;
	virtual void OnCleanupScene()		 override;
	virtual void OnUpdateScene(float dt) override;

protected:
	float m_AccumTime;
};
