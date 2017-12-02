#pragma once

#include <ncltech\Scene.h>
#include <ncltech\PhysicsNode.h>

class PathFollowing;

class TestScene3 : public Scene
{
public:
	TestScene3(const std::string& friendly_name);
	virtual ~TestScene3();

	virtual void OnInitializeScene()	 override;
	virtual void OnCleanupScene()		 override;
	virtual void OnUpdateScene(float dt) override;

protected:
	void CreateTarget(Vector3 pos, Vector3 size, float invMass, bool good = true, bool moving = false);

	bool HitGoodTarget(PhysicsNode* this_obj, PhysicsNode* colliding_obj);
	bool HitBadTarget(PhysicsNode* this_obj, PhysicsNode* colliding_obj);

	float					m_AccumTime;
	GameObject*				m_pPlayer;
	int					    m_score = 0;
	Mesh*					m_TargetMesh;

	std::vector<PathFollowing*> movingTargets;

};