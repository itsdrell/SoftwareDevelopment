#pragma once
#include <vector>

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================


//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Templates
//====================================================================================


//-----------------------------------------------------------------------------------------------
// Does Contain
template< typename T>
bool DoesContain( const T& item, const std::vector<T>& theVector )
{
	for(uint i = 0; i < theVector.size(); i++)
	{
		if(item == theVector.at(i))
			return true;
	}

	return false;
}


template< typename T>
bool DoesContain( const T& item, const std::vector<T*>& theVector )
{
	for(uint i = 0; i < theVector.size(); i++)
	{
		if(item == theVector.at(i))
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
// Calls delete on every element inside a vector and then clears
template< typename T>
void DeleteElementsInVector( std::vector<T*>& theVector )
{
	for(uint i = 0; i < theVector.size(); i++)
	{
		delete theVector.at(i);
		theVector.at(i) = nullptr;
	}

	theVector.clear();
}

//-----------------------------------------------------------------------------------------------
// a < b for modular/curcular variables. Does not work for floats
template< typename T>
bool CycleLessThanOrEqual( T a, T b )
{	
	static T const halfMax = (~(T)0) >> 1;
	T const diff = b - a;
	return (diff <= halfMax);
}

// since LessTahnorEqual covers those cases you can just swap the variables and return for greater!
template<typename T> inline bool CycleGreater(T a, T b) { return CycleLessThanOrEqual(b, a); }
//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [10/31/2018]
//====================================================================================