#pragma once
#include "Engine/Renderer/external/gl/glcorearb.h"
#include "Engine/Renderer/external/gl/glext.h"
#include "Engine/Renderer/external/gl/wglext.h"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"


// declare the variables (PFN + Function Name + Proc)
// gl
extern PFNGLCLEARPROC glClear;
extern PFNGLCLEARCOLORPROC glClearColor;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLDETACHSHADERPROC glDetachShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLDRAWARRAYSPROC glDrawArrays;
extern PFNGLLINEWIDTHPROC glLineWidth;
extern PFNGLENABLEPROC glEnable;
extern PFNGLBLENDFUNCPROC glBlendFunc;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGENSAMPLERSPROC glGenSamplers;
extern PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
extern PFNGLDELETESAMPLERSPROC glDeleteSamplers;
extern PFNGLBINDSAMPLERPROC glBindSampler;
extern PFNGLACTIVETEXTUREARBPROC glActiveTexture;
extern PFNGLBINDTEXTUREPROC glBindTexture;
extern PFNGLPIXELSTOREIPROC glPixelStorei;
extern PFNGLGENTEXTURESPROC glGenTextures;
extern PFNGLTEXPARAMETERIPROC glTexParameteri;
extern PFNGLTEXIMAGE2DPROC glTexImage2D;
extern PFNGLCLEARDEPTHPROC glClearDepth;
extern PFNGLDEPTHFUNCPROC glDepthFunc;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
extern PFNGLDRAWBUFFERSPROC glDrawBuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus; 
extern PFNGLGETERRORPROC glGetError;
extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
extern PFNGLDEPTHMASKPROC glDepthMask;
extern PFNGLGETTEXIMAGEPROC glGetTexImage;
extern PFNGLBINDBUFFERBASEPROC glBindBufferBase;
extern PFNGLDRAWELEMENTSPROC glDrawElements;
extern PFNGLPOLYGONMODEPROC glPolygonMode;
extern PFNGLDISABLEPROC glDisable;
extern PFNGLCULLFACEPROC glCullFace;
extern PFNGLFRONTFACEPROC glFrontFace;
extern PFNGLBLENDFUNCIPROC glBlendFunci;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
extern PFNGLUNIFORM1FVPROC glUniform2fv;
extern PFNGLUNIFORM1FVPROC glUniform3fv;
extern PFNGLUNIFORM1FVPROC glUniform4fv;
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
extern PFNGLTEXSTORAGE2DPROC glTexStorage2D;
extern PFNGLDELETETEXTURESPROC glDeleteTextures;
extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
extern PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;

// wgl
extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;


// Binding macro
#define GL_BIND_FUNCTION(f)      wglGetTypedProcAddress( &f, #f )

// Use this to deduce type of the pointer so we can cast; 
template <typename T>
bool wglGetTypedProcAddress( T *out, char const *name ) 
{
	// Grab the function from the currently bound render contect
	// most opengl 2.0+ features will be found here
	*out = (T) wglGetProcAddress(name); 

	if ((*out) == nullptr) {
		// if it is not part of wgl (the device), then attempt to get it from the GLL library
		// (most OpenGL functions come from here)
		*out = (T) GetProcAddress( gGLLibrary, name); 
	}

	return (*out != nullptr); 
}

//////////////////////////////////////////////////////////////////////////
#define GL_CHECK_ERROR()  GLCheckError( __FILE__, __LINE__ )
bool GLCheckError( char const *file, int line );
bool GLFailed();
bool GLSucceeded();

