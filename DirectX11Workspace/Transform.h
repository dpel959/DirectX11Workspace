#pragma once
#include "Component.h"

class Transform : public Component
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

	bool HasParent() { return _parent != nullptr; }
	std::shared_ptr<Transform> GetParent() { return _parent; }
	void SetParent(const std::shared_ptr<Transform>& parent) { _parent = parent; }
	
	const std::vector<std::shared_ptr<Transform>>& GetChildren() { return _children; }
	void AddChild(std::shared_ptr<Transform> child) { _children.push_back(child); }
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
	std::shared_ptr<Transform> _parent; // 내 직속 부모
	std::vector<std::shared_ptr<Transform>> _children; // 내 자식들
};