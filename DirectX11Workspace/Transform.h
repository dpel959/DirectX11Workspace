#pragma once
#include "Component.h"

// std::enable_shared_from_this<T>는 자기 자신을 가리키는 shared_ptr을 호출하는 shared_from_this()를 호출할 수 있게 하기 위함
// 내부에서 부모 혹은 자식의 shared_ptr은 가질 수 있지만, 자기 자신이 shared_ptr로 관리된다고 해도, 그것을 내가 접근해서 호출할 수는 없다.
// '나를 가리키는 shared_ptr'을 내부에서 호출하기 위해 필요한 클래스. 물론 외부에서도 객체.shared_from_this()로 호출은 가능함. 물론 굳이 객체로 접근 할 수 있으므로 그럴 필요는 잘 없겠다만
class Transform : public Component, public std::enable_shared_from_this<Transform>
{
public:
	Transform();
	~Transform();

	virtual void Init() override;
	virtual void Update() override;

	void UpdateTransform();

	// Local
	const Vec3& GetLocalScale() const { return _localScale; }
	void SetLocalScale(const Vec3& localScale) { _localScale = localScale; UpdateTransform(); }
	const Quaternion& GetLocalRotation() const { return _localRotation; }
	void SetLocalRotation(const Quaternion& localRotation) { _localRotation = localRotation; UpdateTransform(); }
	const Vec3& GetLocalPosition() const { return _localPosition;}
	void SetLocalPosition(const Vec3& localPosition) { _localPosition = localPosition; UpdateTransform(); }

	//World (여기부터 World라고 해보자.)
	const Vec3& GetScale() const { return _scale; }
	void SetScale(const Vec3& scale);
	const Quaternion& GetRotation() const { return _rotation; }
	void SetRotation(const Quaternion& rotation);
	const Vec3& GetPosition() const { return _position; }
	void SetPosition(const Vec3& position);

	Matrix GetWorldmatrix() { return _worldMat; }

	// 계층 관계

	bool HasParent() const { return !_parent.expired(); }
	std::shared_ptr<Transform> GetParent() const { return _parent.lock(); }
	void SetParent(const std::shared_ptr<Transform>& parent);
	
	const std::vector<std::shared_ptr<Transform>>& GetChildren() const { return _children; }
private:
	// local SRT

	Vec3 _localScale = { 1.f,1.f,1.f };
	Quaternion _localRotation = Quaternion::Identity;
	Vec3 _localPosition = { 0.f,0.f,0.f };

	//Cache
	Matrix _localMat = Matrix::Identity;
	Matrix _worldMat = Matrix::Identity;

	Vec3 _scale;
	Quaternion _rotation; // 오일러 각을 사용하면 짐벌 락이 걸리므로, Quaternion 사용
	Vec3 _position;

	Vec3 _right;
	Vec3 _up;
	Vec3 _look;

private:
	// 만약 _parent가 나를 소유하고, 내가 _parent를 shared_ptr로 가리킨다면 순환 참조가 일어난다.
	// 그러므로 의미적으로도, 구조적으로도 부모는 소유하면 안 된다. 부모는 weak_ptr로 가리키자.
	std::weak_ptr<Transform> _parent; 
	std::vector<std::shared_ptr<Transform>> _children; // 내 자식들
};