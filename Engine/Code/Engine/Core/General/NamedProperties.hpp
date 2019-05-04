#pragma once
#include <string>
#include <map>

//====================================================================================
// Forward Declare
//====================================================================================
class NamedPropertyBase;

//====================================================================================
// Classes
//====================================================================================
class NamedProperties
{
public:
	NamedProperties() {}

public:

	//-----------------------------------------------------------------------------------------------
	template<typename T>
	void Set(const std::string& key, const T& value)		//this means that you have 0 to ???? different Set() functions on NamedProperties
	{
		auto theIterator = m_properties.find(key);

		if (theIterator == m_properties.end())
		{
			TypedProperty<T>* newProperty = new TypedProperty<T>(key, value);
			m_properties[key] = newProperty;
		}
		else
		{
			// could also replace or print a warning?
			// if you set with a type like double when it is a float you are fucked
			delete theIterator->second;
			TypedProperty<T>* newProperty = new TypedProperty<T>(key, value);
			m_properties[key] = newProperty;
		}
		
	}

	//-----------------------------------------------------------------------------------------------
	void Set(const std::string&key, const char* value)
	{
		Set<std::string>(key, std::string(value));
	}

	//-----------------------------------------------------------------------------------------------
	template<typename T>
	T Get(const std::string& key, T defaultValue) const
	{
		auto theIterator = m_properties.find(key);

		if (theIterator != m_properties.end())
		{
			TypedProperty<T>* theThing = (TypedProperty<T>*)theIterator->second;
			T value = theThing->Get<T>();

			if (theThing->IsType(typeid(value)))
			{
				return theThing->Get<T>();
			}
		}
		
		return defaultValue;
	}

	//-----------------------------------------------------------------------------------------------
	std::string NamedProperties::Get(const char* keyName, const char* defaultValue)
	{
		return Get<std::string>(std::string(keyName), std::string(defaultValue));
	}


private:
	std::map<std::string, NamedPropertyBase*>		m_properties;
};


//===============================================================================================
class NamedPropertyBase
{
public:
	NamedPropertyBase(const std::string& name)
		: m_name(name) {}

	virtual bool IsType(const type_info& typeInfo) = 0;

public:
	std::string		m_name;
};


//===============================================================================================
template <typename T>
class TypedProperty : public NamedPropertyBase
{
public:
	TypedProperty(const std::string& key, const T& theProperty)
		: NamedPropertyBase(key)
		, m_property(theProperty) {}

	//-----------------------------------------------------------------------------------------------
	template <typename T>
	T Get() const { return m_property; }

	//-----------------------------------------------------------------------------------------------
	bool IsType(const type_info& typeInfo) override
	{
		const type_info& myTypeInfo = typeid(m_property);
		return myTypeInfo == typeInfo;
	}

public:
	T				m_property;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [5/4/2019]
//====================================================================================