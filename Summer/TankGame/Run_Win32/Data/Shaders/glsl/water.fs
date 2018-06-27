#version 420 core

#include "inc/common.glsl"



// Suggest always manually setting bindings - again, consitancy with 
// other rendering APIs and well as you can make assumptions in your
// engine without having to query
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec3 passViewPos;
in vec3 passWorldPos;
in vec3 passWorldNormal;
in vec3 passWorldTangent; 
in vec3 passWorldBitangent; 

in vec2 passUV; 
in vec4 passColor; 

layout(location = 0) out vec4 outColor; 
//layout(location = 1) out vec4 outBloom; 

void main( void )
{
   // Get the surface colour
   vec2 uv_offset = passUV + vec2(GAME_TIME * .1f); 
   vec4 tex_color = texture( gTexDiffuse, passUV + uv_offset  ) * passColor; 

   // range map it to a surface normal
  // vec3 world_normal = normalize(passWorldNormal);
   //vec3 eye_dir = normalize( CAMERA_POSITION - passWorldPos ); 

   //light_factor_t lf = CalculateLighting( passWorldPos, 
   //   eye_dir, 
   //   world_normal, 
   //   SPEC_FACTOR, 
   //   SPEC_POWER ); 

   // Add color of the lit surface to the reflected light
   // to get teh final color; 
   //vec4 final_color = vec4(lf.diffuse, 1) * tex_color + vec4(lf.specular, 0);
   //outColor = clamp( final_color, vec4(0), vec4(1) ); // not necessary - but overflow should go to bloom target (bloom effect)
   
   //outColor = ApplyFog( outColor, passViewPos.z );
   outColor = tex_color;
}
