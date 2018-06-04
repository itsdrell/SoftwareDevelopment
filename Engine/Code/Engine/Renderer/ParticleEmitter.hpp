#pragma once
#if !defined( __PARTICLE_EMITTER__ )
#define __PARTICLE_EMITTER__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/

#include "../Core/General/Transform.hpp"
#include "MeshBuilder.hpp"
#include "../Math/IntRange.hpp"
#include "../Core/Tools/Stopwatch.hpp"
#include <vector>
#include "../Core/General/Rgba.hpp"


/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

class Renderable;
class Mesh;

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/
struct particle_t
{
	Vector3 position; 
	Vector3 velocity;
	Vector3 force;
	float size; 
	float mass; 

	float time_born;        // time he was born
	float time_will_die;    // time he will die 

	void update( float dt ) 
	{
		Vector3 acceleration = force / mass; 
		velocity += acceleration * dt; 
		position += velocity * dt; 

		force = Vector3::ZERO; 
	}

	inline bool is_dead( float time ) { return time >= time_will_die; }

	float get_normalized_age( float t ) ;

};

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/


class ParticleEmitter 
{
public:
	ParticleEmitter(const Vector3& position, Rgba particleColor = GetRandomColor());
	~ParticleEmitter();

	void update( Camera* cam, float dt ); 
	void spawn_particle(); 
	void spawn_particles( uint count ); 

	void set_spawn_rate( float particles_per_second ); 

public:
	Transform					m_transform; 
	Renderable*					m_renderable;

	Mesh*						m_mesh; 
	MeshBuilder					m_builder; 

	std::vector<particle_t>		m_particles; 

	bool						m_spawns_over_time; 
	Timer						m_spawn_interval;  // Timer
	IntRange					m_burst; 
	Rgba						m_color;

	// knobs
	bool						m_randomVelocity;
	Vector3						m_velocity;
	Vector3						m_force; // normally gravity
};

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/
#endif 