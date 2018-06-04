#pragma once
#include <string>
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Renderer/RendererTypes.hpp"


class Rgba;
class Vector2;
class IntRange;
class FloatRange;
class IntRange;
class IntVector2;
class AABB2;

int ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, int defaultValue );
char ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue );
bool ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue );
float ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue );
Rgba ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Rgba& defaultValue );
Vector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vector2& defaultValue );
IntRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntRange& defaultValue );
FloatRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const FloatRange& defaultValue );
IntVector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntVector2& defaultValue );
std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue );
Ints ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const Ints defaultValue);
AABB2 ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const AABB2 defaultValue);

// special case
std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const char* defaultValue=nullptr );


//////////////////////////////////////////////////////////////////////////
// RENDER TYPES
eCullMode ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, eCullMode defaultValue);
eFillMode ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, eFillMode defaultValue);
eWindOrder ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, eWindOrder defaultValue);
eBlendOperation ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, eBlendOperation defaultValue);
eBlendFactor ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, eBlendFactor defaultValue);
DepthBufferComparisons ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, DepthBufferComparisons defaultValue);


