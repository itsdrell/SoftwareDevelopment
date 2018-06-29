// define the shader version (this is required)
#version 420 core

#include "inc/common.glsl"


// Attributes ============================================
// Inputs
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV; 

in vec3 NORMAL;   // new
in vec3 TANGENT;
in vec3 BITANGENT;

// Outputs
out vec2 passUV; 
out vec4 passColor; 
out vec3 passViewPos;
out vec3 passWorldPos;     // new
out vec3 passWorldNormal;  // new
out vec3 passWorldTangent; 
out vec3 passWorldBitangent;

// Entry point - required.  What does this stage do?
void main( void )
{
   vec4 local_pos = vec4( POSITION, 1.0f );  

   vec4 world_pos = MODEL * local_pos ; 
   vec4 camera_pos = VIEW * world_pos; 
   vec4 clip_pos = PROJECTION * camera_pos; 

   passViewPos = camera_pos.xyz; 

   passWorldPos = world_pos.xyz; 
   passUV = UV; 
   passColor = COLOR * TINT; 

   passWorldNormal = normalize((MODEL * vec4( NORMAL, 0.0f )).xyz); 
   passWorldTangent = normalize((MODEL * vec4( TANGENT, 0.0f )).xyz ); 
   passWorldBitangent = normalize((MODEL * vec4(BITANGENT, 0.0f)).xyz); 

   gl_Position = clip_pos; // we pass out a clip coordinate
}


