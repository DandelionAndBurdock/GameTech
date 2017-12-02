#include "TestScene3.h"

#include <nclgl\Vector4.h>
#include <ncltech\GraphicsPipeline.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\SceneManager.h>
#include <ncltech\CommonMeshes.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\CuboidCollisionShape.h>
#include <ncltech\SphereCollisionShape.h>

#include <ncltech\PathFollowing.h>

using namespace CommonUtils;



namespace Target {
	// Colour for "good"/"bad" targets which increase/decrease score
	Vector4 goodColour = Vector4(0.0f, 0.4f, 0.0f, 1.0f);
	Vector4 badColour = Vector4(0.85f, 0.1f, 0.2f, 1.0f);
}

TestScene3::TestScene3(const std::string& friendly_name)
	: Scene(friendly_name)
	, m_AccumTime(0.0f)
	, m_pPlayer(NULL)
{
	// Load target texture
	GLuint tex = SOIL_load_OGL_texture(
		TEXTUREDIR"target.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Load target mesh
	Mesh* cube = CommonMeshes::Cube();
	m_TargetMesh = new Mesh(*cube);
	m_TargetMesh->SetTexture(tex);
}

TestScene3::~TestScene3()
{

}


void TestScene3::OnInitializeScene()
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


	//Create Target //TODO: Vary elasticity based on invMass
	CreateTarget(Vector3(0.1f + 5.f, 2.0f, 0.0f), Vector3(0.1f, 2.0f, 2.f), 0.5f);
	CreateTarget(Vector3(0.1f + 5.f, 2.0f, 10.0f), Vector3(0.1f, 0.5f, 0.5f), 2.0f);
	CreateTarget(Vector3(-10.0f, 2.0f, -15.0f), Vector3(0.1f, 1.5f, 1.5f), 0.1f, false);

	CreateTarget(Vector3(-10.0f, 2.0f, -15.0f), Vector3(0.1f, 1.5f, 1.5f), 0.1f, false, true);

}

void TestScene3::OnCleanupScene()
{
	for (auto& target : movingTargets) {
		delete target;
	}
	//Just delete all created game objects 
	//  - this is the default command on any Scene instance so we don't really need to override this function here.
	Scene::OnCleanupScene();
	
}

void TestScene3::OnUpdateScene(float dt)
{
	m_AccumTime += dt;

	// You can print text using 'printf' formatting
	bool donkeys = true;
	NCLDebug::AddStatusEntry(Vector4(1.0f, 0.4f, 0.4f, 1.0f), "   The %s in this scene are dragable", donkeys ? "donkeys" : "cubes");
	NCLDebug::AddStatusEntry(Vector4(1.0f, 0.0f, 0.0f, 1.0f), "	   Score: %i ", m_score);
	Scene::OnUpdateScene(dt);
	for (auto& target : movingTargets) {
		target->Update(dt);
	}
}

void TestScene3::CreateTarget(Vector3 pos, Vector3 size, float invMass, bool good, bool moving) {
	using namespace Target;
	RenderNode* targetRender = new RenderNode();
	targetRender->SetMesh(m_TargetMesh);
	targetRender->SetTransform(Matrix4::Scale(size));
	if (good) {
		targetRender->SetColor(goodColour);
	}
	else {
		targetRender->SetColor(badColour);
	}
	
	targetRender->SetBoundingRadius(4.0f);


	GameObject* targetObject = new GameObject("Target");
	targetObject->SetRender(new RenderNode());
	targetObject->Render()->AddChild(targetRender);
	targetObject->SetPhysics(new PhysicsNode());
	targetObject->Physics()->SetInverseMass(invMass);
	targetObject->Physics()->SetPosition(pos);
	CollisionShape* pColshape = new CuboidCollisionShape(size);
	targetObject->Physics()->SetNarrowPhaseCollisionShape(pColshape);
	targetObject->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(invMass));
	float radius = max(max(size.x, size.y), size.z);
	CollisionShape* pColBPshape = new SphereCollisionShape(radius);
	targetObject->Physics()->SetBroadPhaseCollisionShape(pColBPshape);

	using namespace std::placeholders; // For _1, _2
	if (good) {
		targetObject->Physics()->SetOnCollisionCallback(std::bind(&TestScene3::HitGoodTarget, this, _1, _2));
	}
	else {
		targetObject->Physics()->SetOnCollisionCallback(std::bind(&TestScene3::HitBadTarget, this, _1, _2));
	}

	if (moving) {
		movingTargets.push_back(new PathFollowing(targetObject));
	}

	this->AddGameObject(targetObject);
}


bool TestScene3::HitGoodTarget(PhysicsNode* this_obj, PhysicsNode* colliding_obj) {
	if (colliding_obj->GetParent()->GetName() == "Projectile") {
		m_score += 100;
	}
	return true;
}
bool TestScene3::HitBadTarget(PhysicsNode* this_obj, PhysicsNode* colliding_obj) {
	if (colliding_obj->GetParent()->GetName() == "Projectile") {
		m_score -= 50;
	}
	return true;
}