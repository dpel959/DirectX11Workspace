#pragma once
#include <memory>
#include "Geometry.h"
#include "VertexData.h"
// 왜 다 include 하나요? 전방선언 안 하나요?
// Geometry는 템플릿 + Geometry 안에 std::vector<T>가 존재한다. 그러므로, VertexData들의 '크기'를 알아야 할 필요가 있다.

// 원래 Maya나 Blender 같은 곳에서 가져온 3D FBX 파일을 파싱해주는 역할을 하는 게 보통.
// 하지만 지금은 없으므로, 단순히 버퍼를 채워주는 용도로 간소화
class GeometryHelper
{
public:
	// 텍스처 전용, Color 전용으로 나누어 대응
	// Q. 근데 왜 return 값으로 오류를 return 하지 않나요? 다렉 함수들은 전부 오류코드를 리턴하던데.
	// A. DX 관련 함수들이 오류코드를 return하는 것은, '외부에서 처리하는 일'들이기 때문이다. 즉, 외부에서 에러가 나도 CPU가 돌리는 C++ 코드 상에서 데이터를 수정하거나, 재실행을 하는 등으로 CPU에서 대처가 가능하다.
	// 그런데, CPU에서만 굴러가는 함수에 오류코드를 반환해봐야, 그것을 대체 누가 해결해주는가? try-catch는 사용할 수 없다. 게임에 사용하기에는 너무 느리기 때문이다.
	// 그러므로, 차라리 Fail-Fast를 따른다. 차라리 터질거면 확실하게, 빠르게 터지라는 것이다. 그래야 문제를 큰 소용 없는 에러 해결 코드를 거쳐 빙 돌아가지 않고 바로 알 수도 있다.
	static void CreateRectangle(const std::shared_ptr<Geometry<VertexTextureData>>& geometry);
	static void CreateRectangle(const std::shared_ptr<Geometry<VertexColorData>>& geometry, const Color& color);
};

