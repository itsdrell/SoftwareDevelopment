#version 420 core

// Uniforms ==============================================
// Constants
#include "inc/light.glsl"


// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;


// Suggest always manually setting bindings - again, consitancy with 
// other rendering APIs and well as you can make assumptions in your
// engine without having to query
//layout(binding = 0) uniform sampler2D gTexDiffuse;

// Attributes ============================================
in vec2 passUV; 
in vec4 passColor; 
in vec3 passWorldPos;   // new
in vec3 passWorldNormal;// new

out vec4 outColor; 

// Entry Point ===========================================
void main( void )
{
   // Interpolation is linear, so normals become not normal
   // over a surface, so renormalize it. 
   vec3 surface_normal = normalize(passWorldNormal);

    vec3 debug_color = (surface_normal + vec3(1)) * .5f;
    outColor = vec4( debug_color, 1 ); 
}