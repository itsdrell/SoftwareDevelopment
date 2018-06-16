#include "Enemy.hpp"

Enemy::Enemy(const Vector3& pos)
	: GameObject("Enemy")
{
	m_transform.SetLocalPosition(pos);

}

void Enemy::Update()
{
	GameObject::Update();
}
