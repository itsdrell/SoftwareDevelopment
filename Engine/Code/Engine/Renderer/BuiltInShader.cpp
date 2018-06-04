#include "Engine/Renderer/BuiltInShader.hpp"
#include "RenderableComponents/Shader.hpp"
#include "RenderableComponents/shaderprogram.hpp"
#include "Renderer.hpp"


void BuiltInShaders::CreateAllBuiltInShaders()
{
	Shader::AddShader("default", CreateDefaultShader());
	Shader::AddShader("invalid", CreateDefaultShader());
}

Shader* BuiltInShaders::CreateDefaultShader()
{

#pragma region FragmentAndVertexCode
	
	//////////////////////////////////////////////////////////////////////////
	// FS
	const char* fs = R"(#version 420 core

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
   outColor = diffuse * passColor;
})";

	//////////////////////////////////////////////////////////////////////////
	// VS
	const char* vs = R"(// define the shader version (this is required)
#version 420 core

// Frame information ;
layout(binding=1, std140) uniform cFrameBlock 
{
   float GAME_TIME;
   float GAME_DELTA_TIME;
   float SYSTEM_TIME; 
   float SYSTEM_DELTA_TIME; 
}; 

// Camera Constants
layout(binding=2, std140) uniform cCameraBlock 
{
    mat4 VIEW;
    mat4 PROJECTION; 
    mat4 VIEW_PROJECTION; 

    mat4 INV_VIEW;          // also just called the camera matrix
    mat4 INV_PROJECTION; 
    mat4 INT_VIEW_PROJECTION; 

    // dupliate of INV_VIEW, but just convenient
    vec3 CAMERA_RIGHT;      float pad0;
    vec3 CAMERA_UP;         float pad1; 
    vec3 CAMERA_FORWARD;    float pad2; 
    vec3 CAMERA_POSITION;   float pad3;  

}; 

uniform mat4 MODEL;


// Attributes - input to this shasder stage (constant as far as the code is concerned)
in vec3 POSITION;
in vec4 COLOR;       // NEW - GLSL will use a Vector4 for this;
in vec2 UV;          // new


//pass variables or "varying" variables
// These are variables output from this stage for use by the next stage
// These are called "varying" because for a triangle, we'll only output 3, and 
// the Raster stage may output hundreds of pixels, each a blended value
// of the initial 3, hence - they "vary"
out vec2 passUV; 
out vec4 passColor;  // NEW - to use it in the pixel stage, we must pass it.

// Entry point - required.  What does this stage do?
void main( void )
{
   // multiply it through - for now, local_pos
   // is being treated as view space position
   // this will be updated later once we introduce the other matrices
    vec4 local_pos = vec4( POSITION, 1 ); 
    vec4 clip_pos =  PROJECTION * (VIEW * local_pos); 
   
    passColor = COLOR; // pass it on. 
    passUV = UV; // we have to set our outs.

    gl_Position = clip_pos;  // changed
})";

#pragma endregion

	Shader* newShader = new Shader();

	//////////////////////////////////////////////////////////////////////////
	// Create shader program
	Strings theDefines;
	//theDefines.push_back("hi");

	newShader->SetProgram(new ShaderProgram("default", fs,vs, theDefines));
	
	//////////////////////////////////////////////////////////////////////////
	// Add all properties
	newShader->m_textures.push_back(Renderer::GetInstance()->CreateOrGetTexture("Data/Images/defaultTexture.png"));
	
	return newShader;
}

Shader* BuiltInShaders::CreateInvalidShader()
{
	Shader* newShader = new Shader();

	//////////////////////////////////////////////////////////////////////////
	// Create shader program
	Strings theDefines;

	newShader->SetProgram(new ShaderProgram("default", GetInvalidFragment(),GetInvalidVertex(), theDefines));

	//////////////////////////////////////////////////////////////////////////
	// Add all properties
	newShader->m_textures.push_back(Renderer::GetInstance()->CreateOrGetTexture("Data/Images/defaultTexture.png"));

	return newShader;
}

const char* BuiltInShaders::GetInvalidFragment()
{
	const char* fs = R"(#version 420 core

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
   //passColor = vec4(1.0, 0.0, 1.0, 1.0);
   outColor = vec4(1.0, 0.0, 1.0, 1.0);
})";
	
	return fs;
}

const char* BuiltInShaders::GetInvalidVertex()
{
	const char* vs = R"(// define the shader version (this is required)
#version 420 core

// Frame information ;
layout(binding=1, std140) uniform cFrameBlock 
{
   float GAME_TIME;
   float GAME_DELTA_TIME;
   float SYSTEM_TIME; 
   float SYSTEM_DELTA_TIME; 
}; 

// Camera Constants
layout(binding=2, std140) uniform cCameraBlock 
{
    mat4 VIEW;
    mat4 PROJECTION; 
    mat4 VIEW_PROJECTION; 

    mat4 INV_VIEW;          // also just called the camera matrix
    mat4 INV_PROJECTION; 
    mat4 INT_VIEW_PROJECTION; 

    // dupliate of INV_VIEW, but just convenient
    vec3 CAMERA_RIGHT;      float pad0;
    vec3 CAMERA_UP;         float pad1; 
    vec3 CAMERA_FORWARD;    float pad2; 
    vec3 CAMERA_POSITION;   float pad3;  

}; 

uniform mat4 MODEL;


// Attributes - input to this shasder stage (constant as far as the code is concerned)
in vec3 POSITION;
in vec4 COLOR;       // NEW - GLSL will use a Vector4 for this;
in vec2 UV;          // new


//pass variables or "varying" variables
// These are variables output from this stage for use by the next stage
// These are called "varying" because for a triangle, we'll only output 3, and 
// the Raster stage may output hundreds of pixels, each a blended value
// of the initial 3, hence - they "vary"
out vec2 passUV; 
out vec4 passColor;  // NEW - to use it in the pixel stage, we must pass it.

// Entry point - required.  What does this stage do?
void main( void )
{
   // multiply it through - for now, local_pos
   // is being treated as view space position
   // this will be updated later once we introduce the other matrices
    vec4 local_pos = vec4( POSITION, 1 ); 
    vec4 clip_pos =  PROJECTION * (VIEW * local_pos); 
   
    passColor = COLOR; // pass it on. 
    passUV = UV; // we have to set our outs.

    gl_Position = clip_pos;  // changed
})";
	
	return vs;
}

