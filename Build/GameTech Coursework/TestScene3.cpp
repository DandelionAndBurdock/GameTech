#include "TestScene3.h"

#include <nclgl\Vector4.h>
#include <ncltech\GraphicsPipeline.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\SceneManager.h>
#include <ncltech\CommonMeshes.h>
#include <ncltech\CommonUtils.h>
using namespace CommonUtils;

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


	//Create Target


	RenderNode* target = new RenderNode();
	target->SetMesh(m_TargetMesh);
	target->SetTransform(Matrix4::Translation(Vector3(0.1f + 5.f, 2.0f, 0.0f)) * Matrix4::Scale(Vector3(0.1f, 2.0f, 2.f)));
	target->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	target->SetBoundingRadius(4.0f);
	this->AddGameObject(new GameObject("Target", target, NULL));
}

void TestScene3::OnCleanupScene()
{
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

}