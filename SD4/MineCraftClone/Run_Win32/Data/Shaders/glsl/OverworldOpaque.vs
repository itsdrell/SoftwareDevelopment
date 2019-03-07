// define the shader version (this is required)
#version 420 core

//-----------------------------------------------------------------------------------------------
#include "inc/common.glsl"

//-----------------------------------------------------------------------------------------------
in vec3 POSITION;
in vec4 COLOR;       
in vec2 UV;          

//-----------------------------------------------------------------------------------------------
out vec3 passWorldPosition;
out vec2 passUV; 
out vec4 passColor;  // NEW - to use it in the pixel stage, we must pass it.

//-----------------------------------------------------------------------------------------------
void main( void )
{
    vec4 local_pos = vec4( POSITION, 1 ); 
    vec4 worldPos = MODEL * local_pos;
    vec4 clip_pos =  PROJECTION * (VIEW * local_pos); 
   
    passWorldPosition = worldPos.xyz;
    passColor = COLOR; // pass it on. 
    passUV = UV; // we have to set our outs.

    gl_Position = clip_pos;  // changed
}