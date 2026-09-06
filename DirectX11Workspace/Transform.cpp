#include "pch.h"
#include "Transform.h"
#include <algorithm>

Transform::Transform()
{
}

Transform::~Transform()
{
}

void Transform::Init()
{

}

void Transform::Update()
{
}

void Transform::SetParent(const std::shared_ptr<Transform>& parent)
{
	const std::shared_ptr<Transform> currentParent = _parent.lock();
	if (currentParent == parent)
	{
		return;
	}

	const std::shared_ptr<Transform> self = shared_from_this();

	if (currentParent)
	{
		auto& siblings = currentParent->_children;
		siblings.erase(
			std::remove(siblings.begin(), siblings.end(), self),
			siblings.end());
	}

	_parent = parent;

	if (parent)
	{
		parent->_children.push_back(self);
	}

	UpdateTransform();
}

void Transform::UpdateTransform()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateFromQuaternion(_localRotation);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_localMat = matScale * matRotation * matTranslation; // 자신의 부모 좌표계로 올라가는 변환행렬
	if (const std::shared_ptr<Transform> parent = _parent.lock())
	{
		_worldMat = _localMat * parent->GetWorldmatrix(); // _worldMat는 말 그대로 나로부터 world까지 변하는 변환 행렬이다.
		// '내 부모의 worldMat'도 '자신부터 world까지의 변환 행렬'을 가지고 있을 것이므로, 그걸 곱하면 바로 world 좌표계로 가는 것이다.
	}
	else
	{
		_worldMat = _localMat;
	}
	
	// 보통 행렬이
	// [Rx, Ry, Rz, 0] -> Right 벡터
	// [Ux, Uy, Uz, 0] -> Up 벡터
	// [Lx, Ly, Lz, 0] -> Look 벡터
	// [Tx, Ty, Tz, 0] -> 이동

	// 이렇게 되어있다. 이때 스케일이 되어있고 말고는 큰 상관이 없다. 구한 right, up, look 벡터가 단위 벡터가 아닐 뿐.

	// 또한, 이러한 완성된 행렬에서 Decompose(분해)를 해주거나, 정규화 등의 여러 편리 함수들이 있다.

	// Decompose는 Vec3, Quaternion, Vec3로 해준다.

	_worldMat.Decompose(_scale, _rotation, _position);

	// 벡터 * 행렬 곱 함수 
	// TransformCoord: 행렬 마지막 행을 Tx, Ty, Tz, 1으로 한다. (이동 적용 = 포지션화)
	// TransformNormal: 행렬 마지막 행을 Tx, Ty, Tz, 0으로 한다. (이동 미적용 = 방향벡터화)

	// 그리고, 알아둬야 할 비보가 있다. SimpleMath::Vector3에는 Right, Up, Forward, Backward가 있다.
	// 그런데, Forward는 0.f 0.f, -1.f이다. SimpleMath는 '오른손 좌표계'를 따른다.
	// 그러므로 우리는 0.f, 0.f, 1.f인 Backward를 사용해주어야한다.

	_right = Vec3::TransformNormal(Vec3::Right, _worldMat);
	_up = Vec3::TransformNormal(Vec3::Up, _worldMat);
	_look = Vec3::TransformNormal(Vec3::Backward, _worldMat);

	// 내 위치가 바뀌면, 내 SRT (변환 행렬)이 바뀐 것. 즉 child의 위치들도 바뀌어야하므로 내 children도 업데이트해준다.
	// child가 child를 호출하고, 재귀적으로 호출되는 구조이다.
	for (const std::shared_ptr<Transform>& child : _children)
	{
		child->UpdateTransform();
	}
}

// 여기서 worldScale은 'world에서 봤을 때 절대적 Scale'을 뜻 함
// 내가 원하는 world 관점에서의'절대적인 크기'가 1이어도, parentScale이 0.5면, 내 스케일은 2가 된다.
void Transform::SetScale(const Vec3& worldScale)
{
	if (const std::shared_ptr<Transform> parent = _parent.lock())
	{
		Vec3 parentScale = parent->GetScale();
		Vec3 scale = worldScale;
		scale.x /= parentScale.x; 
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;
		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(worldScale);
	}
}

void Transform::SetRotation(const Quaternion& worldRotation)
{
	if (const std::shared_ptr<Transform> parent = _parent.lock())
	{
		Quaternion invParentRot = XMQuaternionInverse(parent->GetRotation()); // 쿼터니언은 날먹이 된다.
		SetLocalRotation(worldRotation * invParentRot);
	}
	else
	{
		SetLocalRotation(worldRotation);
	}
}

void Transform::SetPosition(const Vec3& worldPosition)
{
	if (const std::shared_ptr<Transform> parent = _parent.lock())
	{
		// Vec3 parentPosition = _parent->GetPosition();
		// worldPosition - parentPosition;
		// 이렇게 하면 되지 않나요. 왜 이렇게 어렵게 하나요! 하면. 이 나의 localPosition이라는 것 자체가
		// 부모의 좌표계에서 표현된 position 이기 때문이다. 즉, 부모의 Scale이나 Rotation도 영향을 받은 상태의 translation이다. 순서가 S->R->T니까!
		// 로컬에서 월드로 향하는 행렬이 SRT면, 월드에서 로컬로 향하는 역행열은 T-1R-1S-1 이다. 그러면 저 위의 식은 T-1 만 해준것이다. R-1, S-1도 곱해주어야 하는 것이다!

		// 그러므로, 역행렬을 쓰는 것이 맞다. 위의 것은 '일부'만 한 것임.

		Matrix worldToParentLocalMatrix = parent->GetWorldmatrix().Invert();

		// 사실 전역 말고 객체에서도 Transform을 호출할 수 있다.
		// Transform == TransformCoord와 같은 역할을 하고, 파라미터를 하나만 넣으면, 나와 행렬을 연산해 반환하고,
		// 파라미터를 2개 (벡터, 행렬)을 넣으면, 그 값을 계산하여 호출한 벡터3 객체에게 반환한다.

		// 전역으로 바로 임시 객체로 반환하는 게 가장 깔끔함.

		SetLocalPosition(Vec3::Transform(worldPosition, worldToParentLocalMatrix));
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}
