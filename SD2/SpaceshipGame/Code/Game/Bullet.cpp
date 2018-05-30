#include "Game/Bullet.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Lights.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Material.hpp"
#include "GameCommon.hpp"

Bullet::Bullet(Matrix44 basis)
	: GameObject("Bullet")
{
	m_direction = basis.GetForward().Normalize();
	m_transform.SetLocalPosition(basis.GetPosition());

	//////////////////////////////////////////////////////////////////////////
	m_renderable->SetPosition(basis.GetPosition());

	Rgba color = GetRandomColorInRainbow();
	m_light = new PointLight(0,m_transform.GetLocalPosition());
	m_light->m_color = color;

	MeshBuilder mb;
	mb.AddUVSphere(Vector3::ZERO, 1.f, 6, 6 , color);
	Mesh* mesh = mb.CreateMesh<Vertex3D_PCU>();

	Material* bulletMaterial = new Material();
	Shader* bulletShader = Shader::CreateOrGetShader("Data/Shaders/additive.shader");
	bulletShader->SetFillMode(FILLMODE_SOLID);
	bulletMaterial->SetShader(bulletShader); 
	bulletMaterial->SetTexture(0, g_theRenderer->m_defaultTexture);
	//bulletMaterial->SetTexture(0, Renderer::GetInstance()->CreateOrGetTexture("Data/Images/particle.png" ));

	m_renderable->SetMesh(mesh);
	m_renderable->SetMaterial(bulletMaterial);

	//////////////////////////////////////////////////////////////////////////
	m_speed = 20.f;
	m_lifespane = 3.f;


}

void Bullet::Update()
{
	Vector3 currentPosition = m_transform.GetLocalPosition();

	Vector3 newPosition = currentPosition + (m_direction * m_speed * g_theGameClock->deltaTime);

	// update transforms
	m_transform.SetLocalPosition(newPosition);
	m_renderable->m_modelMatrix = m_transform.GetLocalMatrix();
	m_light->m_position = newPosition;


	// hacky timer
	if(m_lifespane <= 0)
	{
		m_isDead = true;
	}
	else
	{
		m_lifespane -= g_theGameClock->deltaTime;
	}
}

