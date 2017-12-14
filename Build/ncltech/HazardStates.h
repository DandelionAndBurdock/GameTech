#pragma once

#include "State.h"
#include "../Tuts_Network_Server/Hazard.h"

class Pursue : public State<Hazard>
{
public:
	static Pursue* GetInstance();

	void Enter(Hazard* owner) override;
	void Update(Hazard* owner, float dt) override;
	void Exit(Hazard* owner) override;

	void ReceiveMessage(Hazard* owner, Messaging::Message msg) override;

	~Pursue();
protected:
	Pursue();

};

class FollowHazardPath : public State<Hazard> {
public:
	static FollowHazardPath* GetInstance();

	void Enter(Hazard* owner) override;
	void Update(Hazard* owner, float dt) override;
	void Exit(Hazard* owner) override;

	void ReceiveMessage(Hazard* owner, Messaging::Message msg) override;

	~FollowHazardPath();
protected:
	FollowHazardPath();
	
};

class Idle : public State<Hazard> {
public:
	static Idle* GetInstance();

	void Enter(Hazard* owner) override;
	void Update(Hazard* owner, float dt) override;
	void Exit(Hazard* owner) override;

	void ReceiveMessage(Hazard* owner, Messaging::Message msg) override;

	~Idle();
protected:
	Idle();

};

class Patrol : public State<Hazard> {
public:
	static Patrol* GetInstance();

	void Enter(Hazard* owner) override;
	void Update(Hazard* owner, float dt) override;
	void Exit(Hazard* owner) override;

	void ReceiveMessage(Hazard* owner, Messaging::Message msg) override;

	~Patrol();
protected:
	float idleTime = 2.0f;
	float pathTime = 2.0f;

	float timeSinceTransition = 0.0f;

	bool isFollowingPath = false;
	bool isIdle = false;
	Patrol();

};