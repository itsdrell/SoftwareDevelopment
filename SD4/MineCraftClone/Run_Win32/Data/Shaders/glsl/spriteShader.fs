// spriteShader.fs
#version 420 core

#include "inc/common.glsl"

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;


// We match the name and type of the previous stages out
in vec4 passColor; // NEW, passed color
in vec2 passUV; 


// Outputs
out vec4 outColor; 

// Entry Point
void main( void )
{
   // sample (gather) our texel colour for this UV
   vec4 diffuse = texture( gTexDiffuse, passUV ); 
   vec4 final_color = diffuse * passColor;

   if (final_color.a <= .5f) {
      discard; 
   }

   outColor = final_color * TINT; 
   
}