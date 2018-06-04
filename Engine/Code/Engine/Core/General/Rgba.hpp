#pragma once
#include "Engine\Math\Vector4.hpp"
#include "EngineCommon.hpp"



class Rgba
{
public:

	Rgba(); // Default-constructs to opaque white (255,255,255,255)

	explicit Rgba( unsigned char redByte, unsigned char greenByte,
				   unsigned char blueByte, unsigned char alphaByte=255 );

	void SetAsBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255 );

	void SetFromNormalizedFloats( float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha = 1.0f );
	void SetFromFloats(float theR, float theG, float theB, float theA = 255.f);

	void SetFromText( const char* text ); // e.g. from "255,255,255"
	void GetAsFloats( float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha ) const;
	void ScaleRGB( float rgbScale ); // Scales (and clamps) RGB components, but not A
	void ScaleAlpha( float alphaScale ); // Scales (and clamps) Alpha, RGB is untouched

	bool operator==( const Rgba& compare ) const;

	
	Vector4 GetAsVector4() const;
	Vector4 GetAsNormalizedVector4() const;
	Vector3 GetRGBNormalized() const;

public:
	unsigned char r; // Red byte, 0-255
	unsigned char g; // Green byte, 0-255
	unsigned char b; // Blue byte, 0-255
	unsigned char a; // Alpha (opacity) byte, 0-255; 0 is transparent, 255 is opaqu


	// SOME DEFAUL COLORS
	static const Rgba WHITE;
	static const Rgba BLACK; 
	static const Rgba CYAN;
	static const Rgba MAGENTA;
	
	// rainbow ;D
	static const Rgba RED;
	static const Rgba ORANGE;
	static const Rgba YELLOW;
	static const Rgba GREEN;
	static const Rgba BLUE;
	static const Rgba INDIGO;
	static const Rgba VIOLET;


};


// Helper stuff
bool RGBvsRGBA(Rgba rgbColor, Rgba rgbaColor);
Rgba GetRandomColor();

// gay af
Rgba GetRainbowColor(int idx, int total = 7);
Rgba GetNextColorInRainbow(Rgba& currentColor);
Rgba GetRandomColorInRainbow();
Rgba InterpolateRainbow(Rgba& currentColor, float percent);