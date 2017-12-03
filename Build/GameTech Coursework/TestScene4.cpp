#include "TestScene4.h"

#include <nclgl\Vector4.h>
#include <ncltech\GraphicsPipeline.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\SceneManager.h>
#include <ncltech\CommonUtils.h>
using namespace CommonUtils;

TestScene4::TestScene4(const std::string& friendly_name)
	: Scene(friendly_name)
	, m_AccumTime(0.0f)
{
}

TestScene4::~TestScene4()
{

}


void TestScene4::OnInitializeScene()
{
	//Disable the physics engine (We will be starting this later!)
	PhysicsEngine::Instance()->SetPaused(true);

	//Set the camera position
	GraphicsPipeline::Instance()->GetCamera()->SetPosition(Vector3(15.0f, 10.0f, -15.0f));
	GraphicsPipeline::Instance()->GetCamera()->SetYaw(140.f);
	GraphicsPipeline::Instance()->GetCamera()->SetPitch(-20.f);

	m_AccumTime = 0.0f;

	//Example usage of Log 
	//- The on screen log can be opened from the bottom left though the
	//  main point of this is the ability to output to file easily from anywhere.
	//  So if your having trouble debugging with hundreds of 'cout << vector3()' just
	//  through them into NCLLOG() and look at the 'program_output.txt' later =]
	NCLDebug::Log("This is a log entry - It will printed to the console, on screen log and <project_dir>\\program_output.txt");
	//NCLERROR("THIS IS AN ERROR!"); // <- On Debug mode this will also trigger a breakpoint in your code!

	//<--- SCENE CREATION --->
	//Create Ground
	this->AddGameObject(BuildCuboidObject("Ground", Vector3(0.0f, -1.0f, 0.0f), Vector3(20.0f, 1.0f, 20.0f), true, 0.0f, true, false, Vector4(0.2f, 0.5f, 1.0f, 1.0f)));

	GameObject *handle, *ball;

	//Create Double Pendulum with Spring
	handle = CommonUtils::BuildSphereObject("",
		Vector3(-7.f, 8.f, -5.0f),				//Position
		0.5f,									//Radius
		true,									//Has Physics Object
		0.0f,									//Infinite Mass
		true,									//No Collision Shape Yet
		true,									//Dragable by the user
		CommonUtils::GenColor(0.45f, 0.5f));	//Color

	ball = CommonUtils::BuildSphereObject("",
		Vector3(-4.f, 7.5f, -5.0f),				//Position
		0.5f,									//Radius
		true,									//Has Physics Object
		1.0f,									// Inverse Mass = 1 / 1kg mass
		true,									//No Collision Shape Yet
		true,									//Dragable by the user
		CommonUtils::GenColor(0.5f, 1.0f));		//Color

	//Add distance constraint between the two objects
	DistanceConstraint* constraint = new DistanceConstraint(
		handle->Physics(),					//Physics Object A
		ball->Physics(),					//Physics Object B
		handle->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
		ball->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
	PhysicsEngine::Instance()->AddConstraint(constraint);

	// Create a box to make a double pendulum
	GameObject* box = CommonUtils::BuildCuboidObject("",
		Vector3(-4.f, 6.f, -5.0f),				//Position
		Vector3(0.7f, 0.7f, 0.7f),				//Half Dimensions
		true,									//Has Physics Object
		1.0f,									//Inverse Mass = 1 / 1kg mass
		true,									//No Collision Shape Yet
		false,									//Dragable by the user
		CommonUtils::GenColor(0.7f, 1.0f));		//Color


	PhysicsEngine::Instance()->AddConstraint(new DistanceConstraint(
		ball->Physics(),													//Physics Object A
		box->Physics(),													//Physics Object B
		ball->Physics()->GetPosition(),		//Attachment Position on Object A	-> Currently the far right edge
		box->Physics()->GetPosition() + Vector3(-0.5f, -0.5f, -0.5f)));	//Attachment Position on Object B	-> Currently the far left edge 

	PhysicsEngine::Instance()->AddConstraint(constraint);

	this->AddGameObject(handle);
	this->AddGameObject(ball);
	this->AddGameObject(box);

	// Newton's Cradle
	Vector3 handlePos = Vector3(7.f, 8.f, 5.0f);
	handle = CommonUtils::BuildSphereObject("",
		handlePos,				//Position
		0.4f,									//Radius
		true,									//Has Physics Object
		0.0f,									//Infinite Mass
		true,									//No Collision Shape Yet
		false,									//Dragable by the user
		CommonUtils::GenColor(0.45f, 0.5f));	//Color
	this->AddGameObject(handle);
	const int NUM_BALLS = 5;
	const float BALL_RADIUS = 0.75f;
	const float STRING_LENGTH = 3.0f;


	for (int i = 0; i < NUM_BALLS; ++i) {
		Vector3 ballPos;
		if (i == 0) {
			ballPos = handlePos;
			ballPos.x -= (STRING_LENGTH + BALL_RADIUS);	// Raise first ball
		}
		else {
			ballPos = handlePos;
			ballPos.y -= STRING_LENGTH + BALL_RADIUS;
			ballPos.x += (i - NUM_BALLS / 2) * (2 * BALL_RADIUS + 0.1f); 
		}
		ball = CommonUtils::BuildSphereObject("",
			ballPos,				//Position
			BALL_RADIUS,								//Radius
			true,									//Has Physics Object
			1.0f,									// Inverse Mass = 1 / 1kg mass
			true,									//No Collision Shape Yet
			true,									//Dragable by the user
			CommonUtils::GenColor(0.2f, 0.2f));		//Color
		this->AddGameObject(ball);
		PhysicsEngine::Instance()->AddConstraint(new DistanceConstraint(
			ball->Physics(),													//Physics Object A
			handle->Physics(),													//Physics Object B
			ball->Physics()->GetPosition(),		//Attachment Position on Object A	
			handle->Physics()->GetPosition()));	//Attachment Position on Object B	
			ball->Physics()->SetElasticity(0.99f);

		PhysicsEngine::Instance()->AddConstraint(constraint);

	}

}

void TestScene4::OnCleanupScene()
{
	//Just delete all created game objects 
	//  - this is the default command on any Scene instance so we don't really need to override this function here.
	Scene::OnCleanupScene();
}

void TestScene4::OnUpdateScene(float dt)
{
	m_AccumTime += dt;

	// You can add status entries to the top left from anywhere in the program
	NCLDebug::AddStatusEntry(Vector4(1.0f, 0.2f, 0.2f, 1.0f), "Newton's Cradle");

	// You can print text using 'printf' formatting
	bool donkeys = true;
	NCLDebug::AddStatusEntry(Vector4(1.0f, 0.4f, 0.4f, 1.0f), "   The %s in this scene are dragable", donkeys ? "donkeys" : "cubes");




}