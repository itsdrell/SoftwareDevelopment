#include "Game/Asteroid.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Material.hpp"
#include "GameCommon.hpp"

Asteroid::Asteroid(const Vector3& position, float radius)
	: GameObject("Asteroid")
{
	m_transform.SetLocalPosition(position);
	m_radius = radius;

	//////////////////////////////////////////////////////////////////////////
	MeshBuilder mb;
	mb.AddDeformedSphere(Vector3::ZERO, radius, .5f, 8, 8 );
	//mb.AddUVSphere(Vector3::ZERO, radius, 8, 8 );
	Mesh* mesh = mb.CreateMesh<VertexLit>();

	Material* asteroidMaterial = new Material();
	Shader* bulletShader = Shader::CreateOrGetShader("Data/Shaders/lit.shader");
	bulletShader->SetFillMode(FILLMODE_SOLID);
	asteroidMaterial->SetShader(bulletShader); 
	asteroidMaterial->SetTexture(0, g_theRenderer->m_defaultTexture);

	m_renderable->SetPosition(position);
	m_renderable->SetMesh(mesh);
	m_renderable->SetMaterial(asteroidMaterial);


}

