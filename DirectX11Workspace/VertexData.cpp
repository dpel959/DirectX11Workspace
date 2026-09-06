#include "pch.h"
#include "VertexData.h"

std::vector<D3D11_INPUT_ELEMENT_DESC> VertexTextureData::descs
{
	// 밑에 써두는 POSITION, COLOR 같은 이름은 규약은 아니고, HLSL과 연동하기 위해 저장하는 이름
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 지금 넘길 버퍼에 float,float,float는 POSITION 데이터에요
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0} // !!중요!! 앞의 12바이트를 POSITION이 먹었으니, 저희 12바이트부터 시작해요
};
// D3D11_APPEND_ALIGNED_ELEMENT - 알아서 크기를 구해 채워준다.

std::vector<D3D11_INPUT_ELEMENT_DESC> VertexColorData::descs
{
	// 밑에 써두는 POSITION, COLOR 같은 이름은 규약은 아니고, HLSL과 연동하기 위해 저장하는 이름
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 지금 넘길 버퍼에 float,float,float는 POSITION 데이터에요
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0} // !!중요!! 앞의 12바이트를 POSITION이 먹었으니, 저희 12바이트부터 시작해요
};
