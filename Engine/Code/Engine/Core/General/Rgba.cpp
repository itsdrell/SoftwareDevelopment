#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Ranges/FloatRange.hpp"

//////////////////////////////////////////////////////////////////////////
// STATIC COLORS
const Rgba Rgba::WHITE = Rgba(255,255,255,255);
const Rgba Rgba::BLACK = Rgba(0,0,0,255);
const Rgba Rgba::CYAN = Rgba(0, 255, 255,255);
const Rgba Rgba::MAGENTA = Rgba(255,0,255);
const Rgba Rgba::RED = Rgba(255,0,0,255);
const Rgba Rgba::YELLOW = Rgba(255,255,0,255);
const Rgba Rgba::GREEN = Rgba(0,255,0,255);
const Rgba Rgba::BLUE = Rgba(0,0,255,255);

const Rgba Rgba::ORANGE = Rgba(255,165,0,255);
const Rgba Rgba::INDIGO = Rgba(75,0,130,255);
const Rgba Rgba::VIOLET = Rgba(138,43,226,255);

// rainbow ;D
const Rgba Rgba::RAINBOW_RED = Rgba(250,165,178,179);
const Rgba Rgba::RAINBOW_ORANGE = Rgba(255,206,161,255);
const Rgba Rgba::RAINBOW_YELLOW = Rgba(252,252,188,255);
const Rgba Rgba::RAINBOW_GREEN = Rgba(185,250,207,255);
const Rgba Rgba::RAINBOW_BLUE = Rgba(179,210,255,255);
const Rgba Rgba::RAINBOW_INDIGO = Rgba(214,174,247,255);
const Rgba Rgba::RAINBOW_VIOLET = Rgba(250,193,248,255);
//////////////////////////////////////////////////////////////////////////

Rgba::Rgba()
{

}

Rgba::Rgba(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetFromNormalizedFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha /*= 1.0f */)
{
	r = (unsigned char) RangeMapFloat(normalizedRed, 0.f, 1.f, 0.f, 255.f);
	g = (unsigned char) RangeMapFloat(normalizedGreen,  0.f, 1.f, 0.f, 255.f);
	b = (unsigned char) RangeMapFloat(normalizedBlue,  0.f, 1.f, 0.f, 255.f);
	a = (unsigned char) RangeMapFloat(normalizedAlpha,  0.f, 1.f, 0.f, 255.f);
}

void Rgba::SetFromFloats(float red, float green, float blue, float alpha )
{	
	r = (unsigned char) ClampFloat( red,	0.f, 255.f);
	g = (unsigned char) ClampFloat( green,	0.f, 255.f);
	b = (unsigned char) ClampFloat( blue,	0.f, 255.f);
	a = (unsigned char) ClampFloat( alpha,	0.f, 255.f);

}

void Rgba::SetFromText( const char* text )
{

	Strings result = SplitString(text,",");

	if((int)result.size() == 3)
	{
		r = (unsigned char)atof(result[0].c_str());
		g = (unsigned char)atof(result[1].c_str());
		b = (unsigned char)atof(result[2].c_str());
		a = 255;
	}
	else
	{
		r = (unsigned char)atof(result[0].c_str());
		g = (unsigned char)atof(result[1].c_str());
		b = (unsigned char)atof(result[2].c_str());
		a = (unsigned char)atof(result[3].c_str());
	}

}

void Rgba::GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const
{
	// These clamp references... but its const...?
	out_normalizedRed = ClampFloat(out_normalizedRed, 0.f, 255.f);
	out_normalizedGreen = ClampFloat(out_normalizedGreen, 0.f, 255.f);
	out_normalizedBlue = ClampFloat(out_normalizedBlue, 0.f, 255.f);
	out_normalizedAlpha = ClampFloat(out_normalizedAlpha, 0.f, 255.f);
}	

void Rgba::ScaleRGB(float rgbScale)
{
	// cast as float so we can add
	float rAsFloat = (float)r;
	float gAsFloat = (float)g;
	float bAsFloat = (float)b;

	//Add
	rAsFloat += rgbScale;
	gAsFloat += rgbScale;
	bAsFloat += rgbScale;
	
	//clamp
	rAsFloat = ClampFloat(rAsFloat, 0.f, 255.f);
	gAsFloat = ClampFloat(gAsFloat, 0.f, 255.f);
	bAsFloat = ClampFloat(bAsFloat, 0.f, 255.f);


	// Cast back
	r = static_cast<unsigned char>(rAsFloat);
	g = static_cast<unsigned char>(gAsFloat);
	b = static_cast<unsigned char>(bAsFloat);
	

}

void Rgba::ScaleAlpha(float alphaScale)
{
	float aAsFloat = (float)a;
	aAsFloat += alphaScale;
	aAsFloat = ClampFloat(aAsFloat, 0, 255);
	a = static_cast<unsigned char>(aAsFloat);
}

bool Rgba::operator==(const Rgba& compare) const
{
	if(this->r == compare.r)
	{
		if(this->g == compare.g)
		{
			if(this->b == compare.b)
			{
				if(this->a == compare.a)
				{
					return true;
				}
			}
		}
	}

	return false;
}

