#version 420 core


// Suggest always manually setting bindings - again, consitancy with 
// other rendering APIs and well as you can make assumptions in your
// engine without having to query
layout(binding = 0) uniform samplerCube gTexSky;
//layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec3 passWorldPosition; 
in vec4 passColor; 
//in vec2 passUV; 

out vec4 outColor; 

void main( void )
{
    // sample (gather) our texel colour for this UV
   //vec4 diffuse = texture( gTexSky, passUV ); 
   //outColor = diffuse * passColor;

   vec3 normal = normalize(passWorldPosition); 
   vec4 tex_color = textureLod( gTexSky, normal , 0); 
   // outColor = vec4((normal + vec3(1)) * .5f, 1f);
   //outColor = tex_color * passColor;
   outColor = tex_color;
   //outColor = mix(tex_color, vec4(1.f,1.f,1.f,1.f), .5f);
   //outColor = vec4(1.f,0.f,0.f,1.f);
}
