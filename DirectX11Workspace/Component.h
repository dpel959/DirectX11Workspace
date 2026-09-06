#pragma once
#include <memory>

class GameObject;

class Component
{
public:
	Component();
	virtual ~Component();

	virtual void Init() = 0;
	virtual void Update() = 0;

	std::shared_ptr<GameObject> GetGameObject() { return _owner.lock(); }

protected:
	std::weak_ptr<GameObject> _owner;
	// 나를 가지는 자는 weak으로 가진다. 당연하다. 나를 가지는 객체는 나를 shared_ptr로 가질 것인데,
	// 나도 shared_ptr로 가지면 순환 참조가 일어난다.
};