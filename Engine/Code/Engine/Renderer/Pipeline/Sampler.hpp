#pragma once

class Sampler
{
public:
	Sampler();
	~Sampler();

	bool Create();
	void Destroy();

	void* GetHandle()const { return m_sampler_handle; }

public:
	void* m_sampler_handle;
};