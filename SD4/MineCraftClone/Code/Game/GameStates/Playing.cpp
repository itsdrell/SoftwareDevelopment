#include "Playing.hpp"
#include "Engine\Renderer\Images/Textures/Texture.hpp"
#include "Engine\Renderer\Images/Sprites/SpriteSheet.hpp"
#include "Engine\Core\General/Camera.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine\Core\Tools/Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Tools/DevConsole.hpp"
#include "Engine/Renderer/RenderableComponents/Material.hpp"
#include "Engine/Renderer/Images/Sprites/Sprite.hpp"
#include "Engine\Renderer\Systems\Scene2D.hpp"
#include "Engine\Renderer\Systems\SpriteRendering.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "Engine/Core/Tools/Command.hpp"
#include "Engine/Input/Mouse.hpp"
#include "Engine/Core/General/HeatMap.hpp"
#include "Engine/Renderer/Systems/Scene.hpp"
#include "Engine/Renderer/Systems/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Systems/MeshBuilder.hpp"
#include "Engine/Renderer/Images/Textures/TextureCube.hpp"
#include "Engine/Renderer/RenderableComponents/Renderable.hpp"
#include "Game/General/GameCamera.hpp"
#include "Game/General/World/Block.hpp"
#include "Game/General/World/BlockDefinition.hpp"
#include "Game/General/World/Chunk.hpp"
#include "Game/General/World/World.hpp"


//====================================================================================

//====================================================================================
Playing::Playing()
{
}

//--------------------------------------------------------------------------
Playing::~Playing()
{
	delete m_scene;
	m_scene = nullptr;

	delete m_renderingPath;
	m_renderingPath = nullptr;

	delete m_world;
	m_world = nullptr;

}

void Playing::StartUp()
{

	//---------------------------------------------------------
	// For Test Scene
	m_scene = new Scene("Test");
	m_renderingPath = new ForwardRenderingPath();

	//m_testChunk = new Chunk(IntVector2(1,1));

	m_world = new World();


}

void Playing::Update()
{
	//CheckKeyBoardInputs();
	m_world->Update();
}

void Playing::Render() const
{
	m_world->Render();
	//RenderTestCube();
}

//-----------------------------------------------------------------------------------------------
void Playing::RenderTestCube() const
{
	MeshBuilder mb;
	Vector3 center = Vector3(5.f, 0.f, 0.f);
	Vector3 dimensions = Vector3::ONE;
	Renderer* r = Renderer::GetInstance();

	BlockDefinition* theDefinition = BlockDefinition::GetDefinitionByName("snow");

	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.SetColor(Rgba::WHITE);

	//-----------------------------------------------------------------------------------------------
	// Top
	// br
	AABB2 uvs = theDefinition->m_topUVs;
	
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	uint idx = mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// Front face
	uvs = theDefinition->m_sideUVs;

	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// left
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// back
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// right face
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// Bottom Face
	uvs = theDefinition->m_bottomUVs;
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// Do the draw
	Mesh* mesh = mb.CreateMesh<Vertex3D_PCU>();

	Shader* cubeShader = Shader::CreateOrGetShader("default");
	cubeShader->SetCullMode(CULLMODE_FRONT);
	r->SetShader(cubeShader);
	r->SetCurrentTexture(0 , g_blockSpriteSheet.m_spriteSheetTexture);
	//r->SetCamera(m_camera);
	r->DrawMesh(mesh, true);
}

void Playing::RenderSkyBox() const
{

}

void Playing::CheckKeyBoardInputs()
{
	if(IsDevConsoleOpen())
		return;

	
}

void Playing::MoveCamera()
{
	
}



