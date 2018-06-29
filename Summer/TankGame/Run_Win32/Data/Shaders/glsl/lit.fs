#version 420 core

#include "inc/common.glsl"
#include "inc/light.glsl"
#include "inc/debug.glsl"
#include "inc/fog.glsl"


// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal; 
layout(binding = 2) uniform sampler2D gTexEmissive;

// Attributes ============================================
in vec2 passUV; 
in vec4 passColor; 
in vec3 passViewPos;
in vec3 passWorldPos;   // new
in vec3 passWorldNormal;// new
in vec3 passWorldTangent; 
in vec3 passWorldBitangent;

out vec4 outColor; 

#define ADD(a, b) (a) + (b)

// Entry Point ===========================================
void main( void )
{
   // Color of this surface
   vec4 tex_color = texture( gTexDiffuse, passUV ) * passColor; 
   vec3 normal_color = texture( gTexNormal, passUV ).xyz;
   vec3 emissive_color = texture( gTexEmissive, passUV ).xyz; 


    // Interpolation is linear, so normals become not normal
   // over a surface, so renormalize it. 
   vec3 world_vnormal = normalize(passWorldNormal);

    // Get the surface to world matrix
   vec3 world_vtan = normalize(passWorldTangent); 
   vec3 world_vbitan = normalize(passWorldBitangent); 
   mat3 surface_to_world = mat3( world_vtan, world_vbitan, world_vnormal ); 

   // range map it to a surface normal
   vec3 surface_normal = normalize( normal_color * vec3( 2.0f, 2.0f, 1.0f ) + vec3( -1.0f, -1.0f, 0.0f ) ); 
   vec3 world_normal = surface_to_world * surface_normal; // tbn

   // used in final lighting equation to compute
   // final color of output - calculated from the light
   vec3 surface_light = vec3(0); // How much light is hitting the surface
   vec3 reflected_light = vec3(0);  // How much light is reflected back

   vec3 eye_dir = normalize( CAMERA_POSITION - passWorldPos ); 

   LightFactorT lf = CalculateLighting( passWorldPos, 
      eye_dir, 
      world_normal, 
      SPECULAR_AMOUNT, 
      SPECULAR_POWER ); 

   // mix for the debug modes. Think of these as if statements. 
   vec4 surface_color = mix( vec4(1, 1, 1, 1), tex_color, 1); //RENDER_MIX.x
   lf.diffuse = mix( vec3(0), lf.diffuse, 1); // RENDER_MIX.y
   lf.specular = mix( vec3(0), lf.specular,1); // RENDER_MIX.z

   // Add color of the lit surface to the reflected light
   // to get teh final color; 
   vec4 final_color = vec4(lf.diffuse, 1) * surface_color + vec4(lf.specular, 0); 
   final_color.xyz = ADD(final_color.xyz, emissive_color); 

   final_color = clamp(final_color, vec4(0), vec4(1) ); // not necessary - but overflow should go to bloom target (bloom effect)
   outColor = ApplyFog( final_color, passViewPos.z );
   

   // Debug Modes 
    #if defined(DEBUG_TANGENT)
        outColor = vec4( WorldNormalToColor( normalize(world_vtan) ), 1);
    #elif defined(DEBUG_BITANGENT)
        outColor = vec4( WorldNormalToColor( normalize(world_vbitan) ), 1);
    #elif defined(DEBUG_UV)
        outColor = vec4( passUV, 0, 1.0f );
    #elif defined(DEBUG_COLOR)
        outColor = tex_color; 
    #elif defined(DEBUG_SURFACE_NORMAL)
        outColor = vec4( normal_color, 1 );
    #elif defined(DEBUG_WORLD_NORMAL)
        outColor = vec4( WorldNormalToColor( normalize(world_normal) ), 1);
    #elif defined(DEBUG_DIFFUSE)
        outColor = vec4(lf.diffuse, 1);
    #elif defined(DEBUG_SPECULAR)
        outColor = vec4(lf.specular, 1);
    #elif defined(DEBUG_DIFFUSE_AND_SPEC)
        outColor = vec4(lf.diffuse, 1) + vec4(lf.specular, 0);
    #endif  
   
    //outColor = ApplyFog( vec4(1,0,0,1), passViewPos.z );

}