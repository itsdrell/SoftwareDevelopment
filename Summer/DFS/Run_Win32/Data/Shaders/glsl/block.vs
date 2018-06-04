#version 420 core

#include "inc/common.glsl"

// Attributes
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV; 

out vec2 passUV; 
out vec2 passBorderUV; 
out vec4 passColor; 

vec2 BORDER_UVS[] = { vec2(0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 0.0f), vec2(1.0f) };


void main( void )
{
   vec4 local_pos = vec4( POSITION, 1.0f );	

   vec4 world_pos = local_pos * MODEL; // assume local is world for now; 
   vec4 camera_pos = world_pos * VIEW; 
   vec4 clip_pos = camera_pos * PROJECTION; 

   passUV = UV; 
   passBorderUV = BORDER_UVS[gl_VertexID % 4]; 
   passColor = COLOR; 
	gl_Position = clip_pos; // we pass out a clip coordinate
}
