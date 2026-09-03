#include "pch.h"
#include "GeometryHelper.h"

void GeometryHelper::CreateRectangle(const std::shared_ptr<Geometry<VertexTextureData>>& geometry)
{
	std::vector<VertexTextureData> vertices;

	// 버텍스들의 데이터! 이건 CPU에 저장하므로, 아직 CPU의 영역 = RAM에 저장됨.
	vertices.resize(4); // 인덱스 버퍼를 이용해 사각형으로 만들어보자!

	vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
	vertices[0].uv = Vec2(0.f, 1.f);

	vertices[1].position = Vec3(-0.5f, 0.5f, 0.f);
	vertices[1].uv = Vec2(0.f, 0.f);

	vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
	vertices[2].uv = Vec2(1.f, 1.f);

	vertices[3].position = Vec3(0.5f, 0.5f, 0.f);
	vertices[3].uv = Vec2(1.f, 0.f);

	geometry->SetVertices(vertices);

	// 인덱스

	// 벡터를 채우는 것인데, 중요한 것이 있다. 이것은 '그리는(정점을 활용하는) 순서'이다.
	// 시계 방향, 반시계 방향 둘 중 하나를 일관적으로 사용해야한다.

	// 인덱스 버퍼는 사실 버텍스를 버텍스 버퍼에 넣은대로 순서대로 조립하지 말고, 제 순서대로 조립해주세요이다.
	// 그런데 왜 이게 좋냐고 하면, GPU의 캐시때문이다. 셰이더 연산을 하려고 했는데, 이미 이전에 한 것이면 캐시에서 결과값을 가져온다.
	// 그리는 순서 제시도 가능 + 캐시로 인해 자연스레 셰이더 연산을 줄일 수 있는 것이다.

	// 이 0,1,2 번호는 버텍스 버퍼에 넣은 정점의 순서를 의미하는 것이다.

	std::vector<uint32> indices = { 0,1,2,2,1,3 };

	geometry->SetIndices(indices);
}

void GeometryHelper::CreateRectangle(const std::shared_ptr<Geometry<VertexColorData>>& geometry, const Color& color)
{
	std::vector<VertexColorData> vertices;

	// 버텍스들의 데이터! 이건 CPU에 저장하므로, 아직 CPU의 영역 = RAM에 저장됨.
	vertices.resize(4); // 인덱스 버퍼를 이용해 사각형으로 만들어보자!

	vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
	vertices[0].color = color;

	vertices[1].position = Vec3(-0.5f, 0.5f, 0.f);
	vertices[1].color = color;

	vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
	vertices[2].color = color;

	vertices[3].position = Vec3(0.5f, 0.5f, 0.f);
	vertices[3].color = color;

	geometry->SetVertices(vertices);

	// 인덱스

	// 벡터를 채우는 것인데, 중요한 것이 있다. 이것은 '그리는(정점을 활용하는) 순서'이다.
	// 시계 방향, 반시계 방향 둘 중 하나를 일관적으로 사용해야한다.

	// 인덱스 버퍼는 사실 버텍스를 버텍스 버퍼에 넣은대로 순서대로 조립하지 말고, 제 순서대로 조립해주세요이다.
	// 그런데 왜 이게 좋냐고 하면, GPU의 캐시때문이다. 셰이더 연산을 하려고 했는데, 이미 이전에 한 것이면 캐시에서 결과값을 가져온다.
	// 그리는 순서 제시도 가능 + 캐시로 인해 자연스레 셰이더 연산을 줄일 수 있는 것이다.

	// 이 0,1,2 번호는 버텍스 버퍼에 넣은 정점의 순서를 의미하는 것이다.

	std::vector<uint32> indices = { 0,1,2,2,1,3 };

	geometry->SetIndices(indices);
}
