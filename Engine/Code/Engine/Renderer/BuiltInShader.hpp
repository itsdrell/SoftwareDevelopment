#pragma once

//////////////////////////////////////////////////////////////////////////
class Shader;


//////////////////////////////////////////////////////////////////////////
// NOTES
// These shaders do NOT support includes in the fs or vs since that requires reading a file
// and defeats the purpose. If there are includes, it will crash. 
//////////////////////////////////////////////////////////////////////////

class BuiltInShaders
{
public:

	static void CreateAllBuiltInShaders();

	static Shader* CreateDefaultShader();
	static Shader* CreateInvalidShader();


	// This is for the shader program to use if it has problems create a shader it can use the
	// invalid fs and vs source to create a new program
	static const char* GetInvalidFragment();
	static const char* GetInvalidVertex();


public:
};