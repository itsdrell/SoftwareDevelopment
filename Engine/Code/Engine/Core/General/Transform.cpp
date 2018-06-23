#include "Transform.hpp"
#include "..\..\Math\Quaternion.hpp"
#include "..\..\Renderer\Systems\DebugRenderSystem.hpp"

//====================================================================================
const transform_t transform_t::IDENTITY = transform_t();



//====================================================================================
Matrix44 transform_t::GetMatrix() const
{
	Matrix44 result;

	Matrix44 translation = Matrix44::MakeTranslation3D(position);
	Rotator theRotation = rotation.GetAsMatrix();

	
	result.Append(translation);
	result.Append(theRotation);

	// #TODO do the scale

	return result;
}

void transform_t::SetMatrix(Matrix44 const & mat)
{
	Matrix44 result = mat;

	position = mat.GetPosition();

	scale = Vector3(mat.GetRight().GetLength(), mat.GetUp().GetLength(), mat.GetForward().GetLength());

	rotation = rotation.FromMatrix(mat);

}

void transform_t::SetPosition(Vector3 pos)
{
	position = pos;
}


void transform_t::Translate(Vector3 offset)
{
	position += offset;
}

Vector3 transform_t::GetPosition() const
{
	return position;
}

void transform_t::SetRotationEuler(Vector3 theEuler)
{
	rotation = rotation.FromEuler(theEuler);
}

void transform_t::RotateByEuler(Vector3 theEuler)
{
	Vector3 currentRotation = rotation.GetEuler();
	Vector3 newRotation = currentRotation + theEuler;

	rotation = rotation.FromEuler(newRotation);
}

Vector3 transform_t::GetEulerAngles() const
{
	return rotation.GetEuler();
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

void Transform::LookAtLocal(Vector3 & localPos, Vector3 localUp)
{
	Matrix44 lookAt = Matrix44::LookAt(GetLocalPosition(), localPos, localUp);
	SetLocalMatrix(lookAt);
}

void Transform::SetParentTransform(Transform& parent)
{
	m_parentTransform = &parent; 
	//parent.m_childrenTransform.push_back(this);
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

void Transform::SetWorldMatrix(Matrix44& theMatrix)
{
	// New World = Local * parent(world)
	// Local = New World * parent(world) inverse
	
	Matrix44 parent;
	
	if(m_parentTransform != nullptr)
	{
		parent = m_parentTransform->GetWorldMatrix();
	}

	parent.Invert();

	//theMatrix.Append(parent);
	
	m_local_transform.SetMatrix(theMatrix * parent);
}

void Transform::LookAtWorld(Vector3 & worldPos, Vector3 worldUp)
{
	Matrix44 lookAt = Matrix44::LookAt(GetWorldPosition(), worldPos, worldUp);
	SetWorldMatrix(lookAt);
}

void Transform::SimpleMoveTowardPoint(Vector3& position, float speed, float ds)
{
	Vector3 currentPos = GetWorldPosition();
	Vector3 distance = position - currentPos;

	Vector3 amountToMove = distance.Normalize() * (speed * ds);

	TranslateLocal(amountToMove);
}

void Transform::RotateTowards(const Transform& target, float maxDegreesToTurn)
{
	Rotator start = m_local_transform.rotation;
	//Quaternion end = target.m_local_transform.rotation;
	//end.invert();

	Vector3 disp = target.GetWorldPosition() - GetWorldPosition();
	disp.Normalize();

	//Vector3 dir = target.m_local_transform.rotation.get_forward();
	//Vector3 dir = Vector3::FORWARD;
	//Quaternion end = m_local_transform.rotation.LookAt(disp);
	//Quaternion end = Quaternion::FromEuler(disp);
	
	//Quaternion end = target.m_local_transform.rotation;

	Vector3 startRot = m_local_transform.GetEulerAngles();
	Vector3 endRot = target.m_local_transform.GetEulerAngles();
	//Quaternion end = Quaternion::FromEuler(-endRot);

	//Quaternion newRotation = QuaternionRotateTorward(start, end, maxDegreesToTurn);
	//newRotation.invert();
	
	//SetLocalRotationEuler(newRotation.get_euler());

	//DebugRenderLog(0.f, newRotation.ToString(), Rgba::WHITE);
	//m_local_transform.SetRotationEuler();
}

// void Transform::AddChild(Transform& newchild)
// {
// 	m_childrenTransform.push_back(&newchild);
// 	newchild.m_parentTransform = this;
// }
// 
// void Transform::DirtyTheChildren()
// {
// 	for(uint i = 0; i < m_childrenTransform.size(); i++)
// 	{
// 		m_childrenTransform.at(i)->SetIsDirty();
// 	}
// }

