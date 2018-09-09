#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include <mutex>
#include <deque>
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
class SpinLock
{
public:
	SpinLock() {}

	void Enter() { m_lock.lock(); } // blocking operation
	bool TryToEnter() { return m_lock.try_lock(); }
	void Leave() { m_lock.unlock(); } // must be called if entered

public: 
	std::mutex m_lock; 
};



//====================================================================================
template <typename T>
class ThreadSafeQueue
{
public:
	void enqueue( T const &v )
	{
		m_lock.Enter();

		// I AM THE ONLY PERSON HERE
		m_data.push_back( v ); 

		m_lock.Leave();
		// no longer true...
	}


	// return if it succeeds
	bool dequeue( T *out_v ) 
	{
		m_lock.Enter();

		bool has_item = !m_data.empty();
		if (has_item) 
		{
			*out_v = m_data.front();
			
			//delete m_data.front(); // shouldnt be an issue
			m_data.pop_front(); 
		}

		m_lock.Leave();
		
		return has_item; 
	}

public:
	std::deque<T>			m_data; 
	SpinLock				m_lock; 
};

//====================================================================================
template <typename T>
class ThreadSafeVector
{
public:
	void Add( T const &v )
	{
		m_lock.Enter();

		// I AM THE ONLY PERSON HERE
		m_data.push_back( v ); 

		m_lock.Leave();
		// no longer true...
	}

	// return if it succeeds
	bool Remove( const T thingToRemove) 
	{
		m_lock.Enter();

		bool has_item = !m_data.empty();
		if (has_item) 
		{
			for(uint i = 0; i < m_data.size(); i++)
			{
				T current = m_data.at(i);

				if(current == thingToRemove)
				{
					m_data.erase(m_data.begin() + i);
					
					m_lock.Leave();
					return true;
				}
			}
		}

		m_lock.Leave();

		return has_item; 
	}

	bool DoesContain( const T theObject)
	{
		m_lock.Enter();

		bool has_item = !m_data.empty();
		if (has_item) 
		{
			for(uint i = 0; i < m_data.size(); i++)
			{
				T current = m_data.at(i);

				if(current == theObject)
				{
					m_lock.Leave();
					return true;
				}
			}
		}

		m_lock.Leave();

		return false; 
	}

	void Clear()
	{
		m_lock.Enter();

		m_data.clear();

		m_lock.Leave();
	}

	//-----------------------------------------------------------------------------------------------
	// You need to call lock outside of these before for looping!
	uint Size() { return (uint) m_data.size(); }
	T At(uint idx) { return m_data.at(idx); }
	
	// this is just remove fast but I had issues with the template compiling
	void Remove( uint& idx )
	{
		T endThing = m_data.at(m_data.size() - 1);

		// edge case of size 1
		if(m_data.size() == 1)
		{
			m_data.pop_back();
			idx--;
			return;
		}

		m_data.at(m_data.size() - 1) = m_data.at(idx);
		m_data.pop_back();
		m_data.at(idx) = endThing;
		idx--; // so we don't have to do it outside of the function
	}

	void Lock() { m_lock.Enter(); }
	void Unlock() { m_lock.Leave(); }

public:
	std::vector<T>			m_data; 
	SpinLock				m_lock; 
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [7/11/2018]
//====================================================================================