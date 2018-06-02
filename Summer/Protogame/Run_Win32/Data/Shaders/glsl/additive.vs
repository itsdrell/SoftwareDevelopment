#version 420 core

#include "inc/common.glsl"

// Attributes
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV; 

out vec2 passUV; 
out vec4 passColor; 

void main( void )
{
   vec4 local_pos = vec4( POSITION, 1.0f );	

   vec4 world_pos = MODEL * local_pos; // assume local is world for now; 
   vec4 camera_pos = VIEW * world_pos; 
   vec4 clip_pos = PROJECTION * camera_pos; 

   passUV = UV; 
   passColor = COLOR * COLOR.a; // pre-multiply the alpha in 
	gl_Position = clip_pos; // we pass out a clip coordinate
}
