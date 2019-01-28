#version 420 core

#include "inc/time.glsl"

// We match the name and type of the previous stages out
in vec4 passColor; // NEW, passed color
in vec2 passUV; 

// Outputs
out vec4 outColor; 

// Binding Texture Slots - saying we expect Diffuse to be at 0
layout(binding = 0) uniform sampler2D gTexColor;
layout(binding = 1) uniform sampler2D gTexDepth; 

// Entry Point
void main( void )
{ 
   // Page 242 Graphic Shaders Theory and Practice book
   const vec3 W = vec3(.2125, .7154, .0721);

   vec3 irgb = texture(gTexColor, passUV).rgb;
   float luminance = dot(irgb, W);

   float l = (sin(SYSTEM_TIME) + 1) * .25;
   vec4 g = vec4(luminance,luminance,luminance,1.);

   outColor = mix(g,vec4(irgb,1),l);

   //outColor = vec4(irgb.r,0,0,1);
   // This may not work because this is using luminance and my default texture is white
}