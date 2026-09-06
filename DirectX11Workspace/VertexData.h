#pragma once
#include <d3d11.h>
#include <vector>
#include "Types.h"

// 사실 이렇게 Position, UV 들고 있는 것은 VertexTextureData라고 한다.
struct VertexTextureData
{
	Vec3 position = { 0.f,0.f,0.f };
	Vec2 uv = { 0.f,0.f };

	// desc를 구별해 넘겨주는 방법은 여러개가 있는데, ID를 대조해서 넘겨주는 방법도 있고,
	// 이렇게 struct마다 static을 만들어 넘겨주는 방법도 있다.
	static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};

struct VertexColorData
{
	Vec3 position = { 0.f,0.f,0.f};
	Color color = { 0.f,0.f,0.f,0.f };

	static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};
