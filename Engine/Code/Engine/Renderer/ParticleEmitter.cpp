/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "Engine/Renderer/ParticleEmitter.hpp"
#include "../Core/Clock.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Material.hpp"
#include "../Input/InputSystem.hpp"
#include "../Core/Camera.hpp"
#include "../Math/MathUtils.hpp"
#include "DebugRenderSystem.hpp"




//------------------------------------------------------------------------
// ParticleEmitter
//------------------------------------------------------------------------
//------------------------------------------------------------------------
ParticleEmitter::ParticleEmitter(const Vector3& position, Rgba particleColor)
{
	Renderer* r = Renderer::GetInstance();

	m_renderable = new Renderable();
	m_renderable->SetPosition(position);

	m_color = particleColor;

	Material* mat = new Material();
	mat->SetShader(Shader::CreateOrGetShader("Data/Shaders/additive.shader")); 
	mat->SetProperty("TINT", m_color);

	mat->SetTexture( 0,r->CreateOrGetTexture("Data/Images/paintParticle1.png" )); 

	//m_mesh = MakeShared( new Mesh(App::GetRenderer()->get_device()) ); 
	MeshBuilder mb;
	mb.AddUVSphere(position, 1.f, 16,16);
	m_mesh = mb.CreateMesh<Vertex3D_PCU>();

	m_renderable->SetMaterial(mat);
	m_renderable->SetMesh( m_mesh ); 

	m_spawn_interval = Timer(g_theGameClock);

	//////////////////////////////////////////////////////////////////////////
	// Default options
	m_randomVelocity = true;
	m_velocity = Vector3::ZERO;
	m_force = Vector3(0.f, -9.8f, 0.f); // normally gravity


	set_spawn_rate( 0.0f ); 
}

//------------------------------------------------------------------------
ParticleEmitter::~ParticleEmitter()
{
	//ITW_SAFE_RELEASE(m_mesh); 

	//Game::GetInstance()->destroy_renderable( m_renderable ); 
	m_renderable = nullptr; 
}

//------------------------------------------------------------------------
void ParticleEmitter::spawn_particle()
{
	particle_t p; 
	p.position = Vector3::ZERO; 
	
	if(m_randomVelocity)
		p.velocity = GetRandomDirection() * 10.f;
	else
		p.velocity = Vector3::ZERO; //GetRandomDirection() * 20.f; // This was GetRandomPointOnSphere()
	
	p.velocity.y = abs(p.velocity.y); 
	// p.velocity.y *= RandomRange( 5.0f, 20.0f ); 
	p.size = GetRandomFloat( 2.f, 8.f );  

	float lifetime = GetRandomFloat( 1.0f, 3.0f ); 

	p.time_born = g_theGameClock->totalTime; 
	p.time_will_die = p.time_born + lifetime; 

	p.force = Vector3::ZERO; 
	p.mass = 1.0f; 

	m_particles.push_back( p ); 
}

//------------------------------------------------------------------------
void ParticleEmitter::spawn_particles( uint count ) 
{
	for (uint i = 0; i < count; ++i) 
	{
		spawn_particle(); 
	}
}

//------------------------------------------------------------------------
void ParticleEmitter::set_spawn_rate( float particles_per_second )
{
	if (particles_per_second == 0.0f) 
	{
		m_spawns_over_time = false; 
	} 
	else 
	{
		m_spawns_over_time = true; 
		m_spawn_interval.SetTimer( 1.0f / particles_per_second ); 
	}
}

//------------------------------------------------------------------------
void ParticleEmitter::update( Camera *cam,  float dt )
{
	uint particles = m_spawn_interval.DecrementAll(); 
	spawn_particles( particles ); 

	//if (InputSystem::GetInstance()->WasKeyJustPressed(InputSystem::GetInstance()->G_THE_LETTER_P)) 
	//{
	//	spawn_particles( 20 ); 
	//}

	//DebugRenderLog(0.f,"Particle amount: " + std::to_string(m_particles.size()));

	float t = g_theGameClock->totalTime; 

	uint particle_count = (uint) m_particles.size();  
	for (uint i = particle_count - 1U; i < particle_count; --i) 
	{
		particle_t &p = m_particles[i]; 
		//p.force = Vector3( 0.0f, -9.8f, 0.0f ); 
		p.force = m_force; //Vector3(0.f,0.f, -9.8f);
		p.update(dt); 

		if (p.is_dead(t)) 
		{
			//m_particles.remove_at_fast(i); 
			m_particles.erase(m_particles.begin() + i, m_particles.begin() + (i+1));
		} 
	}

	MeshBuilder mb; 
	Vector3 right = cam->m_cameraMatrix.GetRight(); 
	Vector3 up = cam->m_cameraMatrix.GetUp(); 
	particle_count = (uint) m_particles.size(); 
	
	//mb.setup_for_vertex<vertex_pct>(); 4
	for (uint i = 0; i < particle_count; ++i) 
	{
		particle_t &p = m_particles[i]; 
		//MeshBuilderAddPlane( &mb, p.position, right, up, aabb2::Centered( vec2(p.size) ) ); 
		//mb.AddPlane(p.position, Vector3(p.size	,p.size , p.size) , GetRandomColorInRainbow()); 
		mb.AddPlane(p.position, Vector3(p.size), up, right, GetRandomColorInRainbow());
	}

	m_renderable->SetMesh(mb.CreateMesh<Vertex3D_PCU>());
	//m_mesh->update_from_builder( mb ); 
}

/************************************************************************/
/*                                                                      */
/* COMMANDS                                                             */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* UNIT TESTS                                                           */
/*                                                                      */
/************************************************************************/