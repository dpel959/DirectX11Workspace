#include "pch.h"
#include "RasterizerState.h"

RasterizerState::RasterizerState(Microsoft::WRL::ComPtr<ID3D11Device> device)
	:_device(device)
{
}

RasterizerState::~RasterizerState()
{
}

void RasterizerState::Create()
{
	D3D11_RASTERIZER_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID; // SOLID : 그냥 있는 그대로 , WIREFRAME : 삼각형 (폴리곤) 단위로만 보여주겠다
	desc.CullMode = D3D11_CULL_BACK; // CULL_NONE, CULL_FRONT(앞에 있는 걸 자름)도 가능. Q. 근데 앞을 왜 잘라? -> A. 카툰 렌더링, 그림자 맵, 스카이박스 등에 사용한다.
	// 컬링에는 여러 단계가 있다. CPU에서 바운딩 박스 안에 있는 물체만 Draw하고 -> NDC 좌표로 끌어오고 RS 단계에서 NDC 밖에 있는 정점들과 후면삼각형을 컬링한다. RS가 '끝난' 이후에는 픽셀 단위로 컬링하기 시작함.
	// OM 단계에서 Depth Test에서 Z-depth를 보고 뒤에 있어 보이지 않는 픽셀들을 컬링함.

	desc.FrontCounterClockwise = false; // 근데 앞, 뒤를 어떻게 판별하는데요? -> '버텍스가 시계 방향으로 구성되면 앞이다' 라고 알려주는 것
	// 우리가 인덱스를 0, 1, 2, 2, 1, 3 으로 시계 방향으로 구성했던 것은 이것 때문. 만약 카메라가 봤을때 버텍스 순서가 반시계 방향이면 이 속성이 false면 잘리게 된다.

	desc.DepthClipEnable = true; // near/far 평면을 벗어난 도형을 클리핑한다. Depth Test와는 별개의 설정이다.
	
	HRESULT hr = _device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());

	assert(SUCCEEDED(hr));
}
