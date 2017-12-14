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

	void ReceiveMessage(Messaging::Message msg) override;

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

	void ReceiveMessage(Messaging::Message msg) override;

	~FollowHazardPath();
protected:
	FollowHazardPath();
	
};