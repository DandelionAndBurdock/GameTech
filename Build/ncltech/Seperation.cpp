#include "Seperation.h"



Seperation::Seperation(GameObject* entity) :
	SteeringBehaviour(entity)
{
}


Seperation::~Seperation()
{
}


Vector3 Seperation::GetVelocity() {
	// Apply an impulse inversely proportional to the seperation
	//Vector3 
	//return 
	//	SteeringForce += Vec2DNormalize(ToAgent) / ToAgent.Length();
	return Vector3();
}
void Seperation::Update(float dt) {

}