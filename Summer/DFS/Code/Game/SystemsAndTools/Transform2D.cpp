#include "Transform2D.hpp"

const Transform_T Transform_T::IDENTITY = Transform_T();

//=============================================================
Transform2D::Transform2D()
{
	m_localTransform = Transform_T::IDENTITY;

	m_parentTransform = nullptr;

}

Matrix44 Transform2D::GetLocalMatrix() const
{
	return m_localTransform.GetMatrix();
}

void Transform2D::SetLocalMatrix(const Matrix44 & mat)
{
	m_localTransform.SetMatrix(mat);
}

void Transform2D::SetLocalPosition(Vector2 pos)
{
	m_localTransform.SetPosition(pos);
}

void Transform2D::TranslateLocal(Vector2 offset)
{
	m_localTransform.Translate(offset);
}

Vector2 Transform2D::GetLocalPosition() const
{
	return m_localTransform.GetPosition();
}

void Transform2D::SetLocalRotation(float rotation)
{
	m_localTransform.SetRotation(rotation);
}

float Transform2D::GetLocalRotation() 
{
	return m_localTransform.GetRotation();
}

void Transform2D::SetLocalScale(const Vector2 & theScale)
{
	m_localTransform.SetScale(theScale);
}

Vector2 Transform2D::GetLocalScale() 
{
	return m_localTransform.GetScale();
}

Matrix44 Transform2D::GetWorldMatrix() const
{
	if(m_parentTransform == nullptr)
	{
		return GetLocalMatrix();
	}
	else
	{
		Matrix44 myMatrix = GetLocalMatrix();
		Matrix44 parentMatrix = m_parentTransform->GetWorldMatrix();

		parentMatrix.Append(myMatrix);

		return parentMatrix;
	}
}

Vector3 Transform2D::GetWorldPosition() const
{
	return GetWorldMatrix().GetPosition();
}


//=============================================================
Matrix44 Transform_T::GetMatrix() const
{
	Matrix44 result;

	Matrix44 translation = Matrix44::MakeTranslation2D(position);
	Matrix44 theRotation = Matrix44::MakeRotationDegrees2D(rotation);

	result.Append(translation);
	result.Append(theRotation);

	return result;
}

void Transform_T::SetMatrix(const Matrix44 & theMatrix)
{
	Matrix44 result = theMatrix;
	result.Invert();
}

void Transform_T::SetPosition(const Vector2 & pos)
{
	position = pos;
}

void Transform_T::Translate(const Vector2& offset)
{
	Vector2 currentPos = GetPosition();
	Vector2 newPos = currentPos + offset;

	SetPosition(newPos);
}

Vector2 Transform_T::GetPosition() const
{
	return position;
}

void Transform_T::SetRotation(float rotation)
{
	this->rotation = rotation;
}

float Transform_T::GetRotation()
{
	return rotation;
}

void Transform_T::SetScale(const Vector2 & s)
{
	scale = s;
}

Vector2 Transform_T::GetScale() const
{
	return scale;
}
