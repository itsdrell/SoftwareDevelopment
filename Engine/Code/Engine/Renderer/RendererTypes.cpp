#include "Engine/Renderer/RendererTypes.hpp"
#include "../Math/MathUtils.hpp"
#include "Systems/Lights.hpp"


GLenum ToGLCubeSide(eTextureCubeSide side)
{
	switch (side)
	{
	case TEXCUBE_RIGHT:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		break;
	case TEXCUBE_LEFT:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		break;
	case TEXCUBE_TOP:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		break;
	case TEXCUBE_BOTTOM:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		break;
	case TEXCUBE_FRONT:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		break;
	case TEXCUBE_BACK:
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		break;
	default:
		return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		break;
	}
}

GLenum ToGLCullMode(eCullMode mode)
{
	switch (mode)
	{
	case CULLMODE_BACK:
		return GL_BACK;
		break;
	case CULLMODE_FRONT:
		return GL_FRONT;
		break;
	case CULLMODE_NONE:
		return GL_NONE;
		break;
	default:
		return GL_BACK;
		break;
	}
	
}

GLenum ToGLFillMode(eFillMode mode)
{
	switch (mode)
	{
	case FILLMODE_SOLID:
		return GL_FILL;
		break;
	case FILLMODE_WIRE:
		return GL_LINE;
		break;
	default:
		return GL_FILL;
		break;
	}
	
}

GLenum ToGLWindOrder(eWindOrder order)
{
	switch (order)
	{
	case WIND_CLOCKWISE:
		return GL_CW;
		break;
	case WIND_COUNTER_CLOCKWISE:
		return GL_CCW;
		break;
	default:
		return GL_CCW;
		break;
	}
	
}

GLenum ToGLBlendOperation(eBlendOperation operation)
{
	switch (operation)
	{
	case COMPARE_ADD:
		return GL_FUNC_ADD;
		break;
	case COMPARE_SUBTRACT:
		return GL_FUNC_SUBTRACT;
		break;
	case COMPARE_REVERSE_SUBTRACT:
		return GL_FUNC_REVERSE_SUBTRACT;
		break;
	case COMPARE_MIN:
		return GL_MIN;
		break;
	case COMPARE_MAX:
		return GL_MAX;
		break;
	default:
		return GL_FUNC_ADD;
		break;
	}
	
}

GLenum ToGLBlendFactor(eBlendFactor factor)
{
	
	switch (factor)
	{
	case BLEND_ZERO:
		return GL_ZERO;
		break;
	case BLEND_ONE:
		return GL_ONE;
		break;
	case BLEND_SRC_COLOR:
		return GL_SRC_COLOR;
		break;
	case BLEND_ONE_MINUS_SRC_COLOR:
		return GL_ONE_MINUS_SRC_COLOR;
		break;
	case BLEND_DST_COLOR:
		return GL_DST_COLOR;
		break;
	case BLEND_ONE_MINUS_DST_COLOR:
		return GL_ONE_MINUS_DST_ALPHA;
		break;
	case BLEND_SRC_ALPHA:
		return GL_SRC_ALPHA;
		break;
	case BLEND_ONE_MINUS_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
		break;
	case BLEND_DST_ALPHA:
		return GL_DST_ALPHA;
		break;
	case BLEND_ONE_MINUS_DST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
		break;
	case BLEND_CONSTANT_COLOR:
		return GL_CONSTANT_COLOR;
		break;
	case BLEND_ONE_MINUS_CONSTANT_COLOR:
		return GL_ONE_MINUS_CONSTANT_COLOR;
		break;
	case BLEND_CONSTANT_ALPHA:
		return GL_CONSTANT_ALPHA;
		break;
	case BLEND_ONE_MINUS_CONSTANT_ALPHA:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
		break;
	case BLEND_SRC_ALPHA_SATURATE:
		return GL_SRC_ALPHA_SATURATE;
		break;
	default:
		return GL_ZERO;
		break;
	}
	
}

GLenum ToGLDepthBufferCompare(DepthBufferComparisons compare)
{
	switch (compare)
	{
	case COMPARE_NEVER:
		return GL_NEVER;
		break;
	case COMPARE_LESS:
		return GL_LESS;
		break;
	case COMPARE_LEQUAL:
		return GL_LEQUAL;
		break;
	case COMPARE_GREATER:
		return GL_GREATER;
		break;
	case COMPARE_GEQUAL:
		return GL_GEQUAL;
		break;
	case COMPARE_EQUAL:
		return GL_EQUAL;
		break;
	case COMPARE_NOT_EQUAL:
		return GL_NOTEQUAL;
		break;
	case COMPARE_ALWAYS:
		return GL_ALWAYS;
		break;
	default:
		return GL_ALWAYS;
		break;
	}
}

