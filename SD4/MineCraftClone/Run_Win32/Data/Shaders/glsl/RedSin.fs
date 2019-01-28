#version 420 core

#include "inc/time.glsl"

// We match the name and type of the previous stages out
in vec4 passColor; // NEW, passed color
in vec2 passUV; 

// Outputs
out vec4 outColor; 

// Binding Texture Slots - saying we expect Diffuse to be at 0
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexOverlay; 

// Entry Point
void main( void )
{
   vec4 color = texture( gTexDiffuse, passUV ); 
   color = color * texture( gTexOverlay, passUV ); 
   
   // Set the red channel to be the sin wave based on time, range-
   // mapped 0 to 1. 
   color.r = (sin( SYSTEM_TIME ) + 1.0f) * .5f;
   // notice I don't use the blocks name (uboTimeClock)
   // as that is just used for the CPU to find the location,
   // I use the members just like I would use a normal uniform.


   outColor = color; 
}