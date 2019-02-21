#include "Engine/Renderer/glfunctions.h"
#include "Engine/Core/Utils/StringUtils.hpp"

// These must be null, we will bind them on RenderStartup
// gl
PFNGLCLEARPROC glClear = nullptr;
PFNGLCLEARCOLORPROC glClearColor = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLDETACHSHADERPROC glDetachShader = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLDRAWARRAYSPROC glDrawArrays = nullptr;
PFNGLLINEWIDTHPROC glLineWidth = nullptr;
PFNGLENABLEPROC glEnable = nullptr;
PFNGLBLENDFUNCPROC glBlendFunc = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLGENSAMPLERSPROC glGenSamplers = nullptr;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = nullptr;
PFNGLDELETESAMPLERSPROC glDeleteSamplers = nullptr;
PFNGLBINDSAMPLERPROC glBindSampler = nullptr;
PFNGLACTIVETEXTUREARBPROC glActiveTexture = nullptr;
PFNGLBINDTEXTUREPROC glBindTexture = nullptr;
PFNGLPIXELSTOREIPROC glPixelStorei = nullptr;
PFNGLGENTEXTURESPROC glGenTextures = nullptr;
PFNGLTEXPARAMETERIPROC glTexParameteri = nullptr;
PFNGLTEXIMAGE2DPROC glTexImage2D = nullptr;
PFNGLCLEARDEPTHPROC glClearDepth = nullptr;
PFNGLDEPTHFUNCPROC glDepthFunc = nullptr;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = nullptr;
PFNGLDRAWBUFFERSPROC glDrawBuffers = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr; 
PFNGLGETERRORPROC glGetError = nullptr;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = nullptr;
PFNGLDEPTHMASKPROC glDepthMask = nullptr;
PFNGLGETTEXIMAGEPROC glGetTexImage = nullptr;
PFNGLBINDBUFFERBASEPROC glBindBufferBase = nullptr;
PFNGLDRAWELEMENTSPROC glDrawElements = nullptr;
PFNGLPOLYGONMODEPROC glPolygonMode = nullptr;
PFNGLDISABLEPROC glDisable = nullptr;
PFNGLCULLFACEPROC glCullFace = nullptr;
PFNGLFRONTFACEPROC glFrontFace = nullptr;
PFNGLBLENDFUNCIPROC glBlendFunci = nullptr;
PFNGLUNIFORM1FVPROC glUniform1fv = nullptr;
PFNGLUNIFORM1FVPROC glUniform2fv = nullptr;
PFNGLUNIFORM1FVPROC glUniform3fv = nullptr;
PFNGLUNIFORM1FVPROC glUniform4fv = nullptr;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = nullptr;
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = nullptr;
PFNGLTEXSTORAGE2DPROC glTexStorage2D = nullptr;
PFNGLDELETETEXTURESPROC glDeleteTextures = nullptr;
PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = nullptr;

// wgl
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
bool GLCheckError( char const *file, int line )
{
#if defined(_DEBUG)
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		DebuggerPrintf( "\nGL ERROR [0x%04x] at [%s(%i)] \n", error, file, line );
		return true; 
	}
#else
	UNUSED(file);
	UNUSED(line);
#endif
	return false; 
}

//------------------------------------------------------------------------
bool GLFailed()
{
	return GL_CHECK_ERROR(); 
}

//------------------------------------------------------------------------
bool GLSucceeded()
{
	return !GLFailed();
}

//--------------------------------------------------------------------------
void GLCheckErrorAndDie( char const *file, char const* function, int line )
{
#if defined(_DEBUG)
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		ERROR_RECOVERABLE( Stringf("\nGL ERROR [0x%04x] in [%s] at [%s(%i)] \n", error, function, file, line).c_str() );
	}
#else
	UNUSED(file);
	UNUSED(function);
	UNUSED(line);
#endif
}
