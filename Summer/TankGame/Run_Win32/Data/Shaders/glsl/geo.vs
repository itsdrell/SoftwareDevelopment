// define the shader version (this is required)
#version 420 core


#include "inc/common.glsl"


uniform vec4 TINT; 


// in
in vec3 POSITION;
in vec4 COLOR;      
in vec2 UV;     

// Out
out vec2 passUV; 
out vec4 passColor; 

// Entry point - required.  What does this stage do?
void main( void )
{
   vec4 local_pos = vec4( POSITION, 1.0f );  

   vec4 world_pos = MODEL * local_pos ; 
   vec4 camera_pos = VIEW * world_pos; 
   vec4 clip_pos = PROJECTION * camera_pos;
   
    passColor = COLOR * TINT; 
    passUV = UV; 

    gl_Position = clip_pos;  // changed
}