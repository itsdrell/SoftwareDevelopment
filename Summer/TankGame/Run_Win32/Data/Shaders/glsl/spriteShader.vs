// spriteShader.vs
#version 420 core

#include "inc/common.glsl"


// Attributes ============================================
// Inputs
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV; 

// Outputs
out vec2 passUV; 
out vec4 passColor; 
out vec3 passWorldPos;     // new


// Entry point - required.  What does this stage do?
void main( void )
{
   vec4 local_pos = vec4( POSITION, 1.0f );  

   vec4 world_pos = MODEL * local_pos ; 
   vec4 camera_pos = VIEW * world_pos; 
   vec4 clip_pos = PROJECTION * camera_pos; 

   passWorldPos = world_pos.xyz; 
   passUV = UV; 
   passColor = COLOR; 


   gl_Position = clip_pos; // we pass out a clip coordinate
}


