// define the shader version (this is required)
#version 420 core

#include "inc/common.glsl"

// Attributes - input to this shasder stage (constant as far as the code is concerned)
in vec3 POSITION;

// Entry point - required.  What does this stage do?
void main( void )
{
   // for now, we're going to set the 
   // clip position of this vertex to the passed 
   // in position. 
   // gl_Position is a "system variable", or have special 
   // meaning within the shader.
   //gl_Position = vec4( POSITION, 1 ); 

   // multiply it through - for now, local_pos
   // is being treated as view space position
   // this will be updated later once we introduce the other matrices
   vec4 local_pos = vec4( POSITION, 1 ); 
   vec4 clip_pos = PROJECTION * local_pos; 
   
   gl_Position = clip_pos;  // changed
}


