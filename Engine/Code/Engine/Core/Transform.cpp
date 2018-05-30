#include "Transform.hpp"

//////////////////////////////////////////////////////////////////////////
const transform_t transform_t::IDENTITY = transform_t();

//////////////////////////////////////////////////////////////////////////

// Transform::Transform(const Vector3 thePosition, const Vector3 theRotation, const Vector3 theScale)
// {
// 	position = thePosition;
// 	rotation = theRotation;
// 	scale = theScale;
// }
// 
// Matrix44 Transform::GetMatrix()
// {
// 	Matrix44 result;
// 	
// 	// Get the matrix for all the things
// 	Matrix44 rotationAroundX = Matrix44::MakeRotationDegreesAroundX(rotation.x);
// 	Matrix44 rotationAroundY = Matrix44::MakeRotationDegreesAroundY(rotation.y);
// 	Matrix44 rotationAroundZ = Matrix44::MakeRotationDegreesAroundZ(rotation.z);
// 
// 	Matrix44 translation = Matrix44::MakeTranslation3D(position);
// 	
// 	result.Append(translation);
// 
// 	// All the appends
// 	result.Append(rotationAroundY);
// 	result.Append(rotationAroundX);
// 	result.Append(rotationAroundZ);
// 
// 	
// 	return result;
// }

//////////////////////////////////////////////////////////////////////////
Matrix44 transform_t::GetMatrix() const
{
	Matrix44 result;



	Matrix44 translation = Matrix44::MakeTranslation3D(position);
	
	// Get the matrix for all the things
	Matrix44 rotationAroundX = Matrix44::MakeRotationDegreesAroundX(euler.x);
	Matrix44 rotationAroundY = Matrix44::MakeRotationDegreesAroundY(euler.y);
	Matrix44 rotationAroundZ = Matrix44::MakeRotationDegreesAroundZ(euler.z);


	result.Append(translation);

	// All the appends
	result.Append(rotationAroundY);
	result.Append(rotationAroundX);
	result.Append(rotationAroundZ);


	return result;
}

void transform_t::SetMatrix(Matrix44 const & mat)
{
	Matrix44 result = mat;
	result.Invert();
}

void transform_t::SetPosition(Vector3 pos)
{
	position = pos;
}

void transform_t::Translate(Vector3 offset)
{
	offset.x;
}

Vector3 transform_t::GetPosition() const
{
	return position;
}

void transform_t::SetRotationEuler(Vector3 theEuler)
{
	this->euler = theEuler;
}

void transform_t::RotateByEuler(Vector3 theEuler)
{
	euler.x;
}

Vector3 transform_t::GetEulerAngles() const
{
	return euler;
}

void transform_t::SetScale(Vector3 s)
{
	scale = s;
}

Vector3 transform_t::GetScale() const
{
	return scale;
}

//////////////////////////////////////////////////////////////////////////
Transform::Transform()
{
	m_local_transform = transform_t::IDENTITY; 
	
	m_parentTransform = nullptr;
}

Matrix44 Transform::GetLocalMatrix() const
{
	return m_local_transform.GetMatrix();
}

void Transform::SetLocalMatrix(Matrix44 const & mat)
{
	m_local_transform.SetMatrix(mat);
}

void Transform::SetLocalPosition(Vector3 pos)
{
	m_local_transform.SetPosition(pos);
}

void Transform::TranslateLocal(Vector3 offset)
{
	m_local_transform.Translate(offset);
}

Vector3 Transform::GetLocalPosition() const
{
	return m_local_transform.GetPosition();
}

void Transform::SetLocalRotationEuler(Vector3 euler)
{
	m_local_transform.SetRotationEuler(euler);
}

void Transform::RotateLocalByEuler(Vector3 euler)
{
	m_local_transform.RotateByEuler(euler);
}

Vector3 Transform::GetLocalEulerAngles() const
{
	return m_local_transform.GetEulerAngles();
}

void Transform::SetLocalScale(Vector3 s)
{
	m_local_transform.SetScale(s);
}

Vector3 Transform::GetLocalScale() const
{
	return m_local_transform.GetScale();
}

void Transform::SetParentTransform(Transform& parent)
{
	m_parentTransform = &parent; 
	parent.m_childrenTransform.push_back(this);
}

Matrix44 Transform::GetWorldMatrix() const
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

Vector3 Transform::GetWorldPosition() const
{
	return GetWorldMatrix().GetPosition();
}

void Transform::AddChild(Transform& newchild)
{
	m_childrenTransform.push_back(&newchild);
	newchild.m_parentTransform = this;
}

void Transform::DirtyTheChildren()
{
	for(uint i = 0; i < m_childrenTransform.size(); i++)
	{
		m_childrenTransform.at(i)->SetIsDirty();
	}
}

