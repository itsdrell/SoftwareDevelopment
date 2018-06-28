#pragma once

enum SamplerTypes
{
	NORMAL,
	MIPS,
	NUM_OF_SAMPLER_TYPES
};



class Sampler
{
public:
	Sampler();
	~Sampler();

	bool CreateDefault();
	bool CreateWithMips();
	void Destroy();

	void* GetHandle()const { return m_sampler_handle; }

public:
	void* m_sampler_handle;
};