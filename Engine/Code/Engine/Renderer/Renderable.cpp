#include "Renderable.hpp"

Renderable::~Renderable()
{
	TODO("Do this pls");
}

void Renderable::SetPosition(const Vector3& position)
{
	m_modelMatrix = Matrix44::MakeTranslation3D(position);
}

bool Renderable::UsesLight()
{
	// this is probably supposed to be something else
	return m_usesLight;
}

