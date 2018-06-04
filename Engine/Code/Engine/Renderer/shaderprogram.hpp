#pragma once
#include "Engine/Renderer/glfunctions.h"
#include "Engine/Core/General/EngineCommon.hpp"


// Renderer/shaderprogram.h
class ShaderProgram
{
public:
	ShaderProgram();
	ShaderProgram(std::string name, const char* fs, const char* vs, Strings defines);
	~ShaderProgram();

	bool LoadFromFiles( char const *root , Strings defines); // load a shader from file
	bool LoadFromFiles(std::string name, std::string vertexPath, std::string fragmentPath, Strings defines);

public:
	GLuint program_handle; // OpenGL handle for this program, default 0
	std::string m_name;
};

static void LogShaderError(GLuint shader_id, const char* filename, uint defineOffset);
static void FormatAndPrintShaderErrors(std::string log, std::string path, uint defineOffset);
static uint AddIncludesToShaderProgram(std::string& shaderSource, std::string shaderPath);
static GLuint LoadShader( char const *filename, GLenum type, Strings defines);
static GLuint LoadShaderFromLiteral(const char*  shaderSource, GLenum type, Strings defines);
static GLuint CreateAndLinkProgram( GLint vs, GLint fs );
static void LogProgramError(GLuint program_id);