Vector4 Rgba::GetAsVector4() const
{
	float fRed = (float)r;
	float fGreen = (float)g;
	float fBlue = (float)b;
	float fAlpha = (float)a;
	
	return Vector4(fRed,fGreen,fBlue,fAlpha);
}

Vector4 Rgba::GetAsNormalizedVector4() const
{
	float fRed = (float)r;
	float fGreen = (float)g;
	float fBlue = (float)b;
	float fAlpha = (float)a;

	fRed = RangeMapFloat(fRed,0.f,255.f,0.f,1.f);
	fGreen = RangeMapFloat(fGreen,0.f,255.f,0.f,1.f);
	fBlue = RangeMapFloat(fBlue,0.f,255.f,0.f,1.f);
	fAlpha = RangeMapFloat(fAlpha,0.f,255.f,0.f,1.f);
	
	return Vector4(fRed,fGreen,fBlue,fAlpha);
}

Vector3 Rgba::GetRGBNormalized() const
{
	float fRed = (float)r;
	float fGreen = (float)g;
	float fBlue = (float)b;
	

	fRed = RangeMapFloat(fRed,0.f,255.f,0.f,1.f);
	fGreen = RangeMapFloat(fGreen,0.f,255.f,0.f,1.f);
	fBlue = RangeMapFloat(fBlue,0.f,255.f,0.f,1.f);
	

	return Vector3(fRed,fGreen,fBlue);
}

std::string Rgba::GetRGBString() const
{
	std::string result = std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b);
	return result;
}

bool RGBvsRGBA(Rgba rgbColor, Rgba rgbaColor)
{
	if(rgbColor.r == rgbaColor.r)
	{
		if(rgbColor.g == rgbaColor.g)
		{
			if(rgbColor.b == rgbaColor.b)
			{
				return true;
			}
		}
	}

	return false;
}

Rgba GetRandomColor()
{
	FloatRange range = FloatRange(0.f,255.f);
	
	unsigned char r = (unsigned char) range.GetRandomInRange();
	unsigned char g = (unsigned char) range.GetRandomInRange();
	unsigned char b = (unsigned char) range.GetRandomInRange();
	
	return Rgba(r,g,b, 255);
}

Rgba GetRainbowColor(int idx, int total /*= 7*/)
{
	float percent = (float) idx / (float) total;

	if(percent < (1.f/7.f))
		return Rgba::RAINBOW_RED;
	if(percent >= (1.f/7.f) && percent < (2.f/7.f))
		return Rgba::RAINBOW_ORANGE;
	if(percent >= (2.f/7.f) && percent < (3.f/7.f))
		return Rgba::RAINBOW_YELLOW;
	if(percent >= (3.f/7.f) && percent < (4.f/7.f))
		return Rgba::RAINBOW_GREEN;
	if(percent >= (4.f/7.f) && percent < (5.f/7.f))
		return Rgba::RAINBOW_BLUE;
	if(percent >= (5.f/7.f) && percent < (6.f/7.f))
		return Rgba::RAINBOW_INDIGO;
	if(percent >= (6.f/7.f))
		return Rgba::RAINBOW_VIOLET;

	return Rgba::WHITE;
}

Rgba GetNextColorInRainbow(Rgba & currentColor)
{

	if(currentColor == Rgba::RAINBOW_RED)
		return Rgba::RAINBOW_ORANGE;
	if(currentColor == Rgba::RAINBOW_VIOLET)
		return Rgba::RAINBOW_RED;
	if(currentColor == Rgba::RAINBOW_INDIGO)
		return Rgba::RAINBOW_VIOLET;
	if(currentColor == Rgba::RAINBOW_BLUE)
		return Rgba::RAINBOW_INDIGO;
	if(currentColor == Rgba::RAINBOW_GREEN)
		return Rgba::RAINBOW_BLUE;
	if(currentColor == Rgba::RAINBOW_YELLOW)
		return Rgba::RAINBOW_GREEN;
	if(currentColor == Rgba::RAINBOW_ORANGE)
		return Rgba::RAINBOW_YELLOW;
	
	return Rgba::WHITE;
}

Rgba GetRandomColorInRainbow()
{
	
	int d = GetRandomIntRange(0,6);

	if(d == 0)
		return Rgba::RAINBOW_RED;
	if(d == 1)
		return Rgba::RAINBOW_ORANGE;
	if(d == 2)
		return Rgba::RAINBOW_YELLOW;
	if(d == 3)
		return Rgba::RAINBOW_GREEN;
	if(d == 4)
		return Rgba::RAINBOW_BLUE;
	if(d == 5)
		return Rgba::RAINBOW_INDIGO;
	if(d == 6)
		return Rgba::RAINBOW_VIOLET;
	
	return Rgba::WHITE;
}

Rgba InterpolateRainbow(Rgba& currentColor, float percent)
{
	percent = ClampFloat(percent,0.f, 1.f);
	
	Rgba nextColor = GetNextColorInRainbow(currentColor);

	return Interpolate(currentColor, nextColor, percent);
}
