
//--------------------------------------------------------------------------------------
// FOG
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// DEFINES
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// defined in common.glsl
uniform vec4 FOG_COLOR; 
uniform float FOG_NEAR_PLANE;
uniform float FOG_FAR_PLANE;
uniform float FOG_NEAR_FACTOR;
uniform float FOG_FAR_FACTOR;

//--------------------------------------------------------------------------------------
// FUNCTIONS
//--------------------------------------------------------------------------------------

vec4 ApplyFog( vec4 color, float view_depth )
{
   
   #if defined(FOG)
      float fog_factor = smoothstep( FOG_NEAR_PLANE, FOG_FAR_PLANE, view_depth ); 
      fog_factor = FOG_NEAR_FACTOR + (FOG_FAR_FACTOR - FOG_NEAR_FACTOR) * fog_factor; 
      color = mix( color, FOG_COLOR, fog_factor );  
      //color = vec4(1,0,0,1); 
   #endif

   return color; 
}