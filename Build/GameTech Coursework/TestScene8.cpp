#include "TestScene8.h"

#include <nclgl\Vector4.h>
#include <ncltech\GraphicsPipeline.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\SceneManager.h>
#include <ncltech\CommonUtils.h>
#include <nclgl\OBJMesh.h>
#include "../Tuts_Physics/ObjectPlayer.h"
#include <ncltech\CuboidCollisionShape.h>
#include <ncltech\CompositeCollisionShape.h>

#include <ncltech\BehaviourTypes.h>

using namespace CommonUtils;

TestScene8::TestScene8(const std::string& friendly_name)
	: Scene(friendly_name)
	, m_AccumTime(0.0f)
	, m_MeshPlayer(new OBJMesh(MESHDIR"raptor.obj"))
	//, m_MeshPlayer(new OBJMesh(MESHDIR"TIE-fighterNew.obj"))
{
	//GLuint m_pCheckerboardTex = SOIL_load_OGL_texture(TEXTUREDIR"checkerboard.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
	//m_MeshPlayer->SetTexture(m_pCheckerboardTex);
	//m_MeshPlayer->SetBumpMap(m_pCheckerboardTex);
}

TestScene8::~TestScene8()
{

	if (m_MeshPlayer)
	{
		delete m_MeshPlayer;
		m_MeshPlayer = NULL;
	}
}


void TestScene8::OnInitializeScene()
{
	//Disable the physics engine (We will be starting this later!)
	PhysicsEngine::Instance()->SetPaused(true);

	//Set the camera position
	GraphicsPipeline::Instance()->GetCamera()->SetPosition(Vector3(15.0f, 10.0f, -15.0f));
	GraphicsPipeline::Instance()->GetCamera()->SetYaw(140.f);
	GraphicsPipeline::Instance()->GetCamera()->SetPitch(-20.f);

	m_AccumTime = 0.0f;

	//<--- SCENE CREATION --->
	//Create Ground
	this->AddGameObject(BuildCuboidObject("Ground", Vector3(0.0f, -1.0f, 0.0f), Vector3(20.0f, 1.0f, 20.0f), true, 0.0f, true, false, Vector4(0.2f, 0.5f, 1.0f, 1.0f)));

	//Create spaceShip
	ObjectPlayer* player = new ObjectPlayer("Player1");
	player->SetRender(new RenderNode(m_MeshPlayer));
	player->Render()->SetBoundingRadius(1.0f);
	player->Render()->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	player->SetPhysics(new PhysicsNode());
	player->Physics()->SetPosition(Vector3(0.0f, 0.5f, 0.0f));
	//CompositeCollisionShape* raptorShape = new CompositeCollisionShape();
	//raptorShape->AddShape(new CuboidCollisionShape(Vector3(0.2f, 0.2f, 0.5f)), Vector3(0.0f));
	//raptorShape->AddShape(new CuboidCollisionShape(Vector3(0.1f, 0.1f, 0.5f)), Vector3(0.0f, 0.2f, 1.10f));
	//raptorShape->AddShape(new SphereCollisionShape(0.2f), Vector3(0.0f, 0.4f, -0.75f));
	//player->Physics()->SetNarrowPhaseCollisionShape(new CuboidCollisionShape(Vector3(0.5f, 0.5f, 1.0f)));
	//player->Physics()->SetNarrowPhaseCollisionShape(raptorShape);
	player->Physics()->AddSteeringBehaviour(Steering::WANDER);
	this->AddGameObject(player);


	//Create spaceShip
	ObjectPlayer* player2 = new ObjectPlayer("Player2");
	player2->SetRender(new RenderNode(m_MeshPlayer));
	player2->Render()->SetBoundingRadius(1.0f);
	player2->Render()->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	player2->Render()->SetModelScale(Vector3(0.5f));
	player2->SetPhysics(new PhysicsNode());
	player2->Physics()->SetPosition(Vector3(20.0f, 0.5f, 20.0f));
	//CompositeCollisionShape* raptorShape = new CompositeCollisionShape();
	//raptorShape->AddShape(new CuboidCollisionShape(Vector3(0.2f, 0.2f, 0.5f)), Vector3(0.0f));
	//raptorShape->AddShape(new CuboidCollisionShape(Vector3(0.1f, 0.1f, 0.5f)), Vector3(0.0f, 0.2f, 1.10f));
	//raptorShape->AddShape(new SphereCollisionShape(0.2f), Vector3(0.0f, 0.4f, -0.75f));
	//player->Physics()->SetNarrowPhaseCollisionShape(new CuboidCollisionShape(Vector3(0.5f, 0.5f, 1.0f)));
	//player->Physics()->SetNarrowPhaseCollisionShape(raptorShape);
	player2->Physics()->AddSteeringBehaviour(Steering::PURSUIT);
	player2->Physics()->ChangePursuitTarget(player->Physics());
	this->AddGameObject(player2);
}

void TestScene8::OnCleanupScene()
{
	//Just delete all created game objects 
	//  - this is the default command on any Scene instance so we don't really need to override this function here.
	Scene::OnCleanupScene();
}

void TestScene8::OnUpdateScene(float dt)
{
	Scene::OnUpdateScene(dt);
	m_AccumTime += dt;


	// You can add status entries to the top left from anywhere in the program
	NCLDebug::AddStatusEntry(Vector4(1.0f, 0.2f, 0.2f, 1.0f), "Newton's Cradle");

	// You can print text using 'printf' formatting
	bool donkeys = true;
	NCLDebug::AddStatusEntry(Vector4(1.0f, 0.4f, 0.4f, 1.0f), "   The %s in this scene are dragable", donkeys ? "donkeys" : "cubes");

}