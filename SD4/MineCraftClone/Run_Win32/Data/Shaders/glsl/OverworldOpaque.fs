#version 420 core

//----------------------------------------------------------------------------------------------- 
layout(binding = 0) uniform sampler2D gTexDiffuse;

uniform vec3 u_cameraPos;
uniform vec3 u_indoorLightRgb;
uniform vec3 u_outdoorLightRgb;
uniform vec3 u_skyColor;
uniform float u_fogNearDistance;
uniform float u_fogFarDistance;

//-----------------------------------------------------------------------------------------------
// We match the name and type of the previous stages out
in vec4 passColor; 
in vec2 passUV; 
in vec3 passWorldPosition;

//-----------------------------------------------------------------------------------------------
// Outputs
out vec4 outColor; 
out vec4 outColor2;

//-----------------------------------------------------------------------------------------------
void main( void )
{
   // sample (gather) our texel colour for this UV
   vec4 diffuse = texture( gTexDiffuse, passUV ); 
   
   float indoorLightLevel = passColor.r;
   float outdoorLightLevel = passColor.g;
   
   vec3 indoorLightRgb = indoorLightLevel * u_indoorLightRgb;
   vec3 outdoorLightRgb = outdoorLightLevel * u_outdoorLightRgb;
   vec3 lightRgb = max(indoorLightRgb, outdoorLightRgb); //?

   lightRgb.r = max(indoorLightRgb.r, outdoorLightRgb.r);
   lightRgb.g = max(indoorLightRgb.g, outdoorLightRgb.g);
   lightRgb.b = max(indoorLightRgb.b, outdoorLightRgb.b);

   vec4 shadedTexel = diffuse * vec4( lightRgb, passColor.a);

   // fog
   float dist = distance( u_cameraPos, passWorldPosition);
   float fogFraction = (dist - u_fogNearDistance) / (u_fogFarDistance - u_fogNearDistance);
   fogFraction = clamp( fogFraction, 0.0, 1.0);
   vec3 finalColor = mix( shadedTexel.rgb, u_skyColor, fogFraction);
   
   outColor = vec4( finalColor, shadedTexel.a);
   outColor2 = vec4( passUV.x, passUV.y, 0.0, 1.0);  
   //outColor = diffuse * passColor;
}