GLenum ToGLType(eRenderDataType type)
{
	switch (type)
	{
	case RENDER_DATA_TYPE_FLOAT:
		return GL_FLOAT;
		break;
	case RENDER_DATA_TYPE_UNSIGNED_BYTE:
		return GL_UNSIGNED_BYTE;
		break;
	default:
		return GL_FLOAT;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
Vertex3D_PCU::Vertex3D_PCU(Vector3 thePosition, Rgba theColor, Vector2 UVcoords)
{
	m_position = thePosition;
	m_color = theColor;
	m_uvTexCoords = UVcoords;
}

Vertex3D_PCU::Vertex3D_PCU(const VertexMaster& other)
{
	m_position = other.m_position;
	m_color = other.m_color;
	m_uvTexCoords = other.m_uvTexCoords;
}

VertexAttributeT const Vertex3D_PCU::ATTRIBUTES[] = 
{
	VertexAttributeT( "POSITION", RENDER_DATA_TYPE_FLOAT,         3, false, offsetof(Vertex3D_PCU, m_position) ),
	VertexAttributeT( "COLOR",    RENDER_DATA_TYPE_UNSIGNED_BYTE, 4, true,  offsetof(Vertex3D_PCU, m_color) ),
	VertexAttributeT( "UV",       RENDER_DATA_TYPE_FLOAT,         2, false, offsetof(Vertex3D_PCU, m_uvTexCoords) ),
	VertexAttributeT(), // like a null terminator, how do we know the list is done;
}; 


VertexLayout Vertex3D_PCU::LAYOUT = VertexLayout( sizeof(Vertex3D_PCU), Vertex3D_PCU::ATTRIBUTES );


//////////////////////////////////////////////////////////////////////////
VertexLit::VertexLit(const Vector3 & thePosition, Rgba & theColor, Vector2 & UVCoords, Vector3 & theNormal, Vector3 & theTangent, Vector3& theBiTangent)
{
	m_position = thePosition;
	m_color = theColor;
	m_uvTexCoords = UVCoords;

	m_normal = theNormal;
	m_tangent = theTangent;
	m_biTangent = theBiTangent;
}

VertexLit::VertexLit(const VertexMaster& other)
{
	m_position = other.m_position;
	m_color = other.m_color;
	m_uvTexCoords = other.m_uvTexCoords;

	m_normal = other.m_normal;
	m_tangent = other.m_tangent;
	m_biTangent = other.m_biTangent;
}

VertexAttributeT const VertexLit::ATTRIBUTES[] = 
{
	VertexAttributeT( "POSITION",	RENDER_DATA_TYPE_FLOAT,         3, false, offsetof(VertexLit, m_position) ),
	VertexAttributeT( "COLOR",		RENDER_DATA_TYPE_UNSIGNED_BYTE, 4, true,  offsetof(VertexLit, m_color) ),
	VertexAttributeT( "UV",			RENDER_DATA_TYPE_FLOAT,         2, false, offsetof(VertexLit, m_uvTexCoords) ),
	VertexAttributeT( "NORMAL",		RENDER_DATA_TYPE_FLOAT,			3, false, offsetof(VertexLit, m_normal)),
	VertexAttributeT( "TANGENT",	RENDER_DATA_TYPE_FLOAT,			3, false, offsetof(VertexLit, m_tangent)),
	VertexAttributeT( "BITANGENT",  RENDER_DATA_TYPE_FLOAT,			3, false, offsetof(VertexLit, m_biTangent)),
	VertexAttributeT(), // like a null terminator, how do we know the list is done;
}; 


VertexLayout VertexLit::LAYOUT = VertexLayout(sizeof(VertexLit), VertexLit::ATTRIBUTES);


//////////////////////////////////////////////////////////////////////////
LightT::LightT(Light* fromLight)
{
	m_position = fromLight->m_position;
	m_attenuation = fromLight->m_attenuation;
	m_colorAndIntensity = Vector4(fromLight->m_color.GetRGBNormalized(), fromLight->m_intensity);
	m_direction = fromLight->m_direction;
	m_directionFactor = fromLight->m_directionFactor;
	
	// this may change later
	m_specAttenuation = m_attenuation;

	m_dotInnerAngle = CosDegrees(fromLight->m_innerAngle);
	m_dotOuterAngle = CosDegrees(fromLight->m_outerAngle);
	
}

void LightT::SetUpForPointLight(Vector3 position, float intesity, Vector3 attenConstants /*= Vector3(0.f, 0.f, 1.f)*/, Rgba theColor)
{
	m_position = position;
	m_attenuation = attenConstants;
	m_colorAndIntensity = Vector4(theColor.GetRGBNormalized(), intesity);

	// just making sure it is unit length
	m_direction = Vector3(1.f,0.f,0.f);
	m_directionFactor = 0;

	// we can just set them to the same
	m_specAttenuation = attenConstants;
	
	m_dotInnerAngle = CosDegrees(360.f * .5f);
	m_dotOuterAngle = CosDegrees(360.f * .5f);

}

void LightT::SetUpForSpotLight(Vector3 position, Vector3& direction, float intesity, float innerAngle, float outerAngle, Vector3 attenConstants /*= Vector3(0.f,0.f,1.f)*/, Rgba theColor /*= Rgba::WHITE */)
{
	m_position = position;
	m_attenuation = attenConstants;
	m_colorAndIntensity = Vector4(theColor.GetRGBNormalized(), intesity);

	// just making sure it is unit length
	m_direction = direction;
	m_directionFactor = 1.f;

	// we can just set them to the same
	m_specAttenuation = attenConstants;

	// need to take the cos(degrees / 2)
	m_dotInnerAngle = CosDegrees(innerAngle * .5f); 
	m_dotOuterAngle = CosDegrees(outerAngle * .5f);
}

void LightT::SetUpForDirectionalLight(Vector3 position, Vector3 direction, float intesity, Vector3 attenConstants /*= Vector3(0.f,0.f,1.f)*/, Rgba theColor /*= Rgba::WHITE */)
{
	m_position = position;
	m_attenuation = attenConstants;
	m_colorAndIntensity = Vector4(theColor.GetRGBNormalized(), intesity);

	// just making sure it is unit length
	m_direction = direction;
	m_directionFactor = 1;

	// we can just set them to the same
	m_specAttenuation = attenConstants;

	m_dotInnerAngle = CosDegrees(180.f * .5f);
	m_dotOuterAngle = CosDegrees(180.f * .5f);
